;
; Test converting an error code to a text message.  We
; try to compile an invalid RE.
;
	new ecode,eoffset,code,bad,msg,len,refmsg
	set refmsg="unmatched closing parenthesis"
	set bad="^.*Fox\.$)("
	set code=$&pcre2compile(bad,"PCRE2_CASELESS|PCRE2_DOTALL",.ecode,.eoffset,"NULL")
	if code'=0 write "Bad compile did not generate error",! quit
	set len=$&pcre2geterrormessage(ecode,.emsg)
	if emsg'=refmsg write "Unexpected error message (",emsg,")",! quit
	write 0,!
	quit
