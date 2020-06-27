;
; pcre2substringlengthbyname
;
; This file is copied for pcre2substringlengthbynumber
;
	set subject="The Quick Brown Fox Jumped Over"
	set regex="The Qui(c.*wn )F(?<mno>.*x) J"

	set code=$&pcre2compile(regex,"0",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set mdata=$&pcre2matchdatacreatefrompattern(code,"0")
	if mdata=0 write "NULL match data pointer returned",! quit

	set mv=$&pcre2match(code,subject,0,0,mdata,0)
	if mv<0 set len=$&pcre2geterrormessage(mv,.emsg) write "Match error ",mv," : ",emsg,! quit
	if mv=0 write "Offsets problem",!

	if mv'=3 write "Unexpected match value ",mv,! quit

	set res=$&pcre2substringlengthbyname(mdata,"mno",.matchlen)
	if res'=0 write "pcre2substringlengthbyname error: ",res,! quit

	; the match should be "ox" for a len of 2
	if matchlen'=2 write "Unexpected match by name length ",matchlen,! quit

	set res=$&pcre2substringlengthbynumber(mdata,1,.matchlen)
	if res'=0 write "pcre2substringlengthbynumber error: ",res,! quit

	; the match should be "ck Brown " for a len of 9
	if matchlen'=9 write "Unexpected match by number length ",matchlen,! quit

	write "0",!
	quit
