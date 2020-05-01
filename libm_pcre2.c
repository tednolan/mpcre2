#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include "gtmxc_types.h"

static void* (*malloc_fn)(int); 
static void (*free_fn)(void*); 
static void *m_pcre2_malloc(PCRE2_SIZE, void *);
static void m_pcre2_free(void *, void *);

static pcre2_general_context *global_gcontext;
static pcre2_match_context *global_mcontext;

typedef struct opt_tab {
	const char *name;
	uint32_t val;
} opt_tab_t;

static char *null_string = "";

struct opt_tab compile_opts [] = {
	{ "PCRE2_ANCHORED", PCRE2_ANCHORED },
	{ "PCRE2_ALLOW_EMPTY_CLASS", PCRE2_ALLOW_EMPTY_CLASS },
	{ "PCRE2_ALT_BSUX", PCRE2_ALT_BSUX },
	{ "PCRE2_ALT_CIRCUMFLEX", PCRE2_ALT_CIRCUMFLEX },
	{ "PCRE2_ALT_VERBNAMES", PCRE2_ALT_VERBNAMES },
	{ "PCRE2_AUTO_CALLOUT", PCRE2_AUTO_CALLOUT },
	{ "PCRE2_CASELESS", PCRE2_CASELESS },
	{ "PCRE2_DOLLAR_ENDONLY", PCRE2_DOLLAR_ENDONLY },
	{ "PCRE2_DOTALL", PCRE2_DOTALL },
	{ "PCRE2_DUPNAMES", PCRE2_DUPNAMES },
	{ "PCRE2_ENDANCHORED", PCRE2_ENDANCHORED },
	{ "PCRE2_EXTENDED", PCRE2_EXTENDED },
	{ "PCRE2_FIRSTLINE", PCRE2_FIRSTLINE },
	{ "PCRE2_LITERAL", PCRE2_LITERAL },
	{ "PCRE2_MATCH_UNSET_BACKREF", PCRE2_MATCH_UNSET_BACKREF },
	{ "PCRE2_MULTILINE", PCRE2_MULTILINE },
	{ "PCRE2_NEVER_BACKSLASH_C", PCRE2_NEVER_BACKSLASH_C },
	{ "PCRE2_NEVER_UCP", PCRE2_NEVER_UCP },
	{ "PCRE2_NEVER_UTF", PCRE2_NEVER_UTF },
	{ "PCRE2_NO_AUTO_CAPTURE", PCRE2_NO_AUTO_CAPTURE },
	{ "PCRE2_NO_AUTO_POSSESS", PCRE2_NO_AUTO_POSSESS },
	{ "PCRE2_NO_DOTSTAR_ANCHOR", PCRE2_NO_DOTSTAR_ANCHOR },
	{ "PCRE2_NO_START_OPTIMIZE", PCRE2_NO_START_OPTIMIZE },
	{ "PCRE2_NO_UTF_CHECK", PCRE2_NO_UTF_CHECK },
	{ "PCRE2_UCP", PCRE2_UCP },
	{ "PCRE2_UNGREEDY", PCRE2_UNGREEDY },
	{ "PCRE2_USE_OFFSET_LIMIT", PCRE2_USE_OFFSET_LIMIT },
	{ "PCRE2_UTF", PCRE2_UTF },
};
int n_compile_opts = sizeof(compile_opts) / sizeof(struct opt_tab);

struct opt_tab match_opts [] = {
	{ "PCRE2_ANCHORED", PCRE2_ANCHORED },
	{ "PCRE2_ENDANCHORED", PCRE2_ENDANCHORED },
	{ "PCRE2_NOTBOL", PCRE2_NOTBOL },
	{ "PCRE2_NOTEOL", PCRE2_NOTEOL },
	{ "PCRE2_NOTEMPTY", PCRE2_NOTEMPTY },
	{ "PCRE2_NOTEMPTY_ATSTART", PCRE2_NOTEMPTY_ATSTART },
	{ "PCRE2_NO_JIT", PCRE2_NO_JIT },
	{ "PCRE2_NO_UTF_CHECK", PCRE2_NO_UTF_CHECK },
	{ "PCRE2_PARTIAL_HARD", PCRE2_PARTIAL_HARD },
	{ "PCRE2_PARTIAL_SOFT", PCRE2_PARTIAL_SOFT },
};
int n_match_opts = sizeof(match_opts) / sizeof(struct opt_tab);

