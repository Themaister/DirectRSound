#include <dsound.h>
#include "dsound-rsd.hpp"
#include "log.hpp"

using namespace Logging;

#define dllexport __declspec(dllexport)

#if DIRECTSOUND_VERSION >= 0x0800

dllexport HRESULT WINAPI DirectSoundCreate8(LPCGUID guid, LPDIRECTSOUND8 *ppDS,
      LPUNKNOWN)
{
   Logging::Init();
   Log("DirectSoundCreate8");
   if (guid && *guid != DSOUND_RSOUND_GUID)
      return DSERR_NODRIVER;

   LPDIRECTSOUND8 rd = new RSoundDS;
   rd->AddRef();
   *ppDS = rd;
   return DS_OK;
}

dllexport HRESULT WINAPI DirectSoundFullDuplexCreate(
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
   Logging::Init();
   Log("DirectSoundFullDuplexCreate");
   return DSERR_ALLOCATED;
}

dllexport HRESULT WINAPI DirectSoundCaptureCreate8(LPCGUID,
      LPDIRECTSOUNDCAPTURE8 *ptr, LPUNKNOWN)
{
   Logging::Init();
   *ptr = (LPDIRECTSOUNDCAPTURE8)0xDEADBEEF;
   Log("DirectSoundCaptureCreate8");
   return DSERR_ALLOCATED;
}

dllexport HRESULT WINAPI GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest)
{
   Logging::Init();
   Log("GetDeviceID");
   if (*pGuidSrc == DSDEVID_DefaultPlayback)
      *pGuidDest = DSOUND_RSOUND_GUID;
   else if (*pGuidSrc == DSDEVID_DefaultVoicePlayback)
      *pGuidDest = DSOUND_RSOUND_GUID;
   else
      return DSERR_INVALIDPARAM;

   return DS_OK;
}

#endif


dllexport HRESULT WINAPI DirectSoundCreate(LPCGUID guid, LPDIRECTSOUND *ppDS,
      LPUNKNOWN)
{
   Logging::Init();
   Log("DirectSoundCreate");
   if (guid && *guid != DSOUND_RSOUND_GUID)
      return DSERR_NODRIVER;

   LPDIRECTSOUND8 rd = new RSoundDS;
   rd->AddRef();
   *ppDS = rd;
   return DS_OK;
}

dllexport HRESULT WINAPI DirectSoundEnumerateA(LPDSENUMCALLBACKA cb, LPVOID ctx)
{
   Logging::Init();
   Log("DirectSoundEnumerateA");
   if (cb)
      cb(0, "RSound networked audio", "RSound", ctx);
   return DS_OK;
}

dllexport HRESULT WINAPI DirectSoundEnumerateW(LPDSENUMCALLBACKW cb, LPVOID ctx)
{
   Logging::Init();
   Log("DirectSoundEnumerateW");
   if (cb)
      cb(0, L"RSound networked audio", L"RSound", ctx);
   return DS_OK;
}

dllexport HRESULT WINAPI DirectSoundCaptureCreate(LPCGUID,
      LPDIRECTSOUNDCAPTURE *ptr, LPUNKNOWN)
{
   Logging::Init();
   *ptr = (LPDIRECTSOUNDCAPTURE)0xDEADBEEF;
   Log("DirectSoundCaptureCreate");
   return DSERR_ALLOCATED;
}

dllexport HRESULT WINAPI DirectSoundCaptureEnumerateA(
      LPDSENUMCALLBACKA, LPVOID)
{
   Logging::Init();
   Log("DirectSoundCaptureEnumerateA");
   return DSERR_INVALIDPARAM;
}

dllexport HRESULT WINAPI DirectSoundCaptureEnumerateW(
      LPDSENUMCALLBACKW, LPVOID)
{
   Logging::Init();
   Log("DirectSoundCaptureEnumerateW");
   return DSERR_INVALIDPARAM;
}

