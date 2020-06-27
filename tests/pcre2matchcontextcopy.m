;
; This file is copied for pcre2matchcontextcreate, pcre2matchcontextcopy &
; pcre2matchcontextfree
;
	set gc=$&pcre2getgeneralcontext()

	set mc=$&pcre2matchcontextcreate(gc)
	if mc=0 write "Match context create failed",! quit

	set mc2=$&pcre2matchcontextcopy(mc)
	if mc2=0 write "Match context copy failed",! quit

	; There is no return value for this, so we just make sure it doesn't crash the runtime
	do &pcre2matchcontextfree(mc2)

	write "0",!
	quit
