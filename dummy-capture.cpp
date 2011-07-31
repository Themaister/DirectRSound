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
