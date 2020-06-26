;
; This file is duplicated for pcre2maketables.m & pcre2setcharactertables
;
	set gc=$&pcre2getgeneralcontext()
	set tab=$&pcre2maketables(gc)
	if tab=0 write "pcre2maketables returned NULL",! quit

	set cc=$&pcre2compilecontextcreate(gc)

	; This function always returns 0 for some reason
	set res=$&pcre2setcharactertables(cc,tab)

	; Set the default tables
	set res=$&pcre2setcharactertables(cc,"NULL")

	write "0",!
	quit
