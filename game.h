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

typedef struct {
    int32_t x;
    int32_t y;
} point_t;

#define point2(n_x, n_y) { .x = (n_x), .y = n_y }
bool compare_points(point_t point_a, point_t point_b);

typedef struct {
    double sum;
    double border;
} tick_timer_t;

void init_tick_timer(tick_timer_t* timer, double border);
bool check_tick_timer(tick_timer_t* timer);
bool add_ticks_to_timer(tick_timer_t* timer, double delta_ticks);

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
    double ticks;
    WINDOW* main_window;
} loop_ctx_t;

bool start_game_loop(game_ctx_t* game_ctx);
level_vec_t prepare_levels(Mibs_Default_Allocator* alloc);

// Used when no action needs to be provided
void empty_action(WINDOW* main_window, level_t* level, game_ctx_t* game_ctx, void* data);


typedef struct {
	struct terrain_t* terrain;
	struct unit_t* unit;
	struct building_t* building;
	point_t position;
} map_finding_result;

typedef struct map_task_t {
    text_unit_t task_name;
    bool finished;
    bool canceled;
    map_finding_result result;
} map_task_t;

typedef struct action_task_t {
    bool finished;
    bool canceled;
    bool has_map_task;
    map_task_t map_task;
    void* selectable;
    void* data;
    //void (*reset_action_data)(struct action_task_t* task);
    void (*custom_action)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, struct action_task_t* task);

    text_unit_t name;
    text_unit_t description;

    // Used only for rendering
    bool is_selected;
} action_task_t;
typedef Mibs_Da(action_task_t*) actions_vec;

void init_action_task(
    struct action_task_t* task,
    void* selectable,
    void* data,
    bool has_map_task,
    text_unit_t map_task_name,
    //void (*reset_action_data)(action_task_t* task),
    void (*custom_action)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, struct action_task_t* task),

    text_unit_t name,
    text_unit_t description
);

void exit_game(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, struct action_task_t* data);

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
    actions_vec actions;
} selectable_t;

typedef enum {
    UT_REACON,
    UT_SQUAD,
} Unit_Type;

typedef struct {
    point_t point;
    tick_timer_t move_timer;
} move_point_t;

typedef Mibs_Da(move_point_t) move_points;

typedef struct unit_t {
    text_unit_t name;
    text_unit_t desciption;
    actions_vec actions;

    Unit_Type type;
    point_t position;
    bool enemy;

    move_points path;

    int8_t symbol;
    int32_t health;
    int32_t armor;
    int32_t size;
    int32_t sight_range;
    int32_t attack_range;

    void (*run_logic)(game_ctx_t* game_ctx, loop_ctx_t* loop_ctx, struct unit_t* unit);
    void (*deinit)(game_ctx_t* game_ctx, struct unit_t* unit);
    void* data;
    game_ctx_t* game_ctx;
} unit_t;

void create_unit(game_ctx_t* game_ctx, unit_t* unit, Unit_Type type);
void set_unit_position(unit_t* unit, point_t* new_position);
void set_unit_positionyx(unit_t* unit, int32_t y, int32_t x);

typedef Mibs_Da(unit_t*) units_vec;

typedef struct building_t {
    text_unit_t name;
    text_unit_t desciption;
    actions_vec actions;

    point_t position;
} building_t;
typedef Mibs_Da(building_t*) buildings_vec;

typedef struct terrain_t {
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

void init_map(map_data_t* map);
void generate_map(map_data_t* map, int32_t size_y, int32_t size_x);
void deinit_map(map_data_t* map);
void add_unit(map_data_t* map, unit_t* unit);

void refresh_map_task(map_task_t* task);

void init_map_task(map_task_t* task, text_unit_t text_unit);

void refresh_action_task(action_task_t* task);

map_finding_result find_on_map(map_data_t* map, point_t position);

#define map_task(t_n) { .task_name = (t_n), .finished = false, .canceled = false, .point.x = -1, .point.y = -1}

#endif
