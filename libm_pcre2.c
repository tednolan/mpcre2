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

static pcre2_general_context *gcontext;

struct opt_tab {
	const char *name;
	uint32_t val;
};

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

	gcontext = pcre2_general_context_create(m_pcre2_malloc, m_pcre2_free, NULL);

	if (!gcontext) {
		fprintf(stderr, "Unable to allocate general context\n");
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

gtm_char_t *mpcre2_compile(int count, gtm_string_t *pattern, gtm_char_t *options,
	gtm_long_t *errorcode, gtm_ulong_t *erroroffset, gtm_char_t *ccontext_str) {

	static char result[32];
	pcre2_code *code;
	int ecode;
	PCRE2_SIZE eoffset;
	uint32_t compile_options;
	pcre2_compile_context *ccontext;
	char *endstring;
	int must_free_ccontext;

	if(initialize() < 0) {
		result[0] = '\0';
		return result;
	}

	if ( parse_compile_options(options, &compile_options) < 0) {
		result[0] = '\0';
		return result;
	}

	      //pcre2_code *pcre2_compile(PCRE2_SPTR pattern, PCRE2_SIZE length,
	       //        uint32_t options, int *errorcode, PCRE2_SIZE *erroroffset,
		//                pcre2_compile_context *ccontext);

	/*
	 * We don't really need a compile context as we are not setting options here
	 * However, we do want the M malloc/free functions to be used.  If we are
	 * supplied a compile context, use that instead, and do not free it.
	 */
	if (strcmp(ccontext_str, "0") == 0) {
		ccontext = pcre2_compile_context_create(gcontext);
		must_free_ccontext = 1;
	} else {
		ccontext = (pcre2_compile_context *) strtoull(ccontext_str, &endstring, 16);
		if (! (*ccontext_str != '\0') && (*endstring == '\0') ) {
			fprintf(stderr, "ccontext_str %s is not parsable\n", ccontext_str);
			result[0] = '\0';
			return result;
		}
	}

	code = pcre2_compile( (PCRE2_SPTR) (pattern->address), (PCRE2_SIZE) (pattern->length),
		compile_options, &ecode, &eoffset, ccontext);

	if (must_free_ccontext) {
		pcre2_compile_context_free(ccontext);
	}

	snprintf(result, sizeof(result), "%p", code);

	return result;

}

//pcre2_code *pcre2_compile(PCRE2_SPTR pattern, PCRE2_SIZE length, uint32_t options, int *errorcode, PCRE2_SIZE *erroroffset, pcre2_compile_context *ccontext);
