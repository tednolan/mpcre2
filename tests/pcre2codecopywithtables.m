;
; pcre2codecopywithtables
;
; We use the same test as for pcre2codecopy
;
	set subject="The Quick Brown Fox Jumped Over The Lazy Sleeping Dog"
	set subject2="A Man A Plan A Canal Panama"
	set regex="^.*(Fox J.*zy)"
	set code=$&pcre2compile(regex,"0",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set code2=$&pcre2codecopywithtables(code)
	if code2=0 write "Code copy failed",! quit

	set mdata=$&pcre2matchdatacreatefrompattern(code2,"0")
	if mdata=0 write "NULL match data pointer returned",! quit

	set mv=$&pcre2match(code2,subject,0,0,mdata,0)
	if mv<0 set len=$&pcre2geterrormessage(mv,.emsg) write "Match error ",mv," : ",emsg,! quit
	if mv=0 write "Offsets problem",!

	; Our correct answer is '2' (one more than the highest capturing pair)
	if mv'=2 write "Unexpected match count",! quit

	; Now try a match that should fail with -1 (No match)
	set mv=$&pcre2match(code2,subject2,0,0,mdata,0)
	if mv'=-1 write "Unexpected match return value: ",mv,! quit

	write 0,!
	quit
