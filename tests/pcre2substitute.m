;
; Test substituting a new string for a matched string
;
	set regex="Lazy.*og"
	set subject="The Lazy Dog Jumped over the other lazy frog"
	set replacement="torpid animal"
	set result1="The torpid animal Jumped over the other lazy frog"
	set rlen1=$length(result1)
	set result2="The torpid animal Jumped over the other torpid animal"
	set rlen2=$length(result2)

	set code=$&pcre2compile(regex,"PCRE2_CASELESS|PCRE2_UNGREEDY",.ecode,.eoffset,"NULL")
        if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	; For this simple test, we don't use match data or match context

	set mv=$&pcre2substitute(code,subject,0,0,"NULL","NULL",replacement,.newstring,.newlen)
	if newstring'=result1 write "Single replacement failed: (",newstring,")",! quit
	if newlen'=rlen1 write "Single replacement length miscalculated: ",newlen,!

	; Re-run with global replace
	set mv=$&pcre2substitute(code,subject,0,"PCRE2_SUBSTITUTE_GLOBAL","NULL","NULL",replacement,.newstring,.newlen)
	if newstring'=result2 write "Multiple replacement failed: (",newstring,")",! quit
	if newlen'=rlen2 write "Multiple replacement length miscalculated: ",newlen,!

	write 0,!
	quit
