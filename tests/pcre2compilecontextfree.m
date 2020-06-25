;
; This test file is copied for pcre2compilecontextcreate,
; pcre2compilecontextcopy & pcretcompilecontextfree
;
	set gc=$&pcre2getgeneralcontext()

	set cc=$&pcre2compilecontextcreate(gc)
	if cc=0 write "Could not create compile context",! quit

	set cc2=$&pcre2compilecontextcopy(cc)
	if cc2=0 write "Could not copy compile context",! quit

	; no return value, so just don't blow up the M runtime..
	do &pcre2compilecontextfree(cc2)

	write "0",!
	quit
