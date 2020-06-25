;
; Find the number of offset pairs in match data
;
	;            012345678901234567
	set subject="hello cruel world"
	set code=$&pcre2compile("hello( c.*l)(.*w.*d)","PCRE2_CASELESS",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set md=$&pcre2matchdatacreatefrompattern(code,"NULL")
	if md=0 write "Create match data failed at ",eoffset," with error ",ecode,! quit

	set mv=$&pcre2match(code,subject,0,0,md,0)
	if mv<0 set len=$&pcre2geterrormessage(mv,.emsg) write "Match error ",mv," : ",emsg,! quit
	if mv=0 write "Offsets problem",!

	; 3 is the correct answer (start of match, then each capture)
	set ovcount=$&pcre2getovectorcount(md)
	if ovcount'=3 write "Unexpected ovcount (",ovcount,")",! quit

	write 0,!
	quit
