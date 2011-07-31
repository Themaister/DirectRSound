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
