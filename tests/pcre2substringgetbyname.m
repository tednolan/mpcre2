;
; This file is copied for all of pcre2substringgetbyname,
; pcre2substringgetbynumber, pcre2substringfree & utility function
; pcre2getmstringfrombuf
;
;int pcre2_substring_get_byname(pcre2_match_data *match_data,
;         PCRE2_SPTR name, PCRE2_UCHAR **bufferptr, PCRE2_SIZE *bufflen);
;
;int pcre2_substring_get_bynumber(pcre2_match_data *match_data,
;         uint32_t number, PCRE2_UCHAR **bufferptr, PCRE2_SIZE *bufflen);

	set subject="The Quick Brown Fox Jumped Over"
	set regex="The Qui(c.*wn )F(?<mno>.*x) J"

	set code=$&pcre2compile(regex,"0",.ecode,.eoffset,"NULL")
	if code=0 write "Compile failed at ",eoffset," with error ",ecode,! quit

	set mdata=$&pcre2matchdatacreatefrompattern(code,"0")
	if mdata=0 write "NULL match data pointer returned",! quit

	set mv=$&pcre2match(code,subject,0,0,mdata,0)
	if mv<0 set len=$&pcre2geterrormessage(mv,.emsg) write "Match error ",mv," : ",emsg,! quit
	if mv=0 write "Offsets problem",!

	if mv'=3 write "Unexpected match value ",mv,! quit

	set res=$&pcre2substringgetbyname(mdata,"mno",.bufpt,.buflen)
	if res'=0 write "pcre2substringgetbynme error: ",res,! quit

	set mstr=$&pcre2getmstringfrombuf(bufpt,buflen)
	if mstr'="ox" write "pcre2substringcopybyname unexpected substring (",substring,")",! quit

	; no return value
	do &pcre2substringfree(bufpt)

	set res=$&pcre2substringgetbynumber(mdata,1,.bufpt,.buflen)
	if res'=0 write "pcre2substringgetbynumber error: ",res,! quit

	set mstr2=$&pcre2getmstringfrombuf(bufpt,buflen)
	if mstr2'="ck Brown " write "pcre2substringgetbynumber unexpected substring (",substring,")",! quit

	; no return value
	do &pcre2substringfree(bufpt)

	write "0",!
	quit

