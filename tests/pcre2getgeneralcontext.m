;
; Get the MPCRE2 default general context
;
	set gc=$&pcre2getgeneralcontext()
	if gc=0 write "Cannot get default general context",! quit

	write "0",!
	quit
