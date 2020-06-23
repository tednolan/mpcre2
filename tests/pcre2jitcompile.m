;
; Test JIT compiling a PCRE2 regular expression.  We check that a
; valid expression compiles and that an invalid one does not.
;
	set good="^.*Fox\.$"
	set code=$&pcre2compile(good,"PCRE2_CASELESS|PCRE2_DOTALL",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit
	set res=$&pcre2jitcompile(code,"PCRE2_JIT_COMPLETE")
	if res'=0 set len=$&pcre2geterrormessage(res,.emsg) write "JIT compile fails (",res,") : ",emsg,! quit
	write 0,!
	quit
