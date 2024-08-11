#ifndef __GAME
#define __GAME

#include <stdint.h>
#include <stdbool.h>

#include "mibs.h"
#include "lang_parser.h"

typedef enum {
    NONE,
    MAIN_LEVEL_ID,
} level_id;

typedef struct level_t{
    const char* name;
    level_id id;
    struct ctx_t* ctx;

    bool is_initialized;
    bool should_close;
    level_id next_level_id;
    void* data;

    void (*run_logic)(struct level_t* level, struct ctx_t* ctx);
    void (*render_level)(struct level_t* level, struct ctx_t* ctx);
    void (*close_level)(struct level_t* level, struct ctx_t* ctx);
} level_t;

typedef Mibs_Da(level_t*) level_vec_t;

typedef struct ctx_t {
    Mibs_Default_Allocator alloc;
    uint64_t ticks;
    text_vec_t game_text;

    bool is_paused;
    int32_t size_y, size_x;
    int32_t min_y, min_x;

    level_t* current_level;
    level_vec_t levels;
} ctx_t;

bool start_game_loop(ctx_t* ctx);
level_vec_t prepare_levels(Mibs_Default_Allocator* alloc);

#endif