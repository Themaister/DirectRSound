#include "dsound-buffer.hpp"
#include <rsound.h>
#include <string.h>

RSoundDSBuffer::RSoundDSBuffer(LPCDSBUFFERDESC desc) : refcnt(0), is_primary(false), rd(0)
{
   if (desc->dwFlags & DSBCAPS_PRIMARYBUFFER)
      // We mix externally, so we really don't care about the primary.
   {
      is_primary = true;
      return;
   }

   memset(&ring, 0, sizeof(ring));
   rsd_init(&rd);
}

RSoundDSBuffer::~RSoundDSBuffer()
{
   if (rd)
   {
      rsd_stop(rd);
      rsd_free(rd);
   }

   if (ring.data)
      delete[] ring.data;
}

ULONG __stdcall RSoundDSBuffer::AddRef()
{
   InterlockedIncrement(&refcnt);
   return refcnt;
}

HRESULT __stdcall RSoundDSBuffer::QueryInterface(REFIID, void**)
{
   return DSERR_UNSUPPORTED;
}

ULONG __stdcall RSoundDSBuffer::Release()
{
   InterlockedDecrement(&refcnt);
   if (refcnt == 0)
   {
      delete this;
      return 0;
   }

   return refcnt;
}
