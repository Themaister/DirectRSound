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


#ifndef DUMMY_CAPTURE_HPP__
#define DUMMY_CAPTURE_HPP__

#include <dsound.h>

class DummyCapture : public IDirectSoundCapture8
{
   public:
      DummyCapture();
      HRESULT __stdcall CreateCaptureBuffer(LPCDSCBUFFERDESC,
            LPDIRECTSOUNDCAPTUREBUFFER *,
            LPUNKNOWN);

      HRESULT __stdcall GetCaps(LPDSCCAPS);
      HRESULT __stdcall Initialize(LPCGUID);

      ULONG __stdcall AddRef();
      ULONG __stdcall Release();
      HRESULT __stdcall QueryInterface(REFIID, void **);

   private:
      volatile long refcnt;
};

#endif