/**
 * @param Create a pointer from a base 16 encoded string
 *
 * We pass pointers out to M as strings, and deode strings from M into pointers.
 * This function does the decode.  We use strtoull to make sure to have enough bits.
 *
 * @param pstr A pointer value encoded as a base 16 string
 *
 * @return A void pointer which must be cast or NULL on failure
 */
static void *pointer_decode(const char *pstr) {

	char *endstr;		/* pointer to where strtoull left off */
	void *outptr;

	outptr = (void *) strtoull(pstr, &endstr, 16);
	if (! (*pstr != '\0') && (*endstr == '\0') ) {
		fprintf(stderr, "Cannot convert %s to a pointer\n", pstr);
			return NULL;
	}

	return outptr;
}

/**
 * @param Create a base 16 encoded string from a pointer
 *
 * We pass pointers out to M as strings, and deode strings from M into pointers.
 * This function does the encode.
 *
 * @param ptr The pointer value to encode
 * @param buf String storage
 * @param size Size of storage
 *
 * @return A void pointer which must be cast or NULL on failure
 */
static void pointer_encode(void *ptr, char *buf, int size) {

	snprintf(buf, size, "%p", ptr);
}
	
/**
 * @brief Pcre2 compatibile wrapper for M malloc()
 *
 * @param size Number of bytes to allocate
 * @param memory_data Unused
 * 
 * @return Pointer to allocated memory or NULL on error
 */
static void *m_pcre2_malloc(PCRE2_SIZE size, void *memory_data) {
	return malloc_fn(size);
}

/**
 * @brief Pcre2 compatible wrapper for M free()
 *
 * @param ptr Pointer to memory to free
 * @param Unused
 *
 * @return None
 */
static void m_pcre2_free(void *ptr, void *memory_data) {

	free_fn(ptr);
}
 
/**
 * @brief Get pointers to the M malloc & free functions
 *
 * This function based on an example in the GT.M Programmers Guide, sets us up
 * to use the M malloc() & free() functions, which is recommended for C code
 * called from M.  We could use names if we linked to the callin shared library,
 * but that would be * an additional dependency.
 *
 * It should be harmless to call this function more than once, but don't do that.
 *
 * @return 0 for success, -1 for an initialization error
 */
static int initialize () {

	void **functable; 
	char *start_address;
	char *endstring;
	static int initialize_done = 0;

	if (initialize_done)
		return 0;
 
	start_address = getenv("GTM_CALLIN_START"); /* set by the M runtime */
 
	if (start_address == (char *)0) {
		fprintf(stderr,"GTM_CALLIN_START is not set\n"); 
		return -1;
	} 

	/*
	 * It seems a safe bet that an unsigned long long is enough bits
	 * for a pointer type.  We don't use functable unless the strtoull
	 * "OK" condition is met. (per the man page).
	 */
	functable = (void **) strtoull(start_address, &endstring, 10);
	if (! (*start_address != '\0') && (*endstring == '\0') ) {
		fprintf(stderr, "GTM_CALLIN_START is not parsable\n");
		return -1;
	}

	malloc_fn = (void* (*)(int)) functable[4]; 
	free_fn = (void (*)(void*)) functable[5]; 

	/*
	 * Now that we have our malloc & free functions located,
	 * we can create our general context
	 */

	global_gcontext = pcre2_general_context_create(m_pcre2_malloc, m_pcre2_free, NULL);

	if (!global_gcontext) {
		fprintf(stderr, "Unable to allocate global general context\n");
		return -1;
	}

	global_mcontext = pcre2_match_context_create(global_gcontext);
	if (!global_mcontext) {
		fprintf(stderr, "Unable to allocate global match context\n");
		return -1;
	}

	initialize_done = 1;
 
	return 0;
}

