/*
  MemTroll

  Copyright (C) 2012 Giuseppe Scrivano <gscrivano@gnu.org>.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void *malloc (size_t size)
{
  static unsigned int counter = 10;
  static void* (*next_malloc)(size_t size) = NULL;

  if (next_malloc == NULL)
    {
      char *msg;
      const char *tmp = getenv ("MALLOC_COUNTER");
      if (tmp)
        counter = atoi (tmp);
      next_malloc = dlsym (RTLD_NEXT, "malloc");
      msg = dlerror ();
      if (msg != NULL)
        {
          fprintf (stderr, "dlopen error : %s\n", msg);
          exit (EXIT_FAILURE);
        }
  }

  if (counter-- == 0)
    return NULL;

  return next_malloc (size);
}
