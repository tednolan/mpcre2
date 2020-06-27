;
; pcre2substringnumberfromname.m
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

	; match name "mno" should be number 2.
	set res=$&pcre2substringnumberfromname(code,"mno")
	if res<0 set len=$&pcre2geterrormessage(res,.emsg) write "Error from pcre2substringnumberfromname (",emsg,")",! quit
	if res'=2 write "Unexpected result from pcre2substringnumberfromname: ",res,! quit

	write "0",!
	quit
