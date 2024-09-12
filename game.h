#ifndef __GAME
#define __GAME

#include <ncurses/curses.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "mibs.h"
#include "lang_parser.h"

#define MAIN_WINDOW_X 70
#define MAIN_WINDOW_Y 25

#define Mibs_Limited_Num(T) struct { T num; T min_value; T max_value; }

#define mibs_create_limited_num(current, min, max) { \
        .num = (current), \
        .min_value = (min), \
        .max_value = (max) \
} \

#define mibs_increase_limited_num(wn) \
    { \
        if((wn)->num+1 >= (wn)->max_value) { \
            (wn)->num = (wn)->min_value; \
        } else { \
            (wn)->num++;\
        } \
    } \

#define mibs_decrease_limited_num(wn) \
    { \
        if((wn)->num+1 <= (wn)->min_value) { \
            (wn)->num = (wn)->max_value; \
        } else { \
            (wn)->num--;\
        } \
    } \

typedef enum {
    LI_NONE,
    LI_MAIN_MENU,
    LI_OPTIONS,
    LI_MAIN_LEVEL
} Level_Id;

typedef struct loop_ctx_t loop_ctx_t;
typedef struct game_ctx_t game_ctx_t;

struct game_ctx_t;

typedef struct level_t {
    const char* name;
    bool is_popup;
    Level_Id id;
    Level_Id next_level_id;
    struct game_ctx_t* game_ctx;

    bool is_initialized;
    bool should_close;

    void* data;

    void (*init)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx);
    void (*run_logic)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx);
    void (*render)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx);
    void (*close)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx);
} level_t;

typedef level_t popup_t;

typedef Mibs_Da(level_t*) level_vec_t;
typedef Mibs_Da(popup_t*) awaiting_popups_t;
typedef Mibs_Limited_Num(int32_t) limited_num_t;

#define DIFFICULTY_AMOUNT 3
typedef enum {
    GD_EASY = 0,
    GD_NORMAL = 1,
    GD_HARD = 2,
} Game_Difficulty;

typedef enum {
    GS_PAUSED = 0,
    GS_NORMAL = 1,
    GS_FASTER = 3,
    GS_FASTEST = 5
} Game_Speed;

// typedef enum {
//     CC_TERRAIN_LOW = 8, // Ncurses has 8 basic colors, indexed from 0
//     CC_TERRAIN_MEDIUM,
//     CC_TERRAIN_HIGH
// } Custom_Color;

typedef enum {
    CP_RED_TEXT = 1,
    CP_ENEMY,
    CP_ALLY,
    CP_GRASS,
    CP_MOUNTAIN,
    CP_WATER,
    CP_TERRAIN_LOW,
    CP_TERRAIN_MEDIUM,
    CP_TERRAIN_HIGH
} Color_Pairs;

// https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
// Initialized in main.c
extern const char * const game_difficulty_str[];

// Window used for input
// Initialized in game.c
extern WINDOW* input_window;

typedef struct game_ctx_t {
    Mibs_Default_Allocator alloc;
    FILE* logs;
    uint64_t ticks;

    Game_Language language;
    Game_Difficulty difficulty;
    Game_Speed speed;

    int32_t size_y, size_x;
    int32_t min_y, min_x;

    game_text_t game_text;
    level_vec_t levels;
    awaiting_popups_t popups;
} game_ctx_t;


typedef struct loop_ctx_t {
    double delta_ticks;
    level_t* current_level;
    popup_t* current_popup;
    uint32_t ticks;
    WINDOW* main_window;
} loop_ctx_t;

typedef struct {
    text_unit_t name;
    text_unit_t description;
    void (*custom_action)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data);
    // This attributes ale only used for rendering, not logic
    bool is_selected;
    bool is_clicked;
} custom_item_t;

typedef Mibs_Da(custom_item_t*) custom_items_vec;

bool start_game_loop(game_ctx_t* game_ctx);
level_vec_t prepare_levels(Mibs_Default_Allocator* alloc);

void custom_item_init(
    game_ctx_t* game_ctx,
    custom_item_t* item,
    const char* name_id, 
    const char* desc_id, 
    void (*custom_action)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data)
    );

// Used when no action needs to be provided
void empty_action(WINDOW* main_window, level_t* level, game_ctx_t* game_ctx, void* data);
void exit_game(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data);

void center_window(WINDOW* parentwin, WINDOW* subwin);
int32_t calculate_padding_ds(Mibs_DString* text, uint32_t size_x);
int32_t calculate_padding_cstr(const char* text, uint32_t size_x);

// --------------------
// Main game
// --------------------

#define REACON_SYMBOL 'r'
#define SQUAD_SYMBOL 's'
#define MOUTAIN_SYMBOL '^'
#define WATER_SYMBOL '~'

typedef struct {
    text_unit_t name;
    text_unit_t desciption;
    custom_items_vec* actions;
} selectable_t;

typedef enum {
    UT_REACON,
    UT_SQUAD,
} Unit_Type;

typedef struct unit_t {
    text_unit_t name;
    text_unit_t desciption;
    custom_items_vec* actions;

    Unit_Type type;
    bool enemy;

    int32_t health;
    int32_t armor;
    int32_t size;
    int32_t sight_range;
    int32_t attack_range;

    // void (*init)(unit_t* unit);
    void (*deinit)(struct unit_t* unit);
    void* data;
    game_ctx_t* game_ctx;
} unit_t;
typedef Mibs_Da(unit_t*) units_vec;

typedef struct {
    text_unit_t name;
    text_unit_t desciption;
    custom_items_vec* actions;
} building_t;
typedef Mibs_Da(unit_t*) buildings_vec;

typedef struct {
    int32_t height;
    //double height;
    bool passable;
} terrain_t;

typedef struct {
    units_vec* units;
    buildings_vec* buildings;

    uint32_t size_x;
    uint32_t size_y;
    terrain_t** terrain;
} map_data_t;

typedef struct {
    double x;
    double y;
} vector2_t;

#define vector2(n_x, n_y) { .x = (n_x), .y = n_y }

typedef struct {
    int32_t x;
    int32_t y;
} point_t;

#define point2(n_x, n_y) { .x = (n_x), .y = n_y }

void generate_map(map_data_t* map, int32_t size_y, int32_t size_x);
void deinit_map(map_data_t* map);

typedef struct {
    text_unit_t task_name;
    bool finished;
    bool canceled;
    point_t point;
} map_task_t;

#define map_task(t_n) { .task_name = (t_n), .finished = false, .canceled = false, .point.x = -1, .point.y = -1}

#endif
