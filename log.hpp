#ifndef LOG_HPP__
#define LOG_HPP__

#include <stdio.h>

namespace Logging
{
   void Log(const char *fmt, ...);
   void SetOutput(FILE *file);
   void Init();
}

#endif
