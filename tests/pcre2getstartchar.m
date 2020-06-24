;
; Minimal test of pcre2getstartchar
;
	;            012345678
	set subject="A Man A Plan A Canal Panama"
	set regex="plan.*[abc]"
	set code=$&pcre2compile(regex,"PCRE2_CASELESS",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set mdata=$&pcre2matchdatacreatefrompattern(code,"0")
	if mdata=0 write "NULL match data pointer returned",! quit

	set mv=$&pcre2match(code,subject,0,0,mdata,0)
	if mv<0 set len=$&pcre2geterrormessage(mv,.emsg) write "Match error ",mv," : ",emsg,! quit
	if mv=0 write "Offsets problem",!

	; Our correct answer is '1'
	if mv'=1 write "Unexpected match count: ",mv,! quit

	set pos=$&pcre2getstartchar(mdata)
	if pos'="8" write "Unexpected pos value (",pos,")",!

	write 0,!
	quit
