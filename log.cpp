#include "log.hpp"
#include <stdarg.h>
#include <stdlib.h>

namespace Logging
{
   static FILE *target = 0;

   void SetOutput(FILE *file)
   {
      if (target && target != stdin && target != stdout && target != stderr)
         fclose(target);
      target = file;
   }

   void Log(const char *fmt, ...)
   {
      if (!target)
         return;

      char buf[1024];

      va_list ap;
      va_start(ap, fmt);
      vsnprintf(buf, sizeof(buf), fmt, ap);
      va_end(ap);

      fprintf(target, "[DirectRSound]: %s\n", buf);
      fflush(target);
   }

   void Init()
   {
      static bool inited = false;
      if (inited)
         return;

      fprintf(stderr, "[DirectRSound] startup!\n");
      const char *path = getenv("RSD_LOG_PATH");
      if (path)
      {
         FILE *file = fopen(path, "w");
         if (file)
            SetOutput(file);
      }

      inited = true;
   }
}
