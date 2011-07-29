#include "dsound-rsd.hpp"

// :D
const GUID DSOUND_RSOUND_GUID = { 1, 2, 3, { 4, 5, 6, 7, 8, 9, 10, 11 } };

HRESULT __stdcall RSoundDS::Compact()
{
   return DS_OK;
}

HRESULT __stdcall RSoundDS::CreateSoundBuffer(LPCDSBUFFERDESC, LPDIRECTSOUNDBUFFER *, LPUNKNOWN)
{
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, LPDIRECTSOUNDBUFFER *)
{
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::GetCaps(LPDSCAPS)
{
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::GetSpeakerConfig(LPDWORD)
{
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::Initialize(LPCGUID)
{
   refcnt = 0;
   return DS_OK;
}

HRESULT __stdcall RSoundDS::SetCooperativeLevel(HWND, DWORD)
{
   return DS_OK;
}

HRESULT __stdcall RSoundDS::SetSpeakerConfig(DWORD)
{
   return DSERR_UNSUPPORTED;
}

ULONG __stdcall RSoundDS::AddRef()
{
   InterlockedIncrement(&refcnt);
   return refcnt;
}

HRESULT __stdcall RSoundDS::QueryInterface(REFIID, void**)
{
   return E_NOINTERFACE;
}

ULONG __stdcall RSoundDS::Release()
{
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
   *cert = DS_UNCERTIFIED;
   return DS_OK;
}

