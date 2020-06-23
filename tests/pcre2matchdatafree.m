;
; Test freeing a match data block.  There is no return value from this function,
; but if it crashes the M runtime, it didn't work.
; 
;
	set regex="^.*(Fox J.*zy)"
	set code=$&pcre2compile(regex,"0",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set mdata=$&pcre2matchdatacreatefrompattern(code,"0")
	if mdata=0 write "NULL match data pointer returned",! quit

	do &pcre2matchdatafree(mdata)

	write 0,!
	quit
