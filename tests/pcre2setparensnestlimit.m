;
; Set the maximum depth of nested parentheses in a pattern
;
; There is no useful return value from this function so only pcre2compile failing
; shows the effect.
;

	set cc=$&pcre2compilecontextcreate("NULL")

	set regex="The (Quick (B.*(w?)n) Fox) Perambulated Over The Happily Sleeping Lazy .*og"

	; We expect this compile to succeed
	set code=$&pcre2compile(regex,"PCRE2_CASELESS",.ecode,.eoffset,cc)
	if code=0 write "Regex compile error (",ecode,") at (",eoffset,")",! quit

	set res=$&pcre2setparensnestlimit(cc,2)

	; We expect this compile to fail
	set code=$&pcre2compile(regex,"PCRE2_CASELESS|PCRE2_NO_START_OPTIMIZE",.ecode,.eoffset,cc)
	if code'=0 write "Regex unexpected success",! quit

	write "0",!
	quit
