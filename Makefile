GTM_INC=/usr/lib/x86_64-linux-gnu/fis-gtm/V6.3-003A_x86_64
mpcre2.so: mpcre2.c
	gcc -g -Wall -fPIC -shared -o mpcre2.so -I$(GTM_INC) mpcre2.c -lpcre2-8

clean:
	/bin/rm -f *.o *.so
