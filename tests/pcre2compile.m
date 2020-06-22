;
; Test compiling a PCRE2 regular expression.  We check that a
; valid expression compiles and that an invalid one does not.
;
	new ecode,eoffset,code,good,bad
	set good="^.*Fox\.$"
	set bad="^.*Fox\.$)("
	set code=$&pcre2compile(good,"PCRE2_CASELESS",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit
	set code=$&pcre2compile(bad,"PCRE2_CASELESS",.ecode,.eoffset,"NULL")
	if code'=0 write "Bad RE (",bad,") accepted!",! quit
	write 0,!
	quit
