/*  DirectRSound - A pure userspace implementation of DirectSound via RSound.
 *  Copyright (C) 2011 - Hans-Kristian Arntzen
 * 
 *  DirectRSound is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  DirectRSound is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with DirectRSound.
 *  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef DSOUND_BUFFER_HPP__
#define DSOUND_BUFFER_HPP__

#include <dsound.h>
#include <stdint.h>
#include <rsound.h>
#include <mmreg.h>

class RSoundDSBuffer : public IDirectSoundBuffer
{
   public:
      RSoundDSBuffer(LPCDSBUFFERDESC);

      ULONG __stdcall AddRef();
      HRESULT __stdcall QueryInterface(REFIID, void**);
      ULONG __stdcall Release();

      HRESULT __stdcall GetCaps(LPDSBCAPS);
      HRESULT __stdcall GetCurrentPosition(LPDWORD, LPDWORD);
      HRESULT __stdcall GetFormat(LPWAVEFORMATEX, DWORD, LPDWORD);
      HRESULT __stdcall GetVolume(LPLONG);
      HRESULT __stdcall GetPan(LPLONG);
      HRESULT __stdcall GetFrequency(LPDWORD);
      HRESULT __stdcall GetStatus(LPDWORD);
      HRESULT __stdcall Initialize(LPDIRECTSOUND, LPCDSBUFFERDESC);
      HRESULT __stdcall Lock(DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD);
      HRESULT __stdcall Play(DWORD, DWORD, DWORD);
      HRESULT __stdcall SetCurrentPosition(DWORD);
      HRESULT __stdcall SetFormat(LPCWAVEFORMATEX);
      HRESULT __stdcall SetVolume(LONG);
      HRESULT __stdcall SetPan(LONG);
      HRESULT __stdcall SetFrequency(DWORD);
      HRESULT __stdcall Stop();
      HRESULT __stdcall Unlock(LPVOID, DWORD, LPVOID, DWORD);
      HRESULT __stdcall Restore();

      ssize_t audio_cb(void *data, size_t bytes);
      void err_cb();

   private:
      volatile long refcnt;
      bool is_primary;

      rsound_t *rd;
      struct
      {
         CRITICAL_SECTION crit;
         uint8_t *data;
         unsigned size;
         unsigned ptr;
         unsigned align;
      } ring;
      DWORD buffer_status;

      WAVEFORMATEXTENSIBLE wfx;

      void set_desc(LPCDSBUFFERDESC desc);
      void set_format(LPWAVEFORMATEX fmt);
      void destruct();

      bool adjust_latency;
      unsigned latency;
      unsigned adjusted_latency(unsigned ptr);
      static unsigned find_latency();
      static bool allow_latency();
      unsigned get_write_ptr();

      bool is_float;
      static void convert_float_to_s32(int32_t *out, const float *in, unsigned samples);

      LONG dsb_volume;
      float gain_volume;
      void apply_volume(void *out, const uint8_t *data, size_t size);
      static float dsbvol_to_gain(LONG db);
};

#endif
