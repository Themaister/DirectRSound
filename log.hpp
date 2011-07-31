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
