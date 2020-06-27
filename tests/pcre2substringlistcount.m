; pcre2substringlistget.m
;
; This file is copied for all of pcre2substringlistget,
; pcre2substringlistcount, pcre2mstringfromsubstringlist
; and pcre2substringlistfree
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

	;
	; Get the substringlist from the match data
	;
	set res=$&pcre2substringlistget(mdata,.listptr,.lenptr);
	if res'=0 set len=$&pcre2geterrormessage(res,.emsg) write "Error from pcre2substringlistget (",emsg,")",! quit

	; Find out many capture strings we got (should be 3)
	set nstring=$&pcre2getsubstringlistcount(listptr)
	if nstring'=3 write "Unexpected string count from pcre2getsubstringlistcount ",nstring,! quit

	; now check the substrings.  In real code, this would be a for loop (indexed from 0)
	set mstr0=$&pcre2mstringfromsubstringlist(listptr,lenptr,0)
	if mstr0'="The Quick Brown Fox J" write "pcre2mstringfromsubstringlist unexpected substring 0 (",mstr0,")",! quit

	set mstr1=$&pcre2mstringfromsubstringlist(listptr,lenptr,1)
	if mstr1'="ck Brown " write "pcre2mstringfromsubstringlist unexpected substring 1 (",mstr1,")",! quit

	set mstr2=$&pcre2mstringfromsubstringlist(listptr,lenptr,2)
	if mstr2'="ox" write "pcre2mstringfromsubstringlist unexpected substring 2 (",mstr2,")",! quit

	; no return value
	do &pcre2substringlistfree(listptr)

	write "0",!
	quit

