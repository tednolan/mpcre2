#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include "gtmxc_types.h"

static void* (*malloc_fn)(int);
static void (*free_fn)(void*);

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

struct opt_tab extra_compile_opts [] = {
	{ "PCRE2_EXTRA_ALLOW_SURROGATE_ESCAPES", PCRE2_EXTRA_ALLOW_SURROGATE_ESCAPES },
	{ "PCRE2_EXTRA_BAD_ESCAPE_IS_LITERAL", PCRE2_EXTRA_BAD_ESCAPE_IS_LITERAL },
	{ "PCRE2_EXTRA_MATCH_LINE", PCRE2_EXTRA_MATCH_LINE },
	{ "PCRE2_EXTRA_MATCH_WORD", PCRE2_EXTRA_MATCH_WORD },
};
int n_extra_compile_opts = sizeof(extra_compile_opts) / sizeof(struct opt_tab);

/*
 * This table has both match and substitute options, because pcre2_substitute()
 * takes many of the match options
 */
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
	{ "PCRE2_SUBSTITUTE_EXTENDED", PCRE2_SUBSTITUTE_EXTENDED },
        { "PCRE2_SUBSTITUTE_GLOBAL", PCRE2_SUBSTITUTE_GLOBAL },
	{ "PCRE2_SUBSTITUTE_OVERFLOW_LENGTH", PCRE2_SUBSTITUTE_OVERFLOW_LENGTH },
	{ "PCRE2_SUBSTITUTE_UNKNOWN_UNSET", PCRE2_SUBSTITUTE_UNKNOWN_UNSET },
	{ "PCRE2_SUBSTITUTE_UNSET_EMPTY", PCRE2_SUBSTITUTE_UNSET_EMPTY },
};
int n_match_opts = sizeof(match_opts) / sizeof(struct opt_tab);

struct opt_tab jit_opts [] = {
	{ "PCRE2_JIT_COMPLETE", PCRE2_JIT_COMPLETE },
	{ "PCRE2_JIT_PARTIAL_SOFT", PCRE2_JIT_PARTIAL_SOFT },
	{ "PCRE2_JIT_PARTIAL_HARD", PCRE2_JIT_PARTIAL_HARD },
};
int n_jit_opts = sizeof(jit_opts) / sizeof(struct opt_tab);

struct opt_tab bsr_opts [] = {
	{ "PCRE2_BSR_ANYCRLF", PCRE2_BSR_ANYCRLF },
	{ "PCRE2_BSR_UNICODE", PCRE2_BSR_UNICODE },
};
int n_bsr_opts = sizeof(bsr_opts) / sizeof(struct opt_tab);

struct opt_tab newline_opts [] = {
	{ "PCRE2_NEWLINE_CR", PCRE2_NEWLINE_CR },
	{ "PCRE2_NEWLINE_LF", PCRE2_NEWLINE_LF },
	{ "PCRE2_NEWLINE_CRLF", PCRE2_NEWLINE_CRLF },
	{ "PCRE2_NEWLINE_ANYCRLF", PCRE2_NEWLINE_ANYCRLF },
	{ "PCRE2_NEWLINE_ANY", PCRE2_NEWLINE_ANY },
	{ "PCRE2_NEWLINE_NUL", PCRE2_NEWLINE_NUL },
};
int n_newline_opts = sizeof(newline_opts) / sizeof(struct opt_tab);

/*
 * First we have a number of general utility functions that are not exported
 */


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
 * @param Decode a string handle into a pointer
 *
 * We pass pointers out to M as strings, and deode strings from M into pointers.
 * This function does the decode.
 *
 * @param pstr A pointer value encoded as a string handle
 *
 * @return A void pointer which must be cast before use
 */
static void *pointer_decode(char *pstr) {

	void *outptr;
	unsigned long long p;	/* should work on 32 or 64 bit system */

	if (sscanf(pstr, "%llu", &p) != 1) {
		fprintf(stderr, "Cannot convert %s to a pointer\n", pstr);
		return NULL;
	}

	outptr = (void *) p;

	return outptr;
}

/**
 * @param Encode a pointer as a string handle
 *
 * Encode a pointer as a string handle to pass out to M
 * 
 * @param ptr The pointer value to encode
 * @param buf String storage
 * @param size Size of storage
 *
 * @return None
 */
static void pointer_encode(void *ptr, char *buf, int size) {

	/*
	 * Unsigned long long should work on both 32 & 64 bit systems
	 */
	snprintf(buf, size, "%llu", (unsigned long long) ptr);
}

/*
 * Then we have several functions which do use the PCRE2 API but are not
 * wrappers for PCRE2 functions and are not exported.
 */
	
