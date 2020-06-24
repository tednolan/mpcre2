$MPCRE2_SRC/mpcre2.so
pcre2compile:	gtm_char_t* mpcre2_compile(I:gtm_string_t*, I:gtm_char_t*, O:gtm_long_t*, O:gtm_ulong_t*, I:gtm_char_t*): SIGSAFE
pcre2geterrormessage: gtm_long_t mpcre2_get_error_message(I:gtm_long_t, O:gtm_string_t* [200]):SIGSAFE
pcre2codefree: void mpcre2_code_free(I:gtm_char_t*): SIGSAFE
pcre2matchdatacreatefrompattern: gtm_char_t* mpcre2_match_data_create_from_pattern(I:gtm_char_t*, I:gtm_char_t*): SIGSAFE
pcre2match: gtm_long_t mpcre2_match(I:gtm_char_t*, I:gtm_string_t*, I:gtm_long_t, I:gtm_char_t*, I:gtm_char_t*, I:gtm_char_t*): SIGSAFE
pcre2dfamatch: gtm_long_t mpcre2_dfa_match(I:gtm_char_t*, I:gtm_string_t*, I:gtm_long_t, I:gtm_char_t*, I:gtm_char_t*, I:gtm_char_t*, I:gtm_long_t): SIGSAFE
pcre2matchdatafree: void mpcre2_match_data_free(I:gtm_char_t*): SIGSAFE
pcre2substitute: gtm_long_t mpcre2_substitute(I:gtm_char_t*, I:gtm_string_t*, I:gtm_long_t, I:gtm_char_t*, I:gtm_char_t*, I:gtm_char_t*,I:gtm_string_t*,O:gtm_string_t* [1048576],O:gtm_long_t*): SIGSAFE
pcre2jitcompile: gtm_long_t mpcre2_jit_compile(I:gtm_char_t*, I:gtm_char_t*): SIGSAFE
pcre2getmark: gtm_string_t* mpcre2_get_mark(I:gtm_char_t*): SIGSAFE
pcre2getstartchar: gtm_long_t mpcre2_get_startchar(I:gtm_char_t*): SIGSAFE
pcre2matchdatacreate: gtm_char_t* mpcre2_match_data_create(I:gtm_long_t, I:gtm_char_t*): SIGSAFE
pcre2getovectorcount: gtm_long_t mpcre2_get_ovector_count(I:gtm_char_t*) : SIGSAFE
pcre2getovectorpointer: gtm_char_t *mpcre2_get_ovector_pointer(I:gtm_char_t *) : SIGSAFE
pcre2getovpair: void mpcre2_get_ov_pair(I:gtm_char_t*, I:gtm_long_t, O:gtm_long_t*, O:gtm_long_t*): SIGSAFE 
pcre2generalcontextcreate: gtm_char_t *mpcre2_general_context_create(I:gtm_char_t*, I:gtm_char_t*, I:gtm_char_t*): SIGSAFE
pcre2generalcontextcopy: gtm_char_t *mpcre2_general_context_copy(I:gtm_char_t *): SIGSAFE
pcre2generalcontextfree: void mpcre2_general_context_free(I:gtm_char_t *): SIGSAFE
pcre2compilecontextcreate: gtm_char_t *mpcre2_compile_context_create(I:gtm_char_t *): SIGSAFE
pcre2compilecontextcopy: gtm_char_t *mpcre2_compile_context_copy(I:gtm_char_t *): SIGSAFE
pcre2compilecontextfree: void mpcre2_compile_context_free(I:gtm_char_t *): SIGSAFE
pcre2setbsr: gtm_long_t mpcre2_set_bsr(I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2setcharactertables: gtm_long_t mpcre2_set_character_tables(I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2setcompileextraoptions: gtm_long_t mpcre2_set_compile_extra_options(I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2setmaxpatternlength: gtm_long_t mpcre2_set_max_pattern_length(I:gtm_char_t *, I:gtm_long_t): SIGSAFE
pcre2setnewline: gtm_long_t mpcre2_set_newline(I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2setparensnestlimit: gtm_long_t mpcre2_set_parens_nest_limit(I:gtm_char_t *, I:gtm_long_t): SIGSAFE
pcre2setcompilerecursionguard: gtm_long_t mpcre2_set_compile_recursion_guard(I:gtm_char_t *, I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2matchcontextcreate: gtm_char_t *mpcre2_match_context_create(I:gtm_char_t *): SIGSAFE
pcre2matchcontextcopy: gtm_char_t *mpcre2_match_context_copy(I:gtm_char_t *): SIGSAFE
pcre2matchcontextfree: void mpcre2_match_context_free(I:gtm_char_t*): SIGSAFE
pcre2setcallout: gtm_long_t mpcre2_set_callout(I:gtm_char_t *, I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2setoffsetlimit: gtm_long_t mpcre2_set_offset_limit(I:gtm_char_t*, I:gtm_long_t): SIGSAFE
pcre2setheaplimit: gtm_long_t mpcre2_set_heap_limit(I:gtm_char_t*, I:gtm_long_t): SIGSAFE
pcre2setmatchlimit: gtm_long_t mpcre2_set_match_limit(I:gtm_char_t*, I:gtm_long_t): SIGSAFE
pcre2setdepthlimit: gtm_long_t mpcre2_set_depth_limit(I:gtm_char_t*, I:gtm_long_t): SIGSAFE
pcre2substringcopybyname: gtm_long_t mpcre2_substring_copy_byname(I:gtm_char_t *, I:gtm_char_t *, O:gtm_string_t * [1048576])
pcre2substringcopybynumber: gtm_long_t mpcre2_substring_copy_bynumber(I:gtm_char_t *, I:gtm_long_t, O:gtm_string_t * [1048576])
pcre2substringfree: void mpcre2_substring_free(I:gtm_char_t *): SIGSAFE
pcre2getmstringfrombuf: gtm_string_t* mpcre2_get_mstring_from_buf(I:gtm_char_t *, I:gtm_long_t): SIGSAFE
pcre2substringgetbyname: gtm_long_t mpcre2_substring_get_byname(I:gtm_char_t *, I:gtm_char_t *, O:gtm_string_t * [80], O:gtm_long_t *): SIGSAFE
pcre2substringgetbynumber: gtm_long_t mpcre2_substring_get_bynumber(I:gtm_char_t *, I:gtm_long_t, O:gtm_string_t * [80], O:gtm_long_t *): SIGSAFE
pcre2substringlengthbyname: gtm_long_t mpcre2_substring_length_byname(I:gtm_char_t *, I:gtm_char_t *, O:gtm_long_t *): SIGSAFE
pcre2substringlengthbynumber: gtm_long_t mpcre2_substring_length_bynumber(I:gtm_char_t *, I:gtm_long_t, O:gtm_long_t *): SIGSAFE
pcre2substringnumberfromname: gtm_long_t mpcre2_substring_number_from_name(I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2substringlistget: gtm_long_t mpcre2_substring_list_get(I:gtm_char_t *, O:gtm_string_t * [1024], O:gtm_string_t * [1024]): SIGSAFE
pcre2getsubstringlistcount: gtm_long_t mpcre2_get_substring_list_count(I:gtm_char_t *): SIGSAFE
pcre2getmstringfromsubstringlist: gtm_string_t *mpcre2_get_mstring_from_substring_list(I:gtm_char_t *, I:gtm_char_t *, I:gtm_long_t): SIGSAFE
pcre2substringlistfree: void mpcre2_substring_list_free(I:gtm_char_t *): SIGSAFE
pcre2jitmatch: gtm_long_t mpcre2_jit_match(I:gtm_char_t*, I:gtm_string_t*, I:gtm_long_t, I:gtm_char_t*, I:gtm_char_t*, I:gtm_char_t*): SIGSAFE
pcre2jitfreeunusedmemory: void mpcre2_jit_free_unused_memory(I:gtm_char_t *): SIGSAFE
pcre2jitstackcreate: gtm_char_t *mpcre2_jit_stack_create(I:gtm_long_t , I:gtm_long_t , I:gtm_char_t *): SIGSAFE
pcre2jitstackassign: void mpcre2_jit_stack_assign(I:gtm_char_t *, I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2jitstackfree: void mpcre2_jit_stack_free(I:gtm_char_t *): SIGSAFE
pcre2serializedecode: gtm_long_t mpcre2_serialize_decode(O:gtm_string_t * [80], I:gtm_long_t, I:gtm_char_t *, I:gtm_char_t *): SIGSAFE
pcre2serializeencode: gtm_long_t mpcre2_serialize_encode(I:gtm_char_t *, I:gtm_long_t , O:gtm_string_t * [1024], O:gtm_long_t *, I:gtm_char_t *): SIGSAFE 
pcre2serializefree: void mpcre2_serialize_free(I:gtm_char_t *): SIGSAFE
pcre2serializegetnumberofcodes: gtm_long_t mpcre2_serialize_get_number_of_codes(I:gtm_char_t *): SIGSAFE
pcre2codecopy: gtm_char_t *mpcre2_code_copy(I:gtm_char_t *): SIGSAFE
pcre2codecopywithtables: gtm_char_t *mpcre2_code_copy_with_tables(I:gtm_char_t *): SIGSAFE
pcre2maketables: gtm_char_t *mpcre2_maketables(I:gtm_char_t *): SIGSAFE
pcre2patterninfo: gtm_long_t mpcre2_pattern_info(I:gtm_char_t *, I:gtm_char_t *, O:gtm_string_t * [80])
mpcre2calloutenumerate: gtm_long_t mpcre2_callout_enumerate(I:gtm_char_t *, I:gtm_char_t *, I:gtm_char_t *): SIGSAFE 
