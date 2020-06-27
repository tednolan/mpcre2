;
; pcre2setoffsetlimit
; 

	set mc=$&pcre2matchcontextcreate("NULL")
	if mc=0 write "Could not create match context",! quit

	; This function always returns 0, so the test should never fail
	set res=$&pcre2setoffsetlimit(mc,100)
	if res'=0 write "Unknown pcre2setoffsetlimit error",! quit

	write "0",!
	quit