/**
 * @brief Turn pcre2_compile options presented as a C string into an integer
 *
 * Normally, when using pcre2_compile() in C, the options are presented as
 * a bitmap constructed by logically ORing macros.  When the options come in
 * from M, they are a string that looks the same as the C interface, but which
 * must be parsed out since this is not easily done on the M side.
 *
 * This means we are called with strings like:
 * 
 *	"PCRE2_ANCHORED|PCRE2_ALLOW_EMPTY_CLASS|PCRE2_ALT_BSUX"
 *
 * If an error is encountered, -1 will be returned, otherwise the indicated
 * logical OR will be.
 *
 * @param options A null terminated C string indicating flags.
 * @param result  Pointer to the storage for the ORed flags result
 *
 * @return The indicated logical OR value or -1 on error
 * 
 */
static int parse_compile_options(char *options, uint32_t *result) {

	size_t len;
	char *saveptr;
	char *token;
	uint32_t res = 0;
	int i;
	char *cpt;
	int found;

	/*
	 * It is possible that options are 0, in which case we don't have to do
	 * much.
	 */
	if((sscanf(options, "%d", &i) == 1) && (i == 0)) {
		fprintf(stderr, "O (%s) OPT\n", options);
		*result = 0;
		return 0;
	}

	/*
	 * Otherwise, we need to copy options so we can process with strtok_r()
	 */
	len = strlen(options);
	cpt = malloc_fn(len + 1);
	if (!cpt) {
		return -1;
	}
	strncpy(cpt, options, len + 1);

	while ( (token = strtok_r(cpt, "|", &saveptr)) ) {

		found = 0;
		for(i = 0; i < n_compile_opts; i++) {
			if (strcmp(token, compile_opts[i].name) == 0) {
				res |= compile_opts[i].val;
				found = 1;
				break;
			}
		}

		if (!found) {
			fprintf(stderr, "Unknown compile option %s\n", token);
			return -1;
		}

		cpt = NULL;	/* signal strtok_r to continue last parse */
	}

	*result = res;
	return 0;
}


/**
 * @brief Turn pcre2_match options presented as a C string into an integer
 *
 * Normally, when using pcre2_match() in C, the options are presented as
 * a bitmap constructed by logically ORing macros.  When the options come in
 * from M, they are a string that looks the same as the C interface, but which
 * must be parsed out since this is not easily done on the M side.
 *
 * This means we are called with strings like:
 * 
 *	"PCRE2_ANCHORED|PCRE2_ENDANCHORED|PCRE2_NOTBOL"
 *
 * If an error is encountered, -1 will be returned, otherwise the indicated
 * logical OR will be.
 *
 * Really this function is identical to parse_compile_options() and the two should
 * be unified.
 *
 * @param options A null terminated C string indicating flags.
 * @param result  Pointer to the storage for the ORed flags result
 *
 * @return The indicated logical OR value or -1 on error
 * 
 */
static int parse_match_options(char *options, uint32_t *result) {

	size_t len;
	char *saveptr;
	char *token;
	uint32_t res = 0;
	int i;
	char *cpt;
	int found;

	/*
	 * It is possible that options are 0, in which case we don't have to do
	 * much.
	 */
	if((sscanf(options, "%d", &i) == 1) && (i == 0)) {
		fprintf(stderr, "O (%s) OPT\n", options);
		*result = 0;
		return 0;
	}

	/*
	 * Otherwise, we need to copy options so we can process with strtok_r()
	 */
	len = strlen(options);
	cpt = malloc_fn(len + 1);
	if (!cpt) {
		return -1;
	}
	strncpy(cpt, options, len + 1);

	while ( (token = strtok_r(cpt, "|", &saveptr)) ) {

		found = 0;
		for(i = 0; i < n_match_opts; i++) {
			if (strcmp(token, match_opts[i].name) == 0) {
				res |= match_opts[i].val;
				found = 1;
				break;
			}
		}

		if (!found) {
			fprintf(stderr, "Unknown match option %s\n", token);
			return -1;
		}

		cpt = NULL;	/* signal strtok_r to continue last parse */
	}

	*result = res;
	return 0;
}

