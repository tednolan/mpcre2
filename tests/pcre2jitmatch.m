;
; Test JIT matching subject strings against a compiled RE.
; This is essentially the same test as for pcre2match with a JIT compile
; thrown in.  More could be done with both tests.
;
	set subject="The Quick Brown Fox Jumped Over The Lazy Sleeping Dog"
	set subject2="A Man A Plan A Canal Panama"
	set regex="^.*(Fox J.*zy)"
	set code=$&pcre2compile(regex,"0",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set res=$&pcre2jitcompile(code,"PCRE2_JIT_COMPLETE")
	if res'=0 set len=$&pcre2geterrormessage(res,.emsg) write "JIT compile fails (",res,") : ",emsg,! quit

	set mdata=$&pcre2matchdatacreatefrompattern(code,"0")
	if mdata=0 write "NULL match data pointer returned",! quit

	set mv=$&pcre2jitmatch(code,subject,0,0,mdata,0)
	if mv<0 set len=$&pcre2geterrormessage(mv,.emsg) write "JIT Match error ",mv," : ",emsg,! quit
	if mv=0 write "Offsets problem",!

	; Our correct answer is '2' (one more than the highest capturing pair)
	if mv'=2 write "Unexpected match count",! quit

	; Now try a match that should fail with -1 (No match)
	set mv=$&pcre2jitmatch(code,subject2,0,0,mdata,0)
	if mv'=-1 write "Unexpected match return value: ",mv,! quit

	write 0,!
	quit
