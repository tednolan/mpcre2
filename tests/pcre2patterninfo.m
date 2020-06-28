;
; pcre2patterninfo.m
;
; This function can return a lot of stuff that is hard to use from M.
; 
; This is an extremely minimal test
;
	set subject="The Quick Brown Fox Jumped Over"
	set regex="The \rQui(?<xyz>c.*wn )F(?<mno>.*x) J"

	set code=$&pcre2compile(regex,"0",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	; see how many named patterns.  Should be 2
	set res=$&pcre2patterninfo(code,"PCRE2_INFO_NAMECOUNT",.ncount)
	if res'=0 set len=$&pcre2geterrormessage(res,.emsg) write "pcre2patterninfo error ",res," : ",emsg,! quit
	if ncount'=2 write "Unexpected named pattern count: ",ncount,! quit

	; See if NL or CR is explicitly matched for
	set res=$&pcre2patterninfo(code,"PCRE2_INFO_HASCRORLF",.hascrlf)
	if res'=0 set len=$&pcre2geterrormessage(res,.emsg) write "pcre2patterninfo error ",res," : ",emsg,! quit
	if hascrlf'=1 write "pcre2getpatterninfo unexpected hascrlf value ",hascrlf,! quit

	write "0",!
	quit
