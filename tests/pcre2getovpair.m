;
; Get a pointer to the match output vector.
; Get the match vector elements
; This test is duplicated for pcre2getovectorpointer & pcre2getovpair
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

	set ovecpointer=$&pcre2getovectorpointer(md)
	if 0=ovecpointer write "Bad ovecpointer",! quit

	; If it is a good pointer, we should be able to use it.
	; The first ov pair should be (0,17) as the whole string is matched.
	;
	do &pcre2getovpair(ovecpointer,0,.start,.end)
	if (start'=0)&(end'=17) write "ovecpointer bad pair 0",! quit

	; first capture group
	do &pcre2getovpair(ovecpointer,1,.start,.end)
	if (start'=5)&(end'=11) write "ovecpointer bad pair 1",! quit

	; second capture group
	do &pcre2getovpair(ovecpointer,2,.start,.end)
	if (start'=11)&(end'=17) write "ovecpointer bad pair 2",! quit

	write 0,!
	quit
