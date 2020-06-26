;
; pcre2setnewline Set the newline convention in a compile context
;
	set cc=$&pcre2compilecontextcreate("NULL")

	set res=$&pcre2setnewline(cc,"PCRE2_NEWLINE_CR")
	if res'=0 write "pcre2setnewline fails",! quit

	set res=$&pcre2setnewline(cc,"PCRE2_NEWLINE_LF")
	if res'=0 write "pcre2setnewline fails",! quit

	set res=$&pcre2setnewline(cc,"PCRE2_NEWLINE_CRLF")
	if res'=0 write "pcre2setnewline fails",! quit

	set res=$&pcre2setnewline(cc,"PCRE2_NEWLINE_ANYCRLF")
	if res'=0 write "pcre2setnewline fails",! quit

	set res=$&pcre2setnewline(cc,"PCRE2_NEWLINE_ANY")
	if res'=0 write "pcre2setnewline fails",! quit

	set res=$&pcre2setnewline(cc,"PCRE2_NEWLINE_NUL")
	if res'=0 write "pcre2setnewline fails",! quit

	write "0",!
	quit

