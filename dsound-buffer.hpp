#ifndef DSOUND_BUFFER_HPP__
#define DSOUND_BUFFER_HPP__

#include <dsound.h>
#include <stdint.h>
#include <rsound.h>
#include <mmreg.h>

class RSoundDSBuffer : public IDirectSoundBuffer
{
   public:
      RSoundDSBuffer(LPCDSBUFFERDESC);

      ULONG __stdcall AddRef();
      HRESULT __stdcall QueryInterface(REFIID, void**);
      ULONG __stdcall Release();

      HRESULT __stdcall GetCaps(LPDSBCAPS);
      HRESULT __stdcall GetCurrentPosition(LPDWORD, LPDWORD);
      HRESULT __stdcall GetFormat(LPWAVEFORMATEX, DWORD, LPDWORD);
      HRESULT __stdcall GetVolume(LPLONG);
      HRESULT __stdcall GetPan(LPLONG);
      HRESULT __stdcall GetFrequency(LPDWORD);
      HRESULT __stdcall GetStatus(LPDWORD);
      HRESULT __stdcall Initialize(LPDIRECTSOUND, LPCDSBUFFERDESC);
      HRESULT __stdcall Lock(DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD);
      HRESULT __stdcall Play(DWORD, DWORD, DWORD);
      HRESULT __stdcall SetCurrentPosition(DWORD);
      HRESULT __stdcall SetFormat(LPCWAVEFORMATEX);
      HRESULT __stdcall SetVolume(LONG);
      HRESULT __stdcall SetPan(LONG);
      HRESULT __stdcall SetFrequency(DWORD);
      HRESULT __stdcall Stop();
      HRESULT __stdcall Unlock(LPVOID, DWORD, LPVOID, DWORD);
      HRESULT __stdcall Restore();

      ssize_t audio_cb(void *data, size_t bytes);
      void err_cb();

   private:
      volatile long refcnt;
      bool is_primary;

      rsound_t *rd;
      struct
      {
         CRITICAL_SECTION crit;
         uint8_t *data;
         unsigned size;
         unsigned ptr;
         unsigned write_ptr;
      } ring;
      DWORD buffer_status;

      WAVEFORMATEXTENSIBLE wfx;

      void set_desc(LPCDSBUFFERDESC desc);
      void destruct();
      unsigned ring_distance(unsigned read_ptr, unsigned write_ptr, unsigned ring_size);

      bool adjust_latency;
      unsigned latency;
      unsigned adjusted_latency(unsigned ptr);
      static unsigned find_latency();

      bool is_float;
      static void convert_float_to_s32(int32_t *out, const float *in, unsigned samples);
};

#endif