/**
 * @brief Get or create a general context
 *
 * PCRE2 uses the pcre2_general_context * type as the hook for replacing the
 * system malloc()/free() functions with custom versions.  Since we want to play
 * nice with M, we use this hook to install the M supplied versions.  The practical
 * effect of this is that the first time a pcre2 function that needs to allocate memory
 * is called, we set up a static pcre2_general context with the custom memory functions.
 *
 * Future requests for a general context return this one.  The exception is if we are
 * passed an encoded handle from M, in which case we decode it as a pointer.  We don't
 * actually expect this to happen.  Currently the general context created here is not
 * freed, so that's a few bytes the M process will not get back.
 *
 * This function is based on an example in the GT.M Programmers Guide, sets us up
 * to use the M malloc() & free() functions, which is recommended for C code
 * called from M.  We could use names if we linked to the callin shared library,
 * but that would be * an additional dependency.
 *
 * @param general_context_str For "0" we create & return (or just return) the static GC.  Otherwise decode as a pointer
 *
 * @return A pcre2_general_context pointer, or NULL on failure
 */
static pcre2_general_context *get_general_context (char *general_context_str) {

	void **functable; 
	char *start_address_str;
	unsigned long long start_address;
	static pcre2_general_context *default_gc = NULL;

	start_address_str = getenv("GTM_CALLIN_START"); /* set by the M runtime */

	/*
	 * If they supply "0" or "NULL" use or create the default_gc * else decode it.
	 */
	if ( (strcmp(general_context_str, "0") != 0) && (strcmp(general_context_str, "NULL") != 0) ) {
		return (pcre2_general_context *) pointer_decode(general_context_str);
	} else if (default_gc) {
		return default_gc;
	}

	/*
	 * We have not been initialized, and have to do some setup
	 */
 
	if (start_address_str == (char *)0) {
		fprintf(stderr,"GTM_CALLIN_START is not set\n"); 
		return NULL;
	} 

	/*
	 * It seems a safe bet that an unsigned long long is enough bits
	 * for a pointer type.  We don't use functable unless the strtoull
	 * "OK" condition is met. (per the man page).
	 */
	if (sscanf(start_address_str, "%llu", &start_address) != 1) {
		fprintf(stderr, "GTM_CALLIN_START is not parsable\n");
		return NULL;
	}
	functable = (void **) start_address;

	malloc_fn = (void* (*)(int)) functable[4]; 
	free_fn = (void (*)(void*)) functable[5]; 

	/*
	 * Now that we have our malloc & free functions located,
	 * we can create our general context
	 */

	default_gc = pcre2_general_context_create(m_pcre2_malloc, m_pcre2_free, NULL);

	if (!default_gc) {
		fprintf(stderr, "Unable to allocate global general context\n");
		return NULL;
	}

	return default_gc;
}

/**
 * @brief Get or create a compile context
 *
 * If we are supplied "0" or "NULL", we create a generic compile context.
 * Otherwise we decode the given context.
 *
 * If we created the context, we set the free flag, otherwise not
 *
 * @param context_str The incoming compile context handle
 * @param must_free Pointer to our free flag
 *
 * @return a pcre2_compile_context pointer or NULL
 * 
 */
pcre2_compile_context *get_compile_context(char *context_str, int *must_free) {
	
	pcre2_general_context *gc;
	pcre2_compile_context *cc;

	*must_free = 0;

	if ( (strcmp(context_str, "0") != 0) && (strcmp(context_str, "NULL") != 0) ) {
		*must_free = 0;
		return (pcre2_compile_context *) pointer_decode(context_str);
	}

	gc = get_general_context("0");
	cc = pcre2_compile_context_create(gc);

	if (!cc) {
		fprintf(stderr, "Can't allocate compile context!\n");
		return NULL;
	}

	*must_free = 1;

	return cc;
}


/**
 * @brief Get or create a match context
 *
 * If we are supplied "0" or "NULL", we create a generic match context.
 * Otherwise we decode the given context.
 *
 * If we created the context, we set the free flag, otherwise not
 *
 * @param context_str The incoming match context handle
 * @param must_free Pointer to our free flag
 *
 * @return a pcre2_match_context pointer or NULL
 * 
 */
pcre2_match_context *get_match_context(char *context_str, int *must_free) {
	
	pcre2_general_context *gc;
	pcre2_match_context *mc;

	*must_free = 0;

	if ( (strcmp(context_str, "0") != 0) && (strcmp(context_str, "NULL") != 0) ) {
		*must_free = 0;
		return (pcre2_match_context *) pointer_decode(context_str);
	}

	gc = get_general_context("0");
	mc = pcre2_match_context_create(gc);

	if (!mc) {
		fprintf(stderr, "Can't allocate match context!\n");
		return NULL;
	}

	*must_free = 1;

	return mc;
}


/**
 * @brief Turn various pcre2 options presented as C strings into integers
 *
 * Normally, when using options in certain pcre2 functions, the options are presented as
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
 * This parse function is used with several different option tables.
 *
 * @param option_table table mapping strings to bitmasks
 * @param option_count number of entries in the option table
 * @param opt_tag String to use as a label for this set of options in error messages
 * @param options A null terminated C string indicating flags.
 * @param result  Pointer to the storage for the ORed flags result
 *
 * @return 0 on success or -1 on error
 * 
 */
