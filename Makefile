#
# This is a very old-school, minimal, Makefile.
# Basically just "make" to generate the shared library
#
GTM_INC=/usr/lib/x86_64-linux-gnu/fis-gtm/V6.3-003A_x86_64
LIB=-lpcre2-8
#OPT=-g
OPT=-O2

mpcre2.so: mpcre2.c
	gcc $(OPT) -Wall -fPIC -shared -o mpcre2.so -I$(GTM_INC) mpcre2.c $(LIB)

doxygen:
	sh -c "doxygen > dox.out 2>&1"

clean:
	/bin/rm -f *.o *.so
