;
; Copy a PCRE general context
;
; This should never really be a need for this.
;

	set gc=$&pcre2generalcontextcreate("NULL","NULL","NULL")
	if gc=0 write "Could not create general context",! quit

	set copy=$&pcre2generalcontextcopy(gc)
	if copy=0 write "Couldnot copy general context",! quit

	write 0,!
	quit
