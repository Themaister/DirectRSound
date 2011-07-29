#include <dsound.h>
#include "dsound-rsd.hpp"

#include <iostream>

#if DIRECTSOUND_VERSION >= 0x0800

HRESULT WINAPI DirectSoundCreate8(LPCGUID guid, LPDIRECTSOUND8 *ppDS,
      LPUNKNOWN)
{
   std::cerr << "DirectSoundCreate8" << std::endl;
   if (guid && *guid != DSOUND_RSOUND_GUID)
      return DSERR_NODRIVER;

   LPDIRECTSOUND8 rd = new RSoundDS;
   rd->AddRef();
   *ppDS = rd;
   return DS_OK;
}

HRESULT WINAPI DirectSoundFullDuplexCreate8(
      LPCGUID,
      LPCGUID,
      LPCDSCBUFFERDESC,
      LPCDSBUFFERDESC,
      HWND,
      DWORD,
      LPDIRECTSOUNDFULLDUPLEX *,
      LPDIRECTSOUNDCAPTUREBUFFER8 *,
      LPDIRECTSOUNDBUFFER8 *,
      LPUNKNOWN)
{
   std::cerr << "DirectSoundFullDuplexCreate8" << std::endl;
   return DSERR_ALLOCATED;
}

HRESULT WINAPI DirectSoundCaptureCreate8(LPCGUID,
      LPDIRECTSOUNDCAPTURE8*, LPUNKNOWN)
{
   std::cerr << "DirectSoundCaptureCreate8" << std::endl;
   return DSERR_ALLOCATED;
}

HRESULT WINAPI GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest)
{
   std::cerr << "GetDeviceID" << std::endl;
   if (*pGuidSrc == DSDEVID_DefaultPlayback)
      *pGuidDest = DSOUND_RSOUND_GUID;
   else if (*pGuidSrc == DSDEVID_DefaultVoicePlayback)
      *pGuidDest = DSOUND_RSOUND_GUID;
   else
      return DSERR_INVALIDPARAM;

   return DS_OK;
}

#endif


HRESULT WINAPI DirectSoundCreate(LPCGUID guid, LPDIRECTSOUND *ppDS,
      LPUNKNOWN)
{
   std::cerr << "DirectSoundCreate" << std::endl;
   if (guid && *guid != DSOUND_RSOUND_GUID)
      return DSERR_NODRIVER;

   LPDIRECTSOUND8 rd = new RSoundDS;
   rd->AddRef();
   *ppDS = rd;
   return DS_OK;
}

HRESULT WINAPI DirectSoundEnumerateA(LPDSENUMCALLBACKA cb, LPVOID ctx)
{
   std::cerr << "DirectSoundEnumerateA" << std::endl;
   if (cb)
      cb(0, "RSound networked audio", "RSound", ctx);
   return DS_OK;
}

HRESULT WINAPI DirectSoundEnumerateW(LPDSENUMCALLBACKW cb, LPVOID ctx)
{
   std::cerr << "DirectSoundEnumerateW" << std::endl;
   if (cb)
      cb(0, L"RSound networked audio", L"RSound", ctx);
   return DS_OK;
}

HRESULT WINAPI DirectSoundCaptureCreate(LPCGUID,
      LPDIRECTSOUNDCAPTURE8*, LPUNKNOWN)
{
   std::cerr << "DirectSoundCaptureCreate" << std::endl;
   return DSERR_ALLOCATED;
}

HRESULT WINAPI DirectSoundCaptureEnumerateA(
      LPDSENUMCALLBACKA, LPVOID)
{
   std::cerr << "DirectSoundCaptureEnumerateA" << std::endl;
   return DSERR_INVALIDPARAM;
}

HRESULT WINAPI DirectSoundCaptureEnumerateW(
      LPDSENUMCALLBACKW, LPVOID)
{
   std::cerr << "DirectSoundCaptureEnumerateW" << std::endl;
   return DSERR_INVALIDPARAM;
}