static int parse_pcre2_options(struct opt_tab *option_table, int option_count, char *opt_tag,
	char *options, uint32_t *result) {

	size_t len;
	char *saveptr;
	char *token;
	uint32_t res = 0;
	char *cpt;
	char *save_cpt;
	int found;
	int i;

	/*
	 * It is possible that options are 0, in which case we don't have to do much.
	 */
	if(strcmp(options, "0") == 0) {
		*result = 0;
		return 0;
	}

	/*
	 * Otherwise, we need to copy options so we can process with strtok_r()
	 * Note that we will set cpt = NULL in some cases, so we need to keep a copy to free
	 */
	len = strlen(options);
	cpt = malloc_fn(len + 1);
	if (!cpt) {
		return -1;
	}
	save_cpt = cpt;
	strncpy(cpt, options, len + 1);

	while ( (token = strtok_r(cpt, "|", &saveptr)) ) {

		found = 0;
		for(i = 0; i < option_count; i++) {
			if (strcmp(token, option_table[i].name) == 0) {
				res |= option_table[i].val;
				found = 1;
				break;
			}
		}

		if (!found) {
			fprintf(stderr, "Unknown %s option %s\n", opt_tag, token);
			free_fn(save_cpt);
			return -1;
		}

		cpt = NULL;	/* signal strtok_r to continue last parse */
	}

	*result = res;

	free_fn(save_cpt);

	return 0;
}

/*
 * This section contains exported helper functions which do not directly map to
 * the PCRE2 API, but paper over the differences between M & C
 */

/*
 * @brief Return an output vector pair
 *
 * Since M cannot directly manipulate output vector pointers, this access function
 * is needed to take the handle and an index and return the queried ouput
 * vector pair
 *
 * @param count Parameter count provided by the M API
 * @param ovector_str A string handle for a pointer to the output vector pairs
 * @param index Which pair to return
 * @param p0 Pointer to the first element of the pair to return
 * @param p1 Pointer to the second element of the pair to return
 *
 * @return None
 */
void mpcre2_get_ov_pair(int count, gtm_char_t *ovector_str, gtm_long_t index, gtm_long_t *p0, gtm_long_t *p1) {

	PCRE2_SIZE *ov;

	ov = (PCRE2_SIZE *) pointer_decode(ovector_str);

	*p0 = ov[index*2];
	*p1 = ov[(index*2)+1];
}

