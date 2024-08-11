#ifndef __LANG_PARSER
#define __LANG_PARSER

#include "mibs.h"

typedef enum {
    English
} language;

typedef struct {
    Mibs_DString id;
    Mibs_DString text;
} text_t;

typedef Mibs_Da(text_t) text_vec_t;

bool load_text(text_vec_t* vec, language lang);

typedef Mibs_Result(text_t) text_finding_result;

text_finding_result find_text_by_id(text_vec_t* vec, Mibs_DString id);

#endif