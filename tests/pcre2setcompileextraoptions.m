;
; pcre2setcompileextraoptions 
;
	set gc=$&pcre2getgeneralcontext()
	set cc=$&pcre2compilecontextcreate(gc)

	set res=$&pcre2setcompileextraoptions(cc,"PCRE2_EXTRA_ALLOW_SURROGATE_ESCAPES|PCRE2_EXTRA_MATCH_LINE");
	if res'=0 set len=$&pcre2geterrormessage(res,.emsg) write "pcre2setcompileextraoptions fails: (",emsg,")",! quit

	write "0",!
	quit
