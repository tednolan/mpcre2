;
; This minimal test of pcre2getmark is taken from the PCRE2 API document
; There is a lot more that could be tested here.
;
	set subject="bc"
	set regex="(*MARK:A)((*MARK:B)a|b)c"
	set code=$&pcre2compile(regex,"0",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set mdata=$&pcre2matchdatacreatefrompattern(code,"0")
	if mdata=0 write "NULL match data pointer returned",! quit

	set mv=$&pcre2match(code,subject,0,0,mdata,0)
	if mv<0 set len=$&pcre2geterrormessage(mv,.emsg) write "Match error ",mv," : ",emsg,! quit
	if mv=0 write "Offsets problem",!

	; Our correct answer is '2'
	if mv'=2 write "Unexpected match count",! quit

	; "When it [the regex] matches "bc", the returned name is A. The B mark is "seen" in
	; the first branch of the group, but it is not on the matching path"

	set mark=$&pcre2getmark(mdata);
	if mark'="A" write "Unexpected mark value (",mark,")",!

	write 0,!
	quit