gtm_long_t regexp(int argc, gtm_string_t *regexp, gtm_string_t *subject, gtm_string_t *out) {

	int pcre2_err;
	PCRE2_SIZE erroroffset;
	pcre2_code *mypat;
	pcre2_match_data *match_data;
	int res;
	PCRE2_SIZE ovcount;
	PCRE2_SIZE *ovp;

	fprintf(stderr, "Called argc = %d\n", argc);

	mypat = pcre2_compile((PCRE2_SPTR) (regexp->address),
		(PCRE2_SIZE) (regexp->length), 0,
		&pcre2_err, &erroroffset, NULL);

	if (!mypat) {
		fprintf(stderr, "Compile fails!\n");
		return(-1);
	}

	match_data = pcre2_match_data_create_from_pattern(mypat, NULL);

	res = pcre2_match(mypat, (PCRE2_SPTR) (subject->address), (PCRE2_SIZE) (subject->length),
		0, 0, match_data, NULL);
	fprintf(stderr, "%d\n", res);

	ovcount = pcre2_get_ovector_count(match_data);
	fprintf(stderr, "ovcount %lu\n", ovcount);

	ovp = pcre2_get_ovector_pointer(match_data); 
	void *memcpy(void *dest, const void *src, size_t n);
	memcpy(out->address,subject->address + ovp[0], ovp[1] - ovp[0]);
	out->length = ovp[1] - ovp[0];

	pcre2_match_data_free(match_data); 
	pcre2_code_free(mypat);

	return res;

}

/**
 * @brief Wrapper for M calls to pcre2_compile
 *
 * This function serves as a wrapper so M can call pcre2_compile().
 * The parameters are similar to the C call, but since we use an M string,
 * for the pattern, we know the length, and don't have to pass that as a
 * separate parameter. 
 * 
 * In general, refer to the PCRE2 documentation for a fuller description
 * of these parameters.  The handle returned on success will be a base64
 * encoded pointer.
 *
 * @param count M API supplied count of arguments to this function
 * @param pattern The regular expression we are compiling
 * @param options Compile options
 * @param errorcode Output parameter to return an error if compile fails
 * @param erroroffset Output parameter indicating the byte offset of a compile failure
 * @param ccontext_str Compile context for compile, "0" for defaults
 *
 * @return A handle for this compiled pattern or a null string on failure
 */
gtm_char_t *mpcre2_compile(int count, gtm_string_t *pattern, gtm_char_t *options,
	gtm_long_t *errorcode, gtm_ulong_t *erroroffset, gtm_char_t *ccontext_str) {

	int ecode;
	PCRE2_SIZE eoffset;
	uint32_t compile_options;
	pcre2_compile_context *ccontext;
	pcre2_code *code;		/* our compiled pattern */
	static char result[32];		/* must be able to hold a hex encoded 64-bit pointer */
	int must_free_ccontext;

	if(initialize() < 0) {
		return null_string;
	}

	/*
	 * If custom compile option strings are passed in, we must
	 * map them from string format to internal format
	 */
	if ( parse_compile_options(options, &compile_options) < 0) {
		return null_string;
	}


	/*
	 * If we are not supplied a compile context, we must create one.  This
	 * solely due to the fact that we want to use the M malloc() & free() functions
	 * to play more nicely with the M environment.
	 * 
	 * If we *are* supplied a compile context, we use that instead, and do not
	 * gratuitously free it.  We test this by seeing if we were passed something
	 * other than a "0".  (We do not attempt to further validate a non "0")
	 */
	if (strcmp(ccontext_str, "0") == 0) {
		ccontext = pcre2_compile_context_create(global_gcontext);
		must_free_ccontext = 1;
	} else {
		ccontext = (pcre2_compile_context *) pointer_decode(ccontext_str);
	}

	/*
	 * Now actually compile the pattern
	 */
	code = pcre2_compile( (PCRE2_SPTR) (pattern->address), (PCRE2_SIZE) (pattern->length),
		compile_options, &ecode, &eoffset, ccontext);

	if (must_free_ccontext) {
		pcre2_compile_context_free(ccontext);
	}

	*erroroffset = eoffset;
	*errorcode = ecode;
	fprintf(stderr, "eoff = %ld, ecode = %d, ptr %p\n", eoffset, ecode,code);

	if (!code) {
		return null_string;
	}

	pointer_encode(code, result, sizeof(result));
	return result;

}

