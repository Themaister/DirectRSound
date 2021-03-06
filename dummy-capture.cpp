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


#include "dummy-capture.hpp"
#include "log.hpp"

using namespace Logging;

DummyCapture::DummyCapture() : refcnt(0)
{
   Log("DummyCapture: constructor");
}

HRESULT __stdcall DummyCapture::CreateCaptureBuffer(LPCDSCBUFFERDESC,
            LPDIRECTSOUNDCAPTUREBUFFER *,
            LPUNKNOWN)
{
   Log("DummyCapture::CreateCaptureBuffer");
   return DSERR_NODRIVER;
}

HRESULT __stdcall DummyCapture::GetCaps(LPDSCCAPS)
{
   Log("DummyCapture::GetCaps");
   return DSERR_NODRIVER;
}

HRESULT __stdcall DummyCapture::Initialize(LPCGUID)
{
   Log("DummyCapture::Initialize");
   return DSERR_NODRIVER;
}

ULONG __stdcall DummyCapture::AddRef()
{
   Log("DummyCapture::AddRef");
   InterlockedIncrement(&refcnt);
   return refcnt;
}

ULONG __stdcall DummyCapture::Release()
{
   Log("DummyCapture::Release");
   InterlockedDecrement(&refcnt);
   if (refcnt == 0)
   {
      delete this;
      return 0;
   }

   return refcnt;
}

HRESULT __stdcall DummyCapture::QueryInterface(REFIID, void **)
{
   Log("DummyCapture::QueryInterface");
   return E_NOINTERFACE;
}
