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


#ifndef DSOUND_RSD_HPP__
#define DSOUND_RSD_HPP__

#include <dsound.h>

class RSoundDS : public IDirectSound8
{
   public:
      RSoundDS();
      HRESULT __stdcall Compact();
      HRESULT __stdcall CreateSoundBuffer(LPCDSBUFFERDESC, LPDIRECTSOUNDBUFFER *, LPUNKNOWN);
      HRESULT __stdcall DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, LPDIRECTSOUNDBUFFER *);
      HRESULT __stdcall GetCaps(LPDSCAPS);
      HRESULT __stdcall GetSpeakerConfig(LPDWORD);
      HRESULT __stdcall Initialize(LPCGUID);
      HRESULT __stdcall SetCooperativeLevel(HWND, DWORD);
      HRESULT __stdcall SetSpeakerConfig(DWORD);
      HRESULT __stdcall VerifyCertification(LPDWORD);

      ULONG __stdcall AddRef();
      HRESULT __stdcall QueryInterface(REFIID, void**);
      ULONG __stdcall Release();
      
   private:
      volatile long refcnt;
};

extern const GUID DSOUND_RSOUND_GUID;

#endif