/**
 * @brief Translate a pcre2_compile() error code into a text message
 *
 * This function provides an M interface to translate a pcre2_compile() error code to a
 * string.  The interface differs a bit from the C function wrapped as the M string
 * supplies its own (max) length.
 *
 * @param count M API supplied argument count
 * @param errorcode Code to be mapped to a string
 *
 */
gtm_long_t mpcre2_get_error_message(int count, gtm_long_t errorcode, gtm_string_t *buffer) {

	gtm_long_t res;

	res = pcre2_get_error_message( (int) errorcode, (PCRE2_UCHAR *) buffer->address,
		buffer->length);

	if (res < 0) {
		buffer->length = 0;
	} else {
		buffer->length = res;
	}

	return res;
}

/**
 * @brief Wrap pcre2_code_free()
 *
 * We pass pcre2 code pointers back and forth to M as text.
 * We need to re-create the code pointer and free it.
 *
 * @param count Count of parameters from M API
 * @param code String value of a code pointer from M
 *
 * @return None
 */
void mpcre2_code_free(int count, gtm_char_t *code) {

	pcre2_code *ptr;

	ptr = (pcre2_code *) pointer_decode(code);

	pcre2_code_free(ptr);
}

/**
 * @brief Wrap pcre2_match_data_create_from_pattern()
 *
 * If we are called with a non "0" general context, use it.
 * Otherwise we use our internal general context.
 *
 * @param count Count of parameters from the M API
 * @param code A compiled pcre2 regular expression pointer in string format
 * @param code A custom general context or "0" in string format
 *
 * @return A stringified match data pointer
 */
gtm_char_t *mpcre2_match_data_create_from_pattern(int count, gtm_char_t *code_str,
	gtm_char_t *gcontext_str) {

	pcre2_code *code;
	pcre2_general_context *gc;
	pcre2_match_data *md;
	static char buf[32];

	if(initialize() < 0) {
		return null_string;
	}

	code = (pcre2_code *) pointer_decode(code_str);

	if (strcmp(gcontext_str, "0") == 0) {
		gc = global_gcontext;
	} else {
		gc = (pcre2_general_context *) pointer_decode(gcontext_str);
	}

	md = pcre2_match_data_create_from_pattern(code, gc);

	pointer_encode((void *) md, buf, sizeof(buf));

	return buf;
}

/**
 * @brief wrap pcre2_match() for M
 *
 * We bring in "subject" as an M string, so we can have embedded zero bytes.  This gives
 * us a length, so we don't have a separate parameter for that.
 *
 * @param count Count of parameters from the M API
 * @param code_str A pcre2 compiled regular expression pointer in string format
 * @param subject The string to search for matches
 * @param startoffset The byte offset at which to start the match search
 * @param options Pcre2 match options in string form
 * @param match_data_str A Pcre2 match data pointer in string format
 * @param mcontext_str A Pcre2 match context pointer in string format, or "0"
 *
 * @return < 0 on error or no match, 0 vector offests too small, else one more than the highest numbered capturing pair that has been set
 */
gtm_long_t mpcre2_match(int count, gtm_char_t *code_str, gtm_string_t *subject,
	gtm_long_t startoffset, gtm_char_t *options_str, gtm_char_t *match_data_str,
	gtm_char_t *mcontext_str) {

	pcre2_code *code;
	pcre2_match_data *match_data;
	pcre2_match_context *mc;
	uint32_t options;

	if (initialize() < 0) {
		return -1;
	}

	code = (pcre2_code *) pointer_decode(code_str);

	if (parse_match_options(options_str, &options) < 0) {
		return -1;
	}

	match_data = (pcre2_match_data *) pointer_decode(match_data_str);

	if (strcmp(mcontext_str, "0") == 0) {
		mc = global_mcontext;
	} else {
		mc = (pcre2_match_context *) pointer_decode(mcontext_str);
	}

	return(pcre2_match(code, (PCRE2_SPTR) subject->address, (PCRE2_SIZE) subject->length,
		(PCRE2_SIZE) startoffset, options, match_data, mc));

}
