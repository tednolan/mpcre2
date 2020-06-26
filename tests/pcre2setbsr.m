;
; Set \R convention
;
	set gc=$&pcre2getgeneralcontext
	set cc=$&pcre2compilecontextcreate(gc)
	set res=$&pcre2setbsr(cc,"PCRE2_BSR_ANYCRLF")
	if res'=0 set len=$&pcre2geterrormessage(res,.emsg) write "pcre2setbsr fails: (",emsg,")",! quit

	set res=$&pcre2setbsr(cc,"PCRE2_BSR_UNICODE")
	if res'=0 set len=$&pcre2geterrormessage(res,.emsg) write "pcre2setbsr fails: (",emsg,")",! quit

	write "0",!
	quit
