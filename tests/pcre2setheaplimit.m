;
; pcre2setheaplimit
; 
	set mc=$&pcre2matchcontextcreate("NULL")
	if mc=0 write "Could not create match context",! quit

	; This function always returns 0, so the test should never fail
	set res=$&pcre2setheaplimit(mc,100)
	if res'=0 write "Unknown pcre2setheaplimit error",! quit

	write "0",!
	quit
