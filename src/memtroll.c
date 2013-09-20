/*
  MemTroll

  Copyright (C) 2012 Giuseppe Scrivano <gscrivano@gnu.org>.
  Copyright (C) 2009-2012 Free Software Foundation, Inc.

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

#include "config.h"
#include "lib.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define LIB_NAME "libmemtroll.so"

/* Taken from GNU coreutils src/stdbuf.c.  */
static void
set_LD_PRELOAD (void)
{
  int ret;
  char *old_libs = getenv ("LD_PRELOAD");
  char *LD_PRELOAD;

  /* Note this would auto add the appropriate search path for "libstdbuf.so":
     gcc stdbuf.c -Wl,-rpath,'$ORIGIN' -Wl,-rpath,$PKGLIBEXECDIR
     However we want the lookup done for the exec'd command not stdbuf.

     Since we don't link against libstdbuf.so add it to PKGLIBEXECDIR
     rather than to LIBDIR.  */
  char const *const search_path[] = {
    PKGLIBEXECDIR,
    ".libs",
    NULL
  };

  char const *const *path = search_path;
  char *libstdbuf;

  while (1)
    {
      struct stat sb;

      if (!**path)              /* system default  */
        {
          libstdbuf = strdup (LIB_NAME);
          if (libstdbuf == NULL)
            exit (EXIT_FAILURE);
          break;
        }
      ret = asprintf (&libstdbuf, "%s/%s", *path, LIB_NAME);
      if (ret < 0)
        exit (EXIT_FAILURE);
      if (stat (libstdbuf, &sb) == 0)   /* file_exists  */
        break;
      free (libstdbuf);

      ++path;
      if ( ! *path)
        error (EXIT_FAILURE, 0, "failed to find %s", LIB_NAME);
    }

  /* FIXME: Do we need to support libstdbuf.dll, c:, '\' separators etc?  */

  if (old_libs)
    ret = asprintf (&LD_PRELOAD, "LD_PRELOAD=%s:%s", old_libs, libstdbuf);
  else
    ret = asprintf (&LD_PRELOAD, "LD_PRELOAD=%s", libstdbuf);

  if (ret < 0)
    exit (EXIT_FAILURE);

  free (libstdbuf);

  ret = putenv (LD_PRELOAD);

  if (ret != 0)
    {
      error (EXIT_FAILURE, errno,
             "failed to update the environment with %s",
             LD_PRELOAD);
    }
}

static void
usage ()
{
  printf ("memtroll -f FROM -t TO [-l LOG] program [options]\n");
}

int
main (int argc, char* const *argv)
{
  int i, opt, from = -1, to = -1;
  set_LD_PRELOAD ();
  FILE *out = stderr;

  while ((opt = getopt (argc, argv, "f:l:t:")) != -1)
    {
      switch (opt)
        {
        case 'f':
          from = atoi (optarg);
          break;

        case 't':
          to = atoi (optarg);
          break;

        case 'l':
          out = fopen (optarg, "w+");
          if (out == NULL)
            error (EXIT_FAILURE, "opening output file");

        default:
          printf ("AA\n");
        }
    }

  if (from < 0 || to < 0)
    {
      usage ();
      exit (EXIT_FAILURE);
    }

  for (i = from; i < to; i++)
    {
      char buffer[12];
      int pid;
      pid_t status = 0;

      sprintf (buffer, "%i", i);
      setenv ("MALLOC_COUNTER", buffer, 1);

      pid = fork ();
      if (pid < 0)
        error (EXIT_FAILURE, "fork");

      if (pid == 0)
        {
          return execvp (argv[optind], argv + optind);
          exit (EXIT_FAILURE);
        }

      do
        {
          pid_t t = waitpid (pid, &status, 0);
          if (t == -1)
            exit (EXIT_FAILURE);

          if (WIFSIGNALED (status) && WTERMSIG (status) == SIGSEGV)
            fprintf (out, "memtroll: segfault at iteration %i\n", i);
        }
      while (!WIFEXITED (status) && !WIFSIGNALED (status));
    }

  fclose (out);

  return 0;
}
