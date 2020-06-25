;
; Create a PCRE general context
;
; This should never really be used, but if used with NULLs will create
; a GC using the system (not M) allocator.  If not used with NULLs, well
; hopefully you got valid pointers from somewhere into M somehow.
;

	set gc=$&pcre2generalcontextcreate("NULL","NULL","NULL")
	if gc=0 write "Could not create general context",! quit

	write 0,!
	quit
