/*  DirectRSound - A pure userspace implementation of DirectSound via RSound.
 *  Copyright (C) 2011 - Hans-Kristian Arntzen
 * 
 *  DirectRSound is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  DirectRSound is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with DirectRSound.
 *  If not, see <http://www.gnu.org/licenses/>.
 */


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
