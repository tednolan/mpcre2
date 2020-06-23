;
; Test matching subject strings against a compiled RE.
; There is a lot more that could be tested here.
;
	set subject="The Quick Brown Fox Jumped Over The Lazy Sleeping Dog"
	set subject2="A Man A Plan A Canal Panama"
	set regex="^.*(Fox J.*zy)"
	set code=$&pcre2compile(regex,"0",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set mdata=$&pcre2matchdatacreatefrompattern(code,"0")
	if mdata=0 write "NULL match data pointer returned",! quit

	set mv=$&pcre2match(code,subject,0,0,mdata,0)
	if mv<0 set len=$&pcre2geterrormessage(mv,.emsg) write "Match error ",mv," : ",emsg,! quit
	if mv=0 write "Offsets problem",!

	; Our correct answer is '2' (one more than the highest capturing pair)
	if mv'=2 write "Unexpected match count",! quit

	; Now try a match that should fail with -1 (No match)
	set mv=$&pcre2match(code,subject2,0,0,mdata,0)
	if mv'=-1 write "Unexpected match return value: ",mv,! quit

	write 0,!
	quit
