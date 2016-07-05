CC = gcc
CFLAGS = -g -O0 -D_REENTRANT -Wall
LIBS = -lpthread
RANLIB = ranlib

PROGS = fcntl_r fcntl_rw

all: $(PROGS)

fcntl_r: fcntl_r.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LIBS)

fcntl_rw: fcntl_rw.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LIBS)

clean:
	rm -f $(PROGS) $(CLEANFILES)

check-syntax:
	gcc -o null -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)
