;
; Free a compiled RE.  There is not really any good check for whether this
; worked, although crashing the runtime would be an indication it did not.
;
	new ecode,eoffset,code,good
	set good="^.*Fox\.$"
	set code=$&pcre2compile(good,"PCRE2_CASELESS|PCRE2_DOTALL",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit
	do &pcre2codefree(code)
	write 0,!
	quit
