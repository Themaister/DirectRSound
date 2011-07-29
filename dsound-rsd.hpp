#ifndef DSOUND_RSD_HPP__
#define DSOUND_RSD_HPP__

#include <dsound.h>

class RSoundDSCommon
{
   public:
};

class RSoundDS : public IDirectSound8
{
   public:
      HRESULT __stdcall Compact();
      HRESULT __stdcall CreateSoundBuffer(LPCDSBUFFERDESC, LPDIRECTSOUNDBUFFER *, LPUNKNOWN);
      HRESULT __stdcall DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, LPDIRECTSOUNDBUFFER *);
      HRESULT __stdcall GetCaps(LPDSCAPS);
      HRESULT __stdcall GetSpeakerConfig(LPDWORD);
      HRESULT __stdcall Initialize(LPCGUID);
      HRESULT __stdcall SetCooperativeLevel(HWND, DWORD);
      HRESULT __stdcall SetSpeakerConfig(DWORD);
      HRESULT __stdcall VerifyCertification(LPDWORD);

      ULONG __stdcall AddRef();
      HRESULT __stdcall QueryInterface(REFIID, void**);
      ULONG __stdcall Release();
      
   private:
      volatile long refcnt;
};

extern const GUID DSOUND_RSOUND_GUID;

#endif
