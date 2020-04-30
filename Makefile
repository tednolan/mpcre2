GTM_INC=/usr/lib/x86_64-linux-gnu/fis-gtm/V6.3-003A_x86_64
libm_pcre2.so: libm_pcre2.c
	gcc -g -Wall -fPIC -shared -o libm_pcre2.so -I$(GTM_INC) libm_pcre2.c -lpcre2-8

clean:
	/bin/rm -f *.o *.so
