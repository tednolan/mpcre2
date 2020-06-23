;
; Test creating match dat from a PCRE2 compiled regular expression.
; Our check is that the return value is not 0
;
	new ecode,eoffset,code,good
	set good="^.*Fox\.$"
	set code=$&pcre2compile(good,"PCRE2_CASELESS|PCRE2_DOTALL",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	; The "0" parameter causes the internal default general context to be used
	set mdata=$&pcre2matchdatacreatefrompattern(code,"0")
	if mdata=0 write "NULL match data pointer returned",! quit
	write 0,!
	quit
