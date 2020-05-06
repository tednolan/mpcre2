./libm_pcre2.so
regexp: gtm_long_t regexp( I:gtm_string_t*, I:gtm_string_t*, O:gtm_string_t* [1024]) :SIGSAFE
pcre2compile:	gtm_char_t* mpcre2_compile(I:gtm_string_t*, I:gtm_char_t*, O:gtm_long_t*, O:gtm_ulong_t*, I:gtm_char_t*): SIGSAFE
pcre2geterrormessage: gtm_long_t mpcre2_get_error_message(I:gtm_long_t, O:gtm_string_t* [200]):SIGSAFE
pcre2codefree: void mpcre2_code_free(I:gtm_char_t*): SIGSAFE
pcre2matchdatacreatefrompattern: gtm_char_t* mpcre2_match_data_create_from_pattern(I:gtm_char_t*, I:gtm_char_t*): SIGSAFE
pcre2match: gtm_long_t mpcre2_match(I:gtm_char_t*, I:gtm_string_t*, I:gtm_long_t, I:gtm_char_t*, I:gtm_char_t*, I:gtm_char_t*): SIGSAFE
pcre2dfamatch: gtm_long_t mpcre2_dfa_match(I:gtm_char_t*, I:gtm_string_t*, I:gtm_long_t, I:gtm_char_t*, I:gtm_char_t*, I:gtm_char_t*, I:gtm_long_t): SIGSAFE
pcre2matchdatafree: void mpcre2_match_data_free(I:gtm_char_t*): SIGSAFE
