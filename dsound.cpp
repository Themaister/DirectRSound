#include <dsound.h>
#include "dsound-rsd.hpp"
#include "log.hpp"
#include "dummy-capture.hpp"

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

   DummyCapture *cap = new DummyCapture;
   cap->AddRef();
   *ptr = cap;

   Log("DirectSoundCaptureCreate8");
   return DS_OK;
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
   {
      GUID tmp = DSOUND_RSOUND_GUID;
      cb(&tmp, "RSound networked audio", "RSound", ctx);
   }
   return DS_OK;
}

dllexport HRESULT WINAPI DirectSoundEnumerateW(LPDSENUMCALLBACKW cb, LPVOID ctx)
{
   Logging::Init();
   Log("DirectSoundEnumerateW");
   if (cb)
   {
      GUID tmp = DSOUND_RSOUND_GUID;
      cb(&tmp, L"RSound networked audio", L"RSound", ctx);
   }
   return DS_OK;
}

dllexport HRESULT WINAPI DirectSoundCaptureCreate(LPCGUID,
      LPDIRECTSOUNDCAPTURE *ptr, LPUNKNOWN)
{
   Logging::Init();

   DummyCapture *cap = new DummyCapture;
   cap->AddRef();
   *ptr = cap;

   Log("DirectSoundCaptureCreate");
   return DS_OK;
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

dllexport HRESULT WINAPI DllCanUnloadNow(void)
{
   Log("DllCanUnloadNow");
   return FALSE;
}

// TODO: Fixup ...
dllexport HRESULT WINAPI DllGetClassObject(REFCLSID, REFIID, LPVOID *)
{
   Log("DllGetClassObject");
   return CLASS_E_CLASSNOTAVAILABLE;
}

#if defined(__GNUC__) && defined(EXPORT_PROXY_SYMBOLS)

// Apparently we need to export the DirectSound functions in a proper order since some
// programs import by ordinal. We have to match the original binary.
// MinGW orders alphabethically so make sure these hook calls will come first.

#define WINSYMBOL(symbol) "_" #symbol
#define HOOKSYMBOL(index) "AAAAAHookCall" #index
#define HOOKSYMBOL_(index) "_" HOOKSYMBOL(index)
#define EXPORT_PROXY(index, target) \
   asm( \
         ".text\n" \
         ".globl " HOOKSYMBOL_(index) "\n" \
         HOOKSYMBOL_(index)  ":\n" \
         "\tjmp " WINSYMBOL(target) "\n" \
         ".section .drectve\n" \
         ".ascii \" -export:\\\"" HOOKSYMBOL(index) "\\\"\"\n" \
         ".text\n" \
   )

EXPORT_PROXY(00, DirectSoundCreate);
EXPORT_PROXY(01, DirectSoundEnumerateA);
EXPORT_PROXY(02, DirectSoundEnumerateW);
EXPORT_PROXY(03, DllCanUnloadNow);
EXPORT_PROXY(04, DllGetClassObject);
EXPORT_PROXY(05, DirectSoundCaptureCreate);
EXPORT_PROXY(06, DirectSoundCaptureEnumerateA);
EXPORT_PROXY(07, DirectSoundCaptureEnumerateW);

#if DIRECTSOUND_VERSION >= 0x0800
EXPORT_PROXY(08, GetDeviceID);
EXPORT_PROXY(09, DirectSoundFullDuplexCreate);
EXPORT_PROXY(10, DirectSoundCreate8);
EXPORT_PROXY(11, DirectSoundCaptureCreate8);
#endif

#endif
