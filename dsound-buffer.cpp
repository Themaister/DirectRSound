#include "dsound-buffer.hpp"
#include <rsound.h>
#include <string.h>

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

ssize_t RSoundDSBuffer::audio_cb(void *data, size_t bytes)
{
   EnterCriticalSection(&ring.crit);
   size_t avail = min(ring.size - ring.ptr, bytes);
   memcpy(data, ring.data + ring.ptr, avail);
   ring.ptr = (ring.ptr + avail) % ring.size;
   memcpy(data, ring.data + ring.ptr, bytes - avail);
   LeaveCriticalSection(&ring.crit);
   return bytes;
}

void RSoundDSBuffer::err_cb()
{}

RSoundDSBuffer::RSoundDSBuffer(LPCDSBUFFERDESC desc) : refcnt(0), is_primary(false), rd(0), buffer_status(0)
{
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

void RSoundDSBuffer::set_desc(LPCDSBUFFERDESC desc)
{
   if (ring.data)
      delete[] ring.data;

   ring.data = new uint8_t[desc->dwBufferBytes];
   ring.size = desc->dwBufferBytes;
   ring.ptr = 0;

   int rate = desc->lpwfxFormat->nSamplesPerSec;
   int channels = desc->lpwfxFormat->nChannels;
   int format = desc->lpwfxFormat->wBitsPerSample == 16 ? RSD_S16_LE : RSD_U8;

   rsd_set_param(rd, RSD_SAMPLERATE, &rate);
   rsd_set_param(rd, RSD_CHANNELS, &channels);
   rsd_set_param(rd, RSD_FORMAT, &format);
   int latency = 128;
   rsd_set_param(rd, RSD_LATENCY, &latency);
}

RSoundDSBuffer::~RSoundDSBuffer()
{
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
   InterlockedIncrement(&refcnt);
   return refcnt;
}

HRESULT __stdcall RSoundDSBuffer::QueryInterface(REFIID, void**)
{
   return DSERR_UNSUPPORTED;
}

ULONG __stdcall RSoundDSBuffer::Release()
{
   InterlockedDecrement(&refcnt);
   if (refcnt == 0)
   {
      delete this;
      return 0;
   }

   return refcnt;
}

HRESULT __stdcall RSoundDSBuffer::GetFrequency(LPDWORD freq)
{
   *freq = wfx.Format.nSamplesPerSec;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::GetPan(LPLONG pan)
{
   *pan = 0;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::GetVolume(LPLONG vol)
{
   *vol = DSBVOLUME_MAX;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::SetFrequency(DWORD freq)
{
   wfx.Format.nSamplesPerSec = freq;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::SetPan(LONG)
{
   return DSERR_CONTROLUNAVAIL;
}

HRESULT __stdcall RSoundDSBuffer::SetVolume(LONG)
{
   return DSERR_CONTROLUNAVAIL;
}

HRESULT __stdcall RSoundDSBuffer::GetCaps(LPDSBCAPS caps)
{
   caps->dwFlags =
      DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2 |
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
   unsigned size = min(dwSizeAllocated, sizeof(wfx));
   memcpy(fmt, &wfx, size);
   *pdwSizeWritten = size;
   return DS_OK;
}

HRESULT __stdcall RSoundDSBuffer::GetStatus(LPDWORD status)
{
   *status = buffer_status;
   return DS_OK;
}

HRESULT RSoundDSBuffer::Initialize(LPDIRECTSOUND, LPCDSBUFFERDESC)
{
   return DSERR_ALREADYINITIALIZED;
}

HRESULT RSoundDSBuffer::SetFormat(LPCWAVEFORMATEX fmt)
{
   memcpy(&wfx, fmt, min(fmt->cbSize, sizeof(wfx)));
   return DS_OK;
}

HRESULT RSoundDSBuffer::GetCurrentPosition(LPDWORD play, LPDWORD write)
{
   EnterCriticalSection(&ring.crit);
   if (play)
      *play = ring.ptr;
   if (write)
      *write = ring.ptr;
   LeaveCriticalSection(&ring.crit);
   return DS_OK;
}

HRESULT RSoundDSBuffer::Lock(
      DWORD offset,
      DWORD bytes,
      LPVOID *ptr1,
      LPDWORD bytes1,
      LPVOID *,
      LPDWORD,
      DWORD flags)
{
   if (ptr1)
      *ptr1 = ring.data + offset;

   *bytes1 = bytes;

   if (offset + bytes > ring.size)
      return DSERR_INVALIDPARAM;

   return DS_OK;
}

HRESULT RSoundDSBuffer::Play(DWORD, DWORD, DWORD)
{
   if (rd)
      rsd_start(rd);
   buffer_status = DSBSTATUS_LOOPING | DSBSTATUS_PLAYING;
   return DS_OK;
}

HRESULT RSoundDSBuffer::Stop()
{
   if (rd)
      rsd_stop(rd);
   buffer_status = 0;
   return DS_OK;
}

HRESULT RSoundDSBuffer::Unlock(LPVOID, DWORD, LPVOID, DWORD)
{
   return DS_OK;
}

HRESULT RSoundDSBuffer::SetCurrentPosition(DWORD pos)
{
   if (pos > ring.size)
      return DSERR_INVALIDPARAM;
   else
   {
      ring.ptr = pos;
      return DS_OK;
   }
}

HRESULT RSoundDSBuffer::Restore()
{
   return DS_OK;
}
