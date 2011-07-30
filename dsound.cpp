#include <dsound.h>
#include "dsound-rsd.hpp"

#include <iostream>

#define dllexport __declspec(dllexport)

#if DIRECTSOUND_VERSION >= 0x0800

dllexport HRESULT WINAPI DirectSoundCreate8(LPCGUID guid, LPDIRECTSOUND8 *ppDS,
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
   std::cerr << "DirectSoundFullDuplexCreate8" << std::endl;
   return DSERR_ALLOCATED;
}

dllexport HRESULT WINAPI DirectSoundCaptureCreate8(LPCGUID,
      LPDIRECTSOUNDCAPTURE8*, LPUNKNOWN)
{
   std::cerr << "DirectSoundCaptureCreate8" << std::endl;
   return DSERR_ALLOCATED;
}

dllexport HRESULT WINAPI GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest)
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


dllexport HRESULT WINAPI DirectSoundCreate(LPCGUID guid, LPDIRECTSOUND *ppDS,
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

dllexport HRESULT WINAPI DirectSoundEnumerateA(LPDSENUMCALLBACKA cb, LPVOID ctx)
{
   std::cerr << "DirectSoundEnumerateA" << std::endl;
   if (cb)
      cb(0, "RSound networked audio", "RSound", ctx);
   return DS_OK;
}

dllexport HRESULT WINAPI DirectSoundEnumerateW(LPDSENUMCALLBACKW cb, LPVOID ctx)
{
   std::cerr << "DirectSoundEnumerateW" << std::endl;
   if (cb)
      cb(0, L"RSound networked audio", L"RSound", ctx);
   return DS_OK;
}

dllexport HRESULT WINAPI DirectSoundCaptureCreate(LPCGUID,
      LPDIRECTSOUNDCAPTURE8*, LPUNKNOWN)
{
   std::cerr << "DirectSoundCaptureCreate" << std::endl;
   return DSERR_ALLOCATED;
}

dllexport HRESULT WINAPI DirectSoundCaptureEnumerateA(
      LPDSENUMCALLBACKA, LPVOID)
{
   std::cerr << "DirectSoundCaptureEnumerateA" << std::endl;
   return DSERR_INVALIDPARAM;
}

dllexport HRESULT WINAPI DirectSoundCaptureEnumerateW(
      LPDSENUMCALLBACKW, LPVOID)
{
   std::cerr << "DirectSoundCaptureEnumerateW" << std::endl;
   return DSERR_INVALIDPARAM;
}

#ifdef __GNUC__

// MinGW seems to be unable to export __stdcall (WINAPI) functions since their
// signature is rather different, so we create some proxy calls and export these instead.
// These just jmp right into the __stdcall variants.

#define WINSYMBOL(symbol) "_" #symbol
#define EXPORT_PROXY(symbol, argcnt) \
   asm( \
         ".text\n" \
         ".globl " WINSYMBOL(symbol) "\n" \
         WINSYMBOL(symbol) ":\n" \
         "\tjmp " WINSYMBOL(symbol) "@" #argcnt "\n" \
         ".section .drectve\n" \
         ".ascii \" -export:\\\"" #symbol "\\\"\"\n" \
         ".text\n" \
      )

EXPORT_PROXY(DirectSoundCreate, 12);
EXPORT_PROXY(DirectSoundCreate8, 12);
EXPORT_PROXY(DirectSoundCaptureCreate, 12);
EXPORT_PROXY(DirectSoundCaptureCreate8, 12);
EXPORT_PROXY(DirectSoundEnumerateA, 8);
EXPORT_PROXY(DirectSoundEnumerateW, 8);
EXPORT_PROXY(DirectSoundCaptureEnumerateA, 8);
EXPORT_PROXY(DirectSoundCaptureEnumerateW, 8);
EXPORT_PROXY(GetDeviceID, 8);
EXPORT_PROXY(DirectSoundFullDuplexCreate, 40);

#endif
