#ifndef __LANG_PARSER
#define __LANG_PARSER

#include "mibs.h"

#define mibs_str(alloc, string) mibs_ds_from_cstr(alloc, string)

typedef enum {
    GL_English
} Game_Language;

// https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
// Initialized in main.c
extern const char * const game_lang_str[];

// TODO create a "deinit" function for text_unit_t
typedef struct {
    Mibs_DString id;
    Mibs_DString text;
} text_unit_t;

typedef Mibs_Da(text_unit_t) game_text_t;

bool load_text(const char* game_path, game_text_t* vec, Game_Language lang);

typedef Mibs_Result(text_unit_t) text_getting_result;

text_getting_result get_text_by_id(game_text_t* vec, const char* id);

#define get_text(id) get_text_by_id(&game_ctx->game_text, (id)).value.text.items
#define get_text_unit(id) get_text_by_id(&game_ctx->game_text, (id)).value

#endif