/*
 * This section contains functions which are exported and are 1 for 1 wrappings
 * of PCRE2 functions.  They are listed in the order given in the PCRE2
 * https://www.pcre.org/current/doc/html/pcre2api.html document. (As of a
 * certain point in time..)
 */

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
	static char result[80];
	int must_free;

	/*
	 * If custom compile option strings are passed in, we must
	 * map them from string format to internal format
	 */
	if ( parse_pcre2_options(compile_opts, n_compile_opts, "compile",
		options, &compile_options) < 0) {
		return null_string;
	}


	ccontext = get_compile_context(ccontext_str, &must_free);

	/*
	 * Now actually compile the pattern
	 */
	code = pcre2_compile( (PCRE2_SPTR) (pattern->address), (PCRE2_SIZE) (pattern->length),
		compile_options, &ecode, &eoffset, ccontext);

	if (must_free) {
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
 * @brief Wrap the pcre2_match_data_create() function
 *
 * If we are called with a non "0" general context, use it.
 * Otherwise we use our internal general context.
 *
 * @param count Count of parameters from the M API
 * @param ovecsize The size of the output vector to create
 * @param gcontext_str A custom general context or "0" in string format
 *
 * @return A stringified match data handle
 */
gtm_char_t *mpcre2_match_data_create(int count, gtm_long_t ovecsize, gtm_char_t *gcontext_str) {

	pcre2_general_context *gc;
	pcre2_match_data *md;
	static char buf[80];

	gc = get_general_context(gcontext_str);

	md = pcre2_match_data_create(ovecsize, gc);

	pointer_encode(md, buf, sizeof(buf));

	return buf;
}

/**
 * @brief Wrap pcre2_match_data_create_from_pattern()
 *
 * If we are called with a non "0" general context, use it.
 * Otherwise we use our internal general context.
 *
 * @param count Count of parameters from the M API
 * @param code A compiled pcre2 regular expression pointer in string format
 * @param gcontext_str A custom general context or "0" in string format
 *
 * @return A stringified match data pointer
 */
gtm_char_t *mpcre2_match_data_create_from_pattern(int count, gtm_char_t *code_str,
	gtm_char_t *gcontext_str) {

	pcre2_code *code;
	pcre2_general_context *gc;
	pcre2_match_data *md;
	static char buf[80];

	code = (pcre2_code *) pointer_decode(code_str);

	gc = get_general_context(gcontext_str);

	md = pcre2_match_data_create_from_pattern(code, gc);

	pointer_encode(md, buf, sizeof(buf));

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
	int must_free;
	gtm_long_t res;

	code = (pcre2_code *) pointer_decode(code_str);

	if (parse_pcre2_options(match_opts, n_match_opts, "match", options_str, &options) < 0) {
		return -1;
	}

	match_data = (pcre2_match_data *) pointer_decode(match_data_str);

	mc = get_match_context(mcontext_str, &must_free);

	res = pcre2_match(code, (PCRE2_SPTR) subject->address, (PCRE2_SIZE) subject->length,
		(PCRE2_SIZE) startoffset, options, match_data, mc);

	if (must_free) {
		pcre2_match_context_free(mc);
	}

	return res;

}

/**
 * @brief wrap pcre2_dfa_match() for M
 *
 * We bring in "subject" as an M string, so we can have embedded zero bytes.  This gives
 * us a length, so we don't have a separate parameter for that.  Additionally, there is
 * no way, or reason to pass in a vector of ints for "workspace from M, so we simply accept
 * a count, and allocate that many on the stack.
 *
 * @param count Count of parameters from the M API
 * @param code_str A pcre2 compiled regular expression pointer in string format
 * @param subject The string to search for matches
 * @param startoffset The byte offset at which to start the match search
 * @param options Pcre2 match options in string form
 * @param match_data_str A Pcre2 match data pointer in string format
 * @param mcontext_str A Pcre2 match context pointer in string format, or "0"
 * @apram wscount Number of entries to create in the workspace vector
 *
 * @return < 0 on error or no match, 0 vector offests too small, else one more than the highest numbered capturing pair that has been set
 */
gtm_long_t mpcre2_dfa_match(int count, gtm_char_t *code_str, gtm_string_t *subject,
	gtm_long_t startoffset, gtm_char_t *options_str, gtm_char_t *match_data_str,
	gtm_char_t *mcontext_str, gtm_long_t wscount) {

	pcre2_code *code;
	pcre2_match_data *match_data;
	pcre2_match_context *mc;
	uint32_t options;
	int workspace[wscount];
	int must_free;
	gtm_long_t res;

	code = (pcre2_code *) pointer_decode(code_str);

	if (parse_pcre2_options(match_opts, n_match_opts, "match", options_str, &options) < 0) {
		return -1;
	}

	match_data = (pcre2_match_data *) pointer_decode(match_data_str);

	mc = get_match_context(mcontext_str, &must_free);

	res = pcre2_dfa_match(code, (PCRE2_SPTR) subject->address, (PCRE2_SIZE) subject->length,
		(PCRE2_SIZE) startoffset, options, match_data, mc, workspace, (PCRE2_SIZE) wscount);

	if (must_free) {
		pcre2_match_context_free(mc);
	}

	return res;

}

/**
 * @brief Wrap the pcre2_match_data_free() function
 *
 * @param count M API argument count
 * @param match_data_str Stringified match data pointer
 * 
 * @return None
 */
void mpcre2_match_data_free(int count, gtm_char_t *match_data_str) {

	pcre2_match_data *match_data;

	match_data = (pcre2_match_data *) pointer_decode(match_data_str);

	pcre2_match_data_free(match_data);
}

/**
 * @brief wrap pcre2_get_mark()
 *
 * Note this text from the PCRE2 documentation:
 *
 *	After a successful match, a partial match
 *	(PCRE2_ERROR_PARTIAL), or a failure to match
 *	(PCRE2_ERROR_NOMATCH), a mark name may be
 *	available. The function pcre2_get_mark()
 *	can be called to access this name, which
 *	can be specified in the pattern by any of
 *	the backtracking control verbs, not just
 *	(*MARK). The same function applies to all
 *	the verbs. It returns a pointer to the
 *	zero-terminated name, which is within the
 *	compiled pattern. If no name is available,
 *	NULL is returned. The length of the name
 *	(excluding the terminating zero) is stored
 *	in the code unit that precedes the name. You
 *	should use this length instead of relying
 *	on the terminating zero if the name might
 *	contain a binary zero.
 *
 *  So we are given liberty to index *BACKWARD* from the
 *  given string, as unusual as that might seem.  In fact
 *  we do do this, so we can construct an M string which will
 *  still be OK if it contains zero bytes.  Since we only handle
 *  8 bit code units, the length of the mark name ipso facto
 *  must be <= 255.
 *
 * @param count Paramater count from the M API
 * @param match_data_str A match data handle
 *
 * @return A mark name, or zero length string if none is available
 *
 */
gtm_string_t *mpcre2_get_mark(int count, gtm_char_t *match_data_str) {

	pcre2_match_data *match_data;
	unsigned char mark_len;
	char *mark_name;
	static gtm_string_t ret;

	match_data = (pcre2_match_data *) pointer_decode(match_data_str);

	mark_name = (char *) pcre2_get_mark(match_data);

	ret.address = NULL;
	ret.length = 0;

	if (mark_name) {
		ret.address = mark_name;
		mark_len = (unsigned char) mark_name[-1];	/* see note in function header */
		ret.length = (int) mark_len;
	} 

	return &ret;

}

/**
 * @brief wrap the pcre2_get_ovector_count() function
 *
 *
 * @param count Parameter count from the M API
 * @param match_data_str String handle for a pcre2 *match_data pointer
 *
 * @return The number of pairs in the output vector for this match
 */
 gtm_long_t mpcre2_get_ovector_count(int count, gtm_char_t *match_data_str) {

 	pcre2_match_data *match_data;

	match_data = (pcre2_match_data *) pointer_decode(match_data_str);

	return pcre2_get_ovector_count(match_data);
 }

/**
 * @brief wrap the pcre2_get_ovector_pointer() function
 *
 * This function is provided, but does not have much direct utility in M, as there
 * is no native way to *use* the returned handle.  Instead, the
 * result must be used with the utility function mpcre2_get_ov_pair()
 * (pcre2getovpair in M)
 *
 * @param count The parameter count from the M API
 * @param match_data_str String handle for a match data pointer
 *
 * @return A string handle for the output vector pointer
 */
 gtm_char_t *mpcre2_get_ovector_pointer(int count, gtm_char_t *match_data_str) {

 	pcre2_match_data *match_data;
	PCRE2_SIZE *p;
	static char buf[80];

	match_data = (pcre2_match_data *) pointer_decode(match_data_str);

	p = pcre2_get_ovector_pointer(match_data);

	pointer_encode(p, buf, sizeof(buf));

	return buf;
 }


/**
 * @brief wrap pcre2_get_startchar()
 *
 * @param count Parameter count from M API
 * @param match_data_str Match data handle
 *
 * @return Code unit offset of successful match
 */
gtm_long_t mpcre2_get_startchar(int count, char *match_data_str) {

	pcre2_match_data *match_data;

	match_data = (pcre2_match_data *) pointer_decode(match_data_str);

	return pcre2_get_startchar(match_data);
}

/**
 * @brief Wrap the pcre2_general_context_create() function
 *
 * In general M code should never need to call this function, as m_pcre2
 * creates a hidden general context when needed.  Also, there is no way
 * from M to specify custom C level arguments (unless the M runtime has another
 * plugin which returns appropriate handles).
 *
 * However, if "0" is passed for all three parameters, NULL will be used, and 
 * a general context using the system (not M) memory functions will be returned.
 *
 * @param count Count of parameters provided by the M API
 * @param malloc_ptr_str String handle for a pointer to a pcre2 malloc() compatible function
 * @param free_ptr_str String handle for a pointer to a pcre2 free() compatible function
 * @param data_ptr_str String handle for a pointer to a data item to be used for marking memory
 *
 * @return A string handle to the created pcre2_general_context (or "0" on failure)
 *
 */
gtm_char_t *mpcre2_general_context_create(int count, gtm_char_t *malloc_ptr_str, gtm_char_t *free_ptr_str, gtm_char_t *data_ptr_str) {
	
	pcre2_general_context *gc;
	static char buf[80];

	gc = pcre2_general_context_create(pointer_decode(malloc_ptr_str), pointer_decode(free_ptr_str), pointer_decode(data_ptr_str));

	pointer_encode(gc, buf, sizeof(buf));

	return buf;
}

/**
 * @brief Wrap the pcre2_general_context_copy() function
 *
 * @param count Parameter count provided by the M API
 * @param general_context_str String handle to a pcre2 general context
 *
 * @return A string handle to a copy of the provided general context or "0"
 *
 */ 
 gtm_char_t *mpcre2_general_context_copy(int count, gtm_char_t *general_context_str) {
 	
	pcre2_general_context *gc;
	pcre2_general_context *gc2;
	static char buf[80];

	gc = (pcre2_general_context *) pointer_decode(general_context_str);

	gc2 = pcre2_general_context_copy(gc);

	pointer_encode(gc2, buf, sizeof(buf));

	return buf;
 }

/**
 * @brief Wrap pcre2_general_context_free()
 *
 * @param count Parameter count from the M API
 * @param gc_str String handle to a pcre2 general context
 *
 * @return None
 */
void mpcre2_general_context_free(int count, gtm_char_t *gc_str) {
	
	pcre2_general_context *gc;

	gc = (pcre2_general_context *) pointer_decode(gc_str);

	pcre2_general_context_free(gc);
}

/**
 * @brief Wrap the pcre2_compile_context_create() function
 *
 * @param count Parameter count provided by the M API
 * @param gc_str String handle to a pcre2 general context
 *
 * @return String handle to a pcre2 compile context
 */
gtm_char_t *mpcre2_compile_context_create(int count, gtm_char_t *gc_str) {

	pcre2_general_context *gc;
	pcre2_compile_context *cc;
	static char buf[80];

	gc = (pcre2_general_context *) pointer_decode(gc_str);

	cc = pcre2_compile_context_create(gc);

	pointer_encode(cc, buf, sizeof(buf));

	return buf;
}

/**
 * @brief Wrap the pcre2_compile_context_copy() function
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle for a pcre2 compile context
 *
 * @return String handle for a copy of the given context
 *
 */
gtm_char_t *mpcre2_compile_context_copy(int count, gtm_char_t *ccontext_str) {

	pcre2_compile_context *cc;
	pcre2_compile_context *cc2;
	static char buf[80];

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	cc2 = pcre2_compile_context_copy(cc);

	pointer_encode(cc2, buf, sizeof(buf));

	return buf;
}

/**
 * @brief Wrap the pcre2_compile_context_free() function
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle for a pcre2 compile context
 *
 * @return None
 */
 void mpcre2_compile_context_free(int count, gtm_char_t *ccontext_str) {
	pcre2_compile_context *cc;

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	pcre2_compile_context_free(cc);
 }

/**
 * @brief Wrap the pcre2_set_bsr() function
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle for a pcre2 compile context
 * @param value_str String form of pcre2 code for handling \R
 *
 * @return 0 on success, non zero on failure
 */
gtm_long_t mpcre2_set_bsr(int count, gtm_char_t *ccontext_str, gtm_char_t *value_str) {

	uint32_t value;
	pcre2_compile_context *cc;

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	/*
	 * Parse the bsr option string
	 */
	if (parse_pcre2_options(bsr_opts, n_bsr_opts, "bsr", value_str, &value) < 0) {
		return PCRE2_ERROR_BADDATA;
	}

	return pcre2_set_bsr(cc, value);
}

/**
 * @brief Wrap the  pcre2_set_character_tables() function
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle for a pcre2 compile context
 * @param tables_str String handle for a result from pcre2_maketables, or "NULL"
 *
 * @return Always returns 0
 */
gtm_long_t mpcre2_set_character_tables(gtm_char_t *ccontext_str, gtm_char_t *tables_str) {

	pcre2_compile_context *cc;
	const unsigned char *tables;

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	tables = (const unsigned char*) pointer_decode(tables_str);

	return pcre2_set_character_tables(cc, tables);
}

/**
 * @brief Wrap the pcre2_set_compile_extra_options() function
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle for a pcre2 compile context
 * @param extra_options_str String with pcre2 "extra" compile options
 *
 * @return 0 in all cases
 */
gtm_long_t mpcre2_set_compile_extra_options(int count, gtm_char_t *ccontext_str, gtm_char_t *extra_options_str) {

	pcre2_compile_context *cc;
	uint32_t extra_options;

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	/* not really much point error checking this */
	if (parse_pcre2_options(extra_compile_opts, n_extra_compile_opts, "extra compile", extra_options_str, &extra_options) < 0) {
		return 0;
	}

	return pcre2_set_compile_extra_options(cc, extra_options);
}

/**
 * @brief Wrap the pcre2_set_max_pattern_length() function
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle for a pcre2 compile context
 * @param value The maximum number of code units allowed in a pcre2 pattern
 *
 * @return 0 in all cases
 */
gtm_long_t mpcre2_set_max_pattern_length(int count, gtm_char_t *ccontext_str, gtm_long_t value) {
	pcre2_compile_context *cc;

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	return pcre2_set_max_pattern_length(cc, value);
}

/**
 * @brief Wrap pcre2_set_newline() function
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle for a pcre2 compile context
 * @param value_str String representing the pcre2 newline convention to set
 *
 * @return 0 on success, nonzero on error
 */
gtm_long_t mpcre2_set_newline(int count,gtm_char_t *ccontext_str, gtm_char_t *value_str) {

	pcre2_compile_context *cc;
	uint32_t value;

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	if(parse_pcre2_options(newline_opts, n_newline_opts, "newline opts", value_str, &value) < 0) {
		return PCRE2_ERROR_BADDATA;
	}

	return pcre2_set_newline(cc, value);
}

/**
 * @brief Wrap the pcre2_set_parens_nest_limit() function
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle to a pcre2 compile context
 * @param value The maximum allowed nesting depth for parens
 *
 * @return 0 in all cases
 */
gtm_long_t mpcre2_set_parens_nest_limit(int count, gtm_char_t *ccontext_str, gtm_long_t value) {

	pcre2_compile_context *cc;

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	return pcre2_set_parens_nest_limit(cc, value);
}

/**
 * @brief Wrap the pcre2_set_compile_recursion_guard() function
 *
 * This function is unlikely to be useful in the context of libm_pcre2, as
 * it sets up a callback for use during expression compilation of parenthesized
 * patterns, and libm_pcre2 does not provide any way to create such a function
 * or return a handle to it.  That functionality could be provided by another
 * plugin, but is left as an exercise for the reader.
 *
 * @param count Parameter count from the M API
 * @param ccontext_str String handle for a pcre2 compile context
 * @param guard_function_str String handle for a pointer to a guard function
 * @param user_data String handle for a piece of user data
 *
 * @return 0 in all cases
 *
 */
 gtm_long_t mpcre2_set_compile_recursion_guard(int count, gtm_char_t *ccontext_str, gtm_char_t *guard_function_str, gtm_char_t *user_data_str) {

	pcre2_compile_context *cc;
	void *guard_function;
	void *user_data;

	cc = (pcre2_compile_context *) pointer_decode(ccontext_str);

	guard_function = (void *) pointer_decode(guard_function_str);

	user_data = (void *) pointer_decode(user_data_str);

	return pcre2_set_compile_recursion_guard(cc, guard_function, user_data);
 }

/**
 * @brief Wrap the pcre2_match_context_create() function
 *
 * @param count Parameter count from the M API
 * @param gcontext_str String handle to a pcre2 general context
 *
 * @return String handle to a pcre2 match context
 */
gtm_char_t *mpcre2_match_context_create(int count, gtm_char_t *gcontext_str) {

	pcre2_general_context *gc;
	pcre2_match_context *mc;
	static char buf[80];

	gc = (pcre2_general_context *) pointer_decode(gcontext_str);

	mc = pcre2_match_context_create(gc);

	pointer_encode(mc, buf, sizeof(buf));

	return buf;
}

/**
 * @brief Wrap the pcre2_match_context_copy() function
 *
 * @param count Parameter count from the M API
 * @param mcontext_str String handle to a pcre2 match context
 *
 * @return String handle for a new pcre2 match context
 */
gtm_char_t *mpcre2_match_context_copy(int count, gtm_char_t *mcontext_str) {

	pcre2_match_context *mc;
	pcre2_match_context *mc2;
	static char buf[80];

	mc = (pcre2_match_context *) pointer_decode(mcontext_str);

	mc2 = pcre2_match_context_copy(mc);

	pointer_encode(mc2, buf, sizeof(buf));

	return buf;
}

/**
 * @brief Wrap the pcre2_match_context_free() function
 *
 * @param count Parameter count from the M API
 * @param mcontext_str String handle for a pcre2 match context
 *
 * @return None
 */ 
 void mpcre2_match_context_free(int count, gtm_char_t* mcontext_str) {

	pcre2_match_context *mc;

	mc = (pcre2_match_context *) pointer_decode(mcontext_str);

 	pcre2_match_context_free(mc);
 }

/**
* @brief Wrap the pcre2_set_callout() function
*
* This function is unlikely to be of use in the libm_pcre2 context as it
* sets up a callout function in a pcre2 match context, and libm_pcre2 does not
* provide any way to create such callouts, or return handles for them.  This could
* be done in another plugin and is left as an exercise for the reader.
* 
* @param count Parameter count provided from the M API
* @param mcontext_str String handle for a pcre2 match context
* @param callout_function_str String handle for callout function
* @param callout_data_str String handle for callout data
*
* @return 0 in all cases
*/
gtm_long_t mpcre2_set_callout(int count, gtm_char_t *mcontext_str, gtm_char_t *callout_function_str, gtm_char_t *callout_data_str) {

	pcre2_match_context *mc;
	void *callout_function;
	void *callout_data;

	mc = (pcre2_match_context *) pointer_decode(mcontext_str);

	callout_function = (void *) pointer_decode(callout_function_str);

	callout_data = (void *) pointer_decode(callout_data_str);

	return pcre2_set_callout(mc, callout_function, callout_data);
}

#if 0
 /* Not supported in Ubuntu version yet */
/**
 * @brief Wrap the pcre2_set_substitute_callout() function
 *
 * This function is unlikely to be useful in the libm_pcre2 context, as it sets up a callout function
 * for PCRE2 to call after each substitution made by pcre2_substitute(), and libm_pcre2 provides no way
 * to create such functions or return handles to them though this could be done in another plugin.
 *
 * @param count Parameter count from the M API
 * @param mcontext_str String handle for a pcre2 match context
 * @param callout_function_str String handle for the callout function
 * @param callout_data_str String handle for the callout data
 * 
 * @return 0 in all cases
 */
gtm_long_t mpcre2_set_substitute_callout(int count, gtm_char_t *mcontext_str, gtm_char_t *callout_function_str, gtm_char_t *callout_data_str) {

	pcre2_match_context *mc;
	void *callout_function;
	void *callout_data;

	mc = (pcre2_match_context *) pointer_decode(mcontext_str);

	callout_function = (void *) pointer_decode(callout_function_str);

	callout_data = (void *) pointer_decode(callout_data_str);

	return pcre2_set_substitute_callout(mc, callout_function, callout_data);
}
#endif

/**
 * @brief Wrap the pcre2_set_offset_limit() function
 *
 * @param count Parameter count from the M API
 * @param mcontext_str String handle for a pcre2 match context
 * @param value Value for the offest limit field in the match context
 *
 * @return 0 in all cases
 */
gtm_long_t mpcre2_set_offset_limit(int count,gtm_char_t *mcontext_str, gtm_long_t value) {

	pcre2_match_context *mc;

	mc = (pcre2_match_context *) pointer_decode(mcontext_str);

	return pcre2_set_offset_limit(mc, value);
}


/**
 * @brief Wrap the pcre2_set_heap_limit() function
 *
 * @param count Parameter count from the M API
 * @param mcontext_str String handle for a pcre2 match context
 * @param value Value for the backtracking heap limit field in the match context
 *
 * @return 0 in all cases
 */
gtm_long_t mpcre2_set_heap_limit(int count,gtm_char_t *mcontext_str, gtm_long_t value) {

	pcre2_match_context *mc;

	mc = (pcre2_match_context *) pointer_decode(mcontext_str);

	return pcre2_set_heap_limit(mc, value);
}


/**
 * @brief Wrap the pcre2_set_match_limit() function
 *
 * @param count Parameter count from the M API
 * @param mcontext_str String handle for a pcre2 match context
 * @param value Value for the match limit field in the match context
 *
 * @return 0 in all cases
 */
gtm_long_t mpcre2_set_match_limit(int count,gtm_char_t *mcontext_str, gtm_long_t value) {

	pcre2_match_context *mc;

	mc = (pcre2_match_context *) pointer_decode(mcontext_str);

	return pcre2_set_match_limit(mc, value);
}


/**
 * @brief Wrap the pcre2_set_depth_limit() function
 *
 * @param count Parameter count from the M API
 * @param mcontext_str String handle for a pcre2 depth context
 * @param value Value for the backtracking depth limit field in the match context
 *
 * @return 0 in all cases
 */
gtm_long_t mpcre2_set_depth_limit(int count,gtm_char_t *mcontext_str, gtm_long_t value) {

	pcre2_match_context *mc;

	mc = (pcre2_match_context *) pointer_decode(mcontext_str);

	return pcre2_set_depth_limit(mc, value);
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
 * @brief Wrap the pcre2_substitute() function
 *
 * @param count M API argument count
 * @param code Handle for a compiled PCRE2 regular expression
 * @param subject The string in which to make the substitution
 * @param startoffset The byte offset in the subject to start checking for substitutions
 * @param options Match options as in pcre2_match()
 * @param match_data Match data handle as in pcre2_match()
 * @param mcontext Pcre2 match context handle
 * @param replacemnt The sting to substitute for matched text
 * @param outputbuffer Where to put the copy of the subject with the replacement(s)
 *
 * @return The number of substitutions or < 0 on error
 *
 */
gtm_long_t mpcre2_substitute(int count, gtm_char_t *code_str, gtm_string_t *subject,
	gtm_long_t startoffset, gtm_char_t *options_str, gtm_char_t *match_data_str,
	gtm_char_t *mcontext_str, gtm_string_t *replacement, gtm_string_t *outputbuffer, gtm_long_t *outputlengthptr) {

	pcre2_code *code;
	pcre2_match_data *match_data;
	pcre2_match_context *mc;
	uint32_t options;
	PCRE2_SIZE outputlength;
	int must_free;
	int res;

	code = (pcre2_code *) pointer_decode(code_str);

	if (parse_pcre2_options(match_opts, n_match_opts, "match", options_str, &options) < 0) {
		return -1;
	}

	/*
	 * If match_data_str is "0" set it to NULL
	 */
	if (strcmp(match_data_str, "0") == 0) {
		match_data = NULL;
	} else {
		match_data = (pcre2_match_data *) pointer_decode(match_data_str);
	}

	mc = get_match_context(mcontext_str, &must_free);

	outputlength = outputbuffer->length;
	res = pcre2_substitute(code, (PCRE2_SPTR)subject->address, (PCRE2_SIZE) subject->length,
		(PCRE2_SIZE) startoffset, options, match_data, mc, (PCRE2_SPTR) replacement->address,
		(PCRE2_SIZE) replacement->length, (PCRE2_UCHAR *) outputbuffer->address, &outputlength);

	if (res < 0) {
		outputbuffer->length = 0;
	} else {
		outputbuffer->length = outputlength;
	}

	*outputlengthptr = (gtm_long_t) outputlength;

	if (must_free) {
		pcre2_match_context_free(mc);
	}

	return res ;
}

/**
 * @brief Wrap pcre2_jit_compile()
 *
 * 
 * @param count Parameter count from M API
 * @param code_str Handle for a previously compiled PCRE2 regular expression
 * @param options_str JIT options
 *
 * @return 0 on success < 0 on error
 *
 */
gtm_long_t mpcre2_jit_compile(int count, gtm_char_t *code_str, gtm_char_t *options_str) {

	pcre2_code *code;
	uint32_t options;

	code = (pcre2_code *) pointer_decode(code_str);

	if (parse_pcre2_options(jit_opts, n_jit_opts, "jit", options_str, &options) < 0) {
		return -1;
	}

	return pcre2_jit_compile(code, options);
}


