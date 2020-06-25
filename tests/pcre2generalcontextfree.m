;
; Free a PCRE general context
;

	set gc=$&pcre2generalcontextcreate("NULL","NULL","NULL")
	if gc=0 write "Could not create general context",! quit

	; no value, so "test" is just not crashing the runtime
	do &pcre2generalcontextfree(gc)

	write 0,!
	quit
