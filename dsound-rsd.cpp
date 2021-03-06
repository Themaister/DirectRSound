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


#include "dsound-rsd.hpp"
#include "dsound-buffer.hpp"
#include "log.hpp"
#include <dsound.h>

using namespace Logging;

// :D
const GUID DSOUND_RSOUND_GUID = { 1, 2, 3, { 4, 5, 6, 7, 8, 9, 10, 11 } };

RSoundDS::RSoundDS() : refcnt(0)
{
   Log("RSoundDS constructor");
}

HRESULT __stdcall RSoundDS::Compact()
{
   Log("RSoundDS::Compact()");
   return DS_OK;
}

HRESULT __stdcall RSoundDS::CreateSoundBuffer(LPCDSBUFFERDESC desc, LPDIRECTSOUNDBUFFER *buffer, LPUNKNOWN)
{
   Log("RSoundDS::CreateSoundBuffer()");
   RSoundDSBuffer *buf = new RSoundDSBuffer(desc);
   buf->AddRef();
   *buffer = buf;

   return DS_OK;
}

HRESULT __stdcall RSoundDS::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, LPDIRECTSOUNDBUFFER *)
{
   Log("RSoundDS::DuplicateSoundBuffer()");
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::GetCaps(LPDSCAPS caps)
{
   Log("RSoundDS::GetCaps()");
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::GetSpeakerConfig(LPDWORD conf)
{
   *conf = DSSPEAKER_STEREO;
   Log("RSoundDS::GetSpeakerConfig()");
   return DS_OK;
}

HRESULT __stdcall RSoundDS::Initialize(LPCGUID)
{
   Log("RSoundDS::Initialize()");
   return DS_OK;
}

HRESULT __stdcall RSoundDS::SetCooperativeLevel(HWND, DWORD)
{
   Log("RSoundDS::SetCooperativeLevel()");
   return DS_OK;
}

HRESULT __stdcall RSoundDS::SetSpeakerConfig(DWORD)
{
   Log("RSoundDS::SetSpeakerConfig()");
   return DS_OK;
}

ULONG __stdcall RSoundDS::AddRef()
{
   Log("RSoundDS::AddRef()");
   InterlockedIncrement(&refcnt);
   return refcnt;
}

HRESULT __stdcall RSoundDS::QueryInterface(REFIID, void**)
{
   Log("RSoundDS::QueryInterface()");
   return E_NOINTERFACE;
}

ULONG __stdcall RSoundDS::Release()
{
   Log("RSoundDS::Release()");
   InterlockedDecrement(&refcnt);
   if (refcnt == 0)
   {
      delete this;
      return 0;
   }

   return refcnt;
}

HRESULT __stdcall RSoundDS::VerifyCertification(LPDWORD cert)
{
   Log("RSoundDS::VerifyCertification()");
   *cert = DS_UNCERTIFIED;
   return DS_OK;
}

