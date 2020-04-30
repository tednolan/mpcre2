./libm_pcre2.so
regexp: gtm_long_t regexp( I:gtm_string_t*, I:gtm_string_t*, O:gtm_string_t* [1024]) :SIGSAFE
pcre2compile:	gtm_char_t* mpcre2_compile(I:gtm_string_t*, I:gtm_char_t*, O:gtm_long_t*, O:gtm_ulong_t*, I:gtm_char_t*): SIGSAFE
pcre2geterrormessage: gtm_long_t mpcre2_get_error_message(I:gtm_long_t, O:gtm_string_t* [200]):SIGSAFE
