#ifndef CONVERT_HPP__
#define CONVERT_HPP__

#include <stddef.h>

namespace Convert
{
   template <class T>
   void apply_volume(T *out, const T *in, size_t samples, float volume)
   {
      for (size_t i = 0; i < samples; i++)
         out[i] = static_cast<T>(in[i] * volume);
   }
}


#endif
