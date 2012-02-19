bin_PROGRAMS = memtroll

lib_LTLIBRARIES = libmemtroll.la

libmemtroll_la_SOURCES = libmemtroll.c
memtroll_SOURCES = memtroll.c

lib.h:
		  echo '#define PKGLIBDIR "$(pkglibdir)"' > $@
		  echo '#define PKGLIBEXECDIR "$(pkglibexecdir)"' >> $@

memtroll.c: lib.h
