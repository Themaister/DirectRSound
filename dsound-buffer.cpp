#include "dsound-buffer.hpp"
#include <rsound.h>
#include <math.h>
#include <string.h>
#include "log.hpp"
#include "convert.hpp"

#undef min
#undef max
#include <algorithm>
#include <limits>


#define WAVE_FORMAT_IEEE_FLOAT__ 0x0003

static const GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT__ = {
   0x00000003,
   0x0000,
   0x0010,
   { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
};

using namespace Logging;

namespace Callback
{
   extern "C"
   {
      static ssize_t audio_cb(void *data, size_t bytes, void *userdata)
      {
         RSoundDSBuffer *buf = reinterpret_cast<RSoundDSBuffer*>(userdata);
         return buf->audio_cb(data, bytes);
      }

      static void err_cb(void *userdata)
      {
         RSoundDSBuffer *buf = reinterpret_cast<RSoundDSBuffer*>(userdata);
         buf->err_cb();
      }
   }
}

float RSoundDSBuffer::dsbvol_to_gain(LONG db)
{
   return powf(10.0f, db * 0.01f / 20.0f);
}

void RSoundDSBuffer::apply_volume(void *out, const uint8_t *data, size_t size)
{
   if (dsb_volume == DSBVOLUME_MAX)
   {
      memcpy(out, data, size);
      return;
   }

   switch (wfx.Format.wBitsPerSample)
   {
      case 8:
         Convert::apply_volume(
               reinterpret_cast<uint8_t*>(out),
               data,
               size / sizeof(uint8_t), gain_volume);
         break;

      case 16:
         Convert::apply_volume(
               reinterpret_cast<int16_t*>(out),
               reinterpret_cast<const int16_t*>(data),
               size / sizeof(int16_t), gain_volume);
         break;

      case 32:
         Convert::apply_volume(
               reinterpret_cast<int32_t*>(out),
               reinterpret_cast<const int32_t*>(data),
               size / sizeof(int32_t), gain_volume);
         break;

      default:
         memcpy(out, data, size);
   }
}

ssize_t RSoundDSBuffer::audio_cb(void *data_, size_t bytes)
{
   uint8_t *data = static_cast<uint8_t*>(data_);
   EnterCriticalSection(&ring.crit);
   size_t avail = std::min(ring.size - ring.ptr, bytes);

   apply_volume(data, ring.data + ring.ptr, avail);
   ring.ptr = (ring.ptr + avail) % ring.size;
   data += avail;

   apply_volume(data, ring.data + ring.ptr, bytes - avail);
   ring.ptr += bytes - avail;

   LeaveCriticalSection(&ring.crit);

   // Not correct.
   if (!(buffer_status & DSBSTATUS_LOOPING))
      Stop();

   return bytes;
}

void RSoundDSBuffer::err_cb()
{
   buffer_status = DSBSTATUS_BUFFERLOST;
}

RSoundDSBuffer::RSoundDSBuffer(LPCDSBUFFERDESC desc) : refcnt(0), is_primary(false), rd(0), buffer_status(0), dsb_volume(DSBVOLUME_MAX), gain_volume(1.0f)
{
   Log("RSoundDSBuffer constructor");
   if (desc->dwFlags & DSBCAPS_PRIMARYBUFFER)
      // We mix externally, so we really don't care about the primary.
   {
      is_primary = true;
      return;
   }

   memset(&ring, 0, sizeof(ring));
   InitializeCriticalSection(&ring.crit);
   rsd_init(&rd);
   rsd_set_callback(rd, Callback::audio_cb, Callback::err_cb, 256, this);

   set_desc(desc);
}

unsigned RSoundDSBuffer::find_latency()
{
   unsigned latency_ms = 128; // Reasonable default.
   const char *lat = getenv("RSD_LATENCY");
   if (lat)
      latency_ms = strtoul(lat, 0, 0);

   return latency_ms;
}

void RSoundDSBuffer::set_format(LPWAVEFORMATEX fmt)
{
   int rate = fmt->nSamplesPerSec;
   int channels = fmt->nChannels;
   int format;
   switch (fmt->wBitsPerSample)
   {
      case 8:
         format = RSD_U8;
         break;
      case 16:
         format = RSD_S16_LE;
         break;
      case 32:
         format = RSD_S32_LE;
         break;

      default:
         format = RSD_S16_LE;
         fmt->wBitsPerSample = 16;
   }

   int latency_ms = find_latency();
   if (rd)
   {
      rsd_set_param(rd, RSD_SAMPLERATE, &rate);
      rsd_set_param(rd, RSD_CHANNELS, &channels);
      rsd_set_param(rd, RSD_FORMAT, &format);
      rsd_set_param(rd, RSD_LATENCY, &latency_ms);
   }

   is_float = false;
   if (fmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
   {
      WAVEFORMATEXTENSIBLE *ext = (WAVEFORMATEXTENSIBLE*)fmt;
      if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT__)
         is_float = true;
   }
   else if (fmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT__)
      is_float = true;

   Log("=============================");
   Log("Buffer info:");
   Log("\tSamplerate: %d", rate);
   Log("\tChannels: %d", channels);
   Log("\tBits: %d", fmt->wBitsPerSample);
   Log("\tIEEE float: %s", is_float ? "true" : "false");
   Log("=============================");

   latency = (latency_ms * rate * channels * fmt->wBitsPerSample) / (8 * 1000);

   // To compensate for added latency in rsound itself we adjust the read pointer to reflect this. Only do this when the total latency is big enough (video playing usually).
   adjust_latency = 4 * latency < ring.size;
   Log(adjust_latency ?
         "Using latency compensation!" :
         "Not using latency compensation!");
}

void RSoundDSBuffer::set_desc(LPCDSBUFFERDESC desc)
{
   if (ring.data)
      delete[] ring.data;

   ring.data = new uint8_t[desc->dwBufferBytes];
   ring.size = desc->dwBufferBytes;
   memset(ring.data, 0, ring.size);

   ring.ptr = 0;
   ring.write_ptr = desc->dwBufferBytes >> 1;
   
   if (desc->lpwfxFormat)
      SetFormat(desc->lpwfxFormat);
}

unsigned RSoundDSBuffer::adjusted_latency(unsigned ptr)
{
   return (ring.size + ring.ptr - latency) % ring.size;
}

void RSoundDSBuffer::destruct()
{
   Log("RSoundDSBuffer::destruct");
   if (is_primary)
      return;

   if (rd)
   {
      rsd_stop(rd);
      rsd_free(rd);
   }

   DeleteCriticalSection(&ring.crit);

   if (ring.data)
      delete[] ring.data;
}

ULONG __stdcall RSoundDSBuffer::AddRef()
{
   Log("RSoundDSBuffer::AddRef");
   InterlockedIncrement(&refcnt);
   return refcnt;
}

HRESULT __stdcall RSoundDSBuffer::QueryInterface(REFIID, void**)
{
   Log("RSoundDSBuffer::QueryInterface");
   return DSERR_UNSUPPORTED;
}

ULONG __stdcall RSoundDSBuffer::Release()
{
   Log("RSoundDSBuffer::Release");
   InterlockedDecrement(&refcnt);
   if (refcnt == 0)
   {
      destruct();
      delete this;
      return 0;
   }

   return refcnt;
}

HRESULT __stdcall RSoundDSBuffer::GetFrequency(LPDWORD freq)
{
   Log("RSoundDSBuffer::GetFrequency");
   *freq = wfx.Format.nSamplesPerSec;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::GetPan(LPLONG pan)
{
   Log("RSoundDSBuffer::GetPan");
   *pan = 0;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::GetVolume(LPLONG vol)
{
   Log("RSoundDSBuffer::GetVolume");
   EnterCriticalSection(&ring.crit);
   *vol = dsb_volume;
   LeaveCriticalSection(&ring.crit);
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::SetFrequency(DWORD freq)
{
   Log("RSoundDSBuffer::SetFrequency");
   wfx.Format.nSamplesPerSec = freq;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::SetPan(LONG)
{
   Log("RSoundDSBuffer::SetPan");
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::SetVolume(LONG vol)
{
   Log("RSoundDSBuffer::SetVolume");
   EnterCriticalSection(&ring.crit);
   dsb_volume = vol;
   gain_volume = dsbvol_to_gain(dsb_volume);
   LeaveCriticalSection(&ring.crit);

   Log("Setting volume to %.1f dB, Gain: %.3f", vol / 100.0f, gain_volume);
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::GetCaps(LPDSBCAPS caps)
{
   Log("RSoundDSBuffer::GetCaps");
   caps->dwFlags =
      DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2 |
      DSBCAPS_CTRLVOLUME |
      DSBCAPS_LOCSOFTWARE | (is_primary ? DSBCAPS_PRIMARYBUFFER : 0);

   caps->dwBufferBytes = ring.size;
   caps->dwUnlockTransferRate = 0;
   caps->dwPlayCpuOverhead = 0;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::GetFormat(LPWAVEFORMATEX fmt,
      DWORD dwSizeAllocated,
      LPDWORD pdwSizeWritten)
{
   Log("RSoundDSBuffer::GetFormat");
   unsigned size = std::min((size_t)dwSizeAllocated, sizeof(wfx));
   memcpy(fmt, &wfx, size);

   if (pdwSizeWritten)
      *pdwSizeWritten = size;

   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::GetStatus(LPDWORD status)
{
   Log("RSoundDSBuffer::GetStatus");
   *status = buffer_status;
   return DS_OK;
}

HRESULT RSoundDSBuffer::Initialize(LPDIRECTSOUND, LPCDSBUFFERDESC)
{
   Log("RSoundDSBuffer::Initialize");
   return DSERR_ALREADYINITIALIZED;
}

HRESULT RSoundDSBuffer::SetFormat(LPCWAVEFORMATEX fmt)
{
   Log("RSoundDSBuffer::SetFormat");
   memcpy(&wfx, fmt, std::min((size_t)fmt->cbSize + sizeof(WAVEFORMATEX), sizeof(wfx)));
   set_format((LPWAVEFORMATEX)&wfx);
   return DS_OK;
}

HRESULT RSoundDSBuffer::GetCurrentPosition(LPDWORD play, LPDWORD write)
{
   EnterCriticalSection(&ring.crit);
   if (play)
   {
      if (adjust_latency)
         *play = adjusted_latency(ring.ptr);
      else
         *play = ring.ptr;
   }
   if (write)
      *write = ring.write_ptr;
   LeaveCriticalSection(&ring.crit);

   Log("RSoundDSBuffer::GetCurrentPosition: Play = %u, Write = %u", ring.ptr, ring.write_ptr);

   return DS_OK;
}

HRESULT RSoundDSBuffer::Lock(
      DWORD offset,
      DWORD bytes,
      LPVOID *ptr1,
      LPDWORD bytes1,
      LPVOID *ptr2,
      LPDWORD bytes2,
      DWORD flags)
{
   Log("RSoundDSBuffer::Lock: Offset = %d, Bytes = %d, FromWrite = %d", offset, bytes, flags & DSBLOCK_FROMWRITECURSOR ? 1 : 0);
   DWORD status;
   GetStatus(&status);
   if (status & DSBSTATUS_BUFFERLOST)
      return DSERR_BUFFERLOST;

   if (flags & DSBLOCK_FROMWRITECURSOR)
      offset = ring.write_ptr;

   if (bytes > ring.size || offset > ring.size)
      return DSERR_INVALIDPARAM;

   if (flags & DSBLOCK_ENTIREBUFFER)
   {
      *ptr1 = ring.data;
      *bytes1 = ring.size;
      return DS_OK;
   }

   size_t avail = std::min(ring.size - offset, bytes);

   if (ptr1)
   {
      *ptr1 = ring.data + offset;
      *bytes1 = avail;
      if (ptr2)
      {
         *ptr2 = avail < bytes ? ring.data : 0;
         *bytes2 = bytes - avail;
      }
   }

   return DS_OK;
}

HRESULT RSoundDSBuffer::Play(DWORD, DWORD, DWORD flags)
{
   Log("RSoundDSBuffer::Play");
   if (rd)
   {
      if (rsd_start(rd) < 0)
      {
         buffer_status = 0;
         return DSERR_BUFFERLOST;
      }
      buffer_status = ((flags & DSBPLAY_LOOPING) ? DSBSTATUS_LOOPING : 0) | DSBSTATUS_PLAYING;
   }
   return DS_OK;
}

HRESULT RSoundDSBuffer::Stop()
{
   Log("RSoundDSBuffer::Stop");
   if (rd)
      rsd_stop(rd);
   buffer_status = 0;

   if (ring.data)
   {
      ring.ptr = 0;
      ring.write_ptr = ring.size >> 1;
      memset(ring.data, 0, ring.size);
   }

   return DS_OK;
}

unsigned RSoundDSBuffer::ring_distance(unsigned read_ptr, unsigned write_ptr, unsigned ring_size)
{
   if (write_ptr < read_ptr)
      return write_ptr + ring_size - read_ptr;
   else
      return write_ptr - read_ptr;
}

void RSoundDSBuffer::convert_float_to_s32(int32_t *out, const float *in, unsigned samples)
{
   for (unsigned i = 0; i < samples; i++)
   {
      float val = in[i];
      if (val > 1.0f)
         val = 1.0f;
      else if (val < -1.0f)
         val = -1.0f;
      out[i] = static_cast<int32_t>(val * static_cast<float>(std::numeric_limits<int32_t>::max()));
   }
}

HRESULT RSoundDSBuffer::Unlock(LPVOID ptr1, DWORD bytes1, LPVOID ptr2, DWORD bytes2)
{
   Log("RSoundDSBuffer::Unlock: bytes1 = %d, bytes2 = %d", bytes1, bytes2);
   if (bytes2)
      ring.write_ptr = bytes2;
   else
      ring.write_ptr = (ring.write_ptr + bytes1) % ring.size;

   EnterCriticalSection(&ring.crit);
   unsigned distance = ring_distance(ring.ptr, ring.write_ptr, ring.size);
   if (distance < ring.size / 2)
      ring.write_ptr = (ring.ptr + ring.size / 2) % ring.size;

   if (is_float)
   {
      if (ptr1 && bytes1)
         convert_float_to_s32(reinterpret_cast<int32_t*>(ptr1), reinterpret_cast<float*>(ptr1),
               bytes1 / sizeof(float));
      if (ptr2 && bytes2)
         convert_float_to_s32(reinterpret_cast<int32_t*>(ptr2), reinterpret_cast<float*>(ptr2),
               bytes2 / sizeof(float));
   }

   LeaveCriticalSection(&ring.crit);

   return DS_OK;
}

HRESULT RSoundDSBuffer::SetCurrentPosition(DWORD pos)
{
   Log("RSoundDSBuffer::SetCurrentPosition: Pos = %d", pos);
   if (pos > ring.size)
      return DSERR_INVALIDPARAM;
   else
   {
      EnterCriticalSection(&ring.crit);
      ring.ptr = pos;
      ring.write_ptr = (ring.ptr + ring.size / 2) % ring.size;
      LeaveCriticalSection(&ring.crit);
      return DS_OK;
   }
}

HRESULT RSoundDSBuffer::Restore()
{
   Log("RSoundDSBuffer::Restore");
   return Play(0, 0, DSBPLAY_LOOPING);
}
