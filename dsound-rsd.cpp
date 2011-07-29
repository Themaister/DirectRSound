#include "dsound-rsd.hpp"
#include "dsound-buffer.hpp"

// :D
const GUID DSOUND_RSOUND_GUID = { 1, 2, 3, { 4, 5, 6, 7, 8, 9, 10, 11 } };

RSoundDS::RSoundDS() : refcnt(0)
{}

HRESULT __stdcall RSoundDS::Compact()
{
   return DS_OK;
}

HRESULT __stdcall RSoundDS::CreateSoundBuffer(LPCDSBUFFERDESC desc, LPDIRECTSOUNDBUFFER *buffer, LPUNKNOWN)
{
   RSoundDSBuffer *buf = new RSoundDSBuffer(desc);
   buf->AddRef();
   *buffer = buf;

   return DS_OK;
}

HRESULT __stdcall RSoundDS::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, LPDIRECTSOUNDBUFFER *)
{
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::GetCaps(LPDSCAPS caps)
{
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::GetSpeakerConfig(LPDWORD)
{
   return DSERR_UNSUPPORTED;
}

HRESULT __stdcall RSoundDS::Initialize(LPCGUID)
{
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

