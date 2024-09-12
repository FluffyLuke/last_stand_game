#ifndef __MAIN_LEVEL
#define __MAIN_LEVEL

#include "../game.h"
#include <ncurses/curses.h>
#include <stdint.h>

#define GS_PAUSED_SYMBOL "||"
#define GS_NORMAL_SYMBOL ">"
#define GS_FASTER_SYMBOL ">>"
#define GS_FASTEST_SYMBOL ">>>"

#define get_text(id) get_text_by_id(&game_ctx->game_text, (id)).value.text.items

typedef struct {
    WINDOW* actions_w;
    WINDOW* map_w;
    WINDOW* selectable_desc_w;
    WINDOW* action_desc_w;
    double all_ticks;

    int32_t actions_w_y, actions_w_x;
    int32_t map_w_y, map_w_x;
    int32_t selectable_desc_w_y, selectable_desc_w_x;
    int32_t action_desc_w_y, action_desc_w_x;

    selectable_t main_selectable;

    WINDOW* current_window;
    selectable_t* current_selectable;
    custom_item_t* current_action;

    // Map data
    struct {
        map_data_t* data;
        int32_t offset_x, offset_y;
        int32_t selector_x, selector_y;
        bool reset_selector;
        map_task_t* current_task;
    } map;
} ml_data;

bool change_map_task(ml_data* data, map_task_t* task) {
    if(data->map.current_task != NULL) {
        return false;
    }
    data->map.current_task = task;
    data->map.reset_selector = true;
    return true;
}

void force_change_map_task(ml_data* data, map_task_t* task) {
    data->map.current_task->finished = true;
    data->map.current_task->canceled = true;
    data->map.current_task = task;
    data->map.reset_selector = true;
}

bool running_map_task(ml_data* data) {
    return data->map.current_task == NULL;
}

void init_map_task(map_task_t* task, text_unit_t text_unit) {
    task->task_name = text_unit;
    task->canceled = false;
    task->finished = false;
    task->point.x = -1;
    task->point.y = -1;
}

#ifdef __LEVELS

void init_main_level(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    ml_data* data = (ml_data*)malloc(sizeof(ml_data));
    memset(data, 0, sizeof(ml_data));
    level_t* level = loop_ctx->current_level;

    data->actions_w = subwin(loop_ctx->main_window, 17, 25, 1, 1);
    data->selectable_desc_w = subwin(loop_ctx->main_window, 5, 25, 19, 1);
    data->map_w = subwin(loop_ctx->main_window, 17, 42, 1, 27);
    data->action_desc_w = subwin(loop_ctx->main_window, 5, 42, 19, 27);
    data->current_window = data->actions_w;
    getmaxyx(data->actions_w, data->actions_w_y, data->actions_w_x);
    getmaxyx(data->map_w, data->map_w_y, data->map_w_x);
    getmaxyx(data->selectable_desc_w, data->selectable_desc_w_y, data->selectable_desc_w_x);
    getmaxyx(data->action_desc_w, data->action_desc_w_y, data->action_desc_w_x);

    data->main_selectable.name = get_text_by_id(&game_ctx->game_text, "ml_ms_name").value;
    data->main_selectable.desciption = get_text_by_id(&game_ctx->game_text, "ml_ms_desc").value;

    custom_item_t* quit_action = (custom_item_t*)malloc(sizeof(custom_item_t));
    memset(quit_action, 0, sizeof(custom_item_t));
    data->current_action = quit_action;
    data->current_action->is_selected = true;
    quit_action->name = get_text_by_id(&game_ctx->game_text, "ml_ms_quit_a_name").value;
    quit_action->description = get_text_by_id(&game_ctx->game_text, "ml_ms_quit_a_desc").value;
    quit_action->custom_action = exit_game;
    data->main_selectable.actions = (custom_items_vec*)malloc(sizeof(custom_items_vec));
    memset(data->main_selectable.actions, 0, sizeof(custom_items_vec));
    mibs_da_append(&game_ctx->alloc, data->main_selectable.actions, quit_action);
    data->current_selectable = &data->main_selectable;
    data->map.offset_x = 0;
    data->map.offset_y = 0;
    data->all_ticks = 0;

    uint32_t map_size_y, map_size_x;
    data->map.data = (map_data_t*)malloc(sizeof(map_data_t));
    switch (game_ctx->difficulty) {
        case GD_EASY: {
            map_size_y = 50;
            map_size_x = 50;
            break;
        }
        case GD_NORMAL: {
            map_size_y = 75;
            map_size_x = 75;
            break;
        }
        case GD_HARD: {
            map_size_y = 100;
            map_size_x = 100;
            break;
        }
        default: {
            map_size_y = 50;
            map_size_x = 50;
            break;
        }
    }

    generate_map(data->map.data, map_size_y, map_size_x);

    level->data = data;
    level->is_initialized = true;
    level->should_close = false;
}

void run_main_level_logic(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    ml_data* data = (ml_data*)loop_ctx->current_level->data;
    int32_t input = 0;
    input = wgetch(input_window);

    data->all_ticks += loop_ctx->delta_ticks;
    static int32_t i = 0;

    // if(input == ERR) {
    //     return;
    // }

    // Universal input
    switch(input) {
        case 'q': { data->current_window = data->actions_w; break; }
        case 'e': { data->current_window = data->map_w; break; }
        // case '3': { data->current_window = data->name_w; break; }
        // case '4': { data->current_window = data->desc_w; break; }
        case '1': { game_ctx->speed = GS_PAUSED; break; }
        case '2': { game_ctx->speed = GS_NORMAL; break; }
        case '3': { game_ctx->speed = GS_FASTER; break; }
        case '4': { game_ctx->speed = GS_FASTEST; break; }
    }

    // Action window input
    if(data->current_window == data->actions_w) {
        if(input == KEY_UP || input == 'w') {
            if((i+1) < data->current_selectable->actions->count) {
                i++;
                data->current_action->is_selected = false;
                data->current_action = data->current_selectable->actions->items[i];
                data->current_action->is_selected = true;
            }
        }
        if(input == KEY_DOWN || input == 's') {
            if(i > 0) {
                i--;
                data->current_action->is_selected = false;
                data->current_action = data->current_selectable->actions->items[i];
                data->current_action->is_selected = true;
            }
        }
        if(input == KEY_RIGHT || input == 'd') {
            data->current_action->custom_action(loop_ctx, game_ctx, NULL);
        }
    }

    // Map window input

    // Reset select tool
    if(data->map.reset_selector) {
        data->map.reset_selector = false;
        data->map.selector_x = data->map_w_x / 2;
        data->map.selector_y = data->map_w_y / 2;
    }

    if(data->current_window == data->map_w) {
        // Normal map
        if(data->map.current_task == NULL) {
            if(input == KEY_UP || input == 'w') {
            data->map.offset_y--;
            }
            if(input == KEY_DOWN || input == 's') {
                data->map.offset_y++;
            }
            if(input == KEY_RIGHT || input == 'd') {
                data->map.offset_x++;
            }
            if(input == KEY_LEFT || input == 'a') {
                data->map.offset_x--;
            }
        // Selecting
        } else {
            if(input == KEY_UP || input == 'w') {
                if(data->map.selector_y-2 <= 0) data->map.offset_y--;
                else data->map.selector_y--;
            }
            else if(input == KEY_DOWN || input == 's') {
                if(data->map.selector_y >= data->map_w_y-3) data->map.offset_y++;
                else data->map.selector_y++;
            }
            else if(input == KEY_RIGHT || input == 'd') {
                if(data->map.selector_x >= data->map_w_x-4) data->map.offset_x++;
                else data->map.selector_x++;
            }
            else if(input == KEY_LEFT || input == 'a') {
                if(data->map.selector_x-3 <= 0) data->map.offset_x--;
                else data->map.selector_x--;
            } 
            else if(input == '\n') {
                data->map.current_task->finished = true;
                data->map.current_task->point.x = data->map.selector_x;
                data->map.current_task->point.x = data->map.selector_y;
                data->map.current_task = NULL;
            }
            else if(input == KEY_BACKSPACE) {
                data->map.current_task->finished = true;
                data->map.current_task->canceled = true;
                data->map.current_task = NULL;
            }
        }
    }
}

void render_main_level(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    static int32_t size_y = MAIN_WINDOW_Y, size_x = MAIN_WINDOW_X;
    ml_data* data = (ml_data*)loop_ctx->current_level->data;

    WINDOW* main_window = loop_ctx->main_window;

    mvwvline(main_window, 0, 26, '|', size_y);
    mvwhline(main_window, 18, 0, '-', size_x);

    // TODO use variable for holding texts instead of searching for them every iteration

    // Action list (top-left)
    if(data->current_window == data->actions_w) {
        wattron(main_window, A_STANDOUT);
        mvwprintw(main_window, 0, 16, "%s", get_text("ml_actions_w"));
        wattroff(main_window, A_STANDOUT);
    } else {
        mvwprintw(main_window, 0, 16, "%s", get_text("ml_actions_w"));
    }
    for(int32_t i = 0; i < data->current_selectable->actions->count; i++) {
        custom_item_t* item = data->current_selectable->actions->items[i];
        if(item->is_selected) wattron(data->actions_w, A_STANDOUT);
        mvwprintw(data->actions_w, i*2+1, 1, "                       ");
        mvwprintw(data->actions_w, i*2+1, 1, "%s", item->name.text.items);
        if(item->is_selected) wattroff(data->actions_w, A_STANDOUT);
    }

    // Map (top-right)
    if(data->current_window == data->map_w) {
        wattron(main_window, A_STANDOUT);
        mvwprintw(main_window, 0, 30, "%s", get_text("ml_map_w"));
        wattroff(main_window, A_STANDOUT);
    } else {
        mvwprintw(main_window, 0, 30, "%s", get_text("ml_map_w"));
    }

    int8_t secs, minutes, hours;
    secs = (int64_t)data->all_ticks / 50 % 60;
    minutes = (int64_t)data->all_ticks / 50 / 60 % 60;
    hours = (int64_t)data->all_ticks / 50 / 60 / 60 % 24;
    mvwprintw(main_window, 0, 47, "%d", hours);
    mvwprintw(main_window, 0, 49, ":", hours);
    mvwprintw(main_window, 0, 50, "%d", minutes);
    mvwprintw(main_window, 0, 52, ":", hours);
    mvwprintw(main_window, 0, 53, "%d", secs);

    if(game_ctx->speed == GS_PAUSED) wattron(main_window, A_STANDOUT);
    mvwprintw(main_window, 0, 57, GS_PAUSED_SYMBOL);
    if(game_ctx->speed == GS_PAUSED) wattroff(main_window, A_STANDOUT);

    if(game_ctx->speed == GS_NORMAL) wattron(main_window, A_STANDOUT);
    mvwprintw(main_window, 0, 60, GS_NORMAL_SYMBOL);
    if(game_ctx->speed == GS_NORMAL) wattroff(main_window, A_STANDOUT);

    if(game_ctx->speed == GS_FASTER) wattron(main_window, A_STANDOUT);
    mvwprintw(main_window, 0, 62, GS_FASTER_SYMBOL);
    if(game_ctx->speed == GS_FASTER) wattroff(main_window, A_STANDOUT);

    if(game_ctx->speed == GS_FASTEST) wattron(main_window, A_STANDOUT);
    mvwprintw(main_window, 0, 65, GS_FASTEST_SYMBOL);
    if(game_ctx->speed == GS_FASTEST) wattroff(main_window, A_STANDOUT);


    // Map-terrain (normal)
    for(int32_t iy = 0; iy < data->map_w_y; iy++) {
        for(int32_t ix = 0; ix < data->map_w_x; ix++) {
            if((iy+data->map.offset_y) >= data->map.data->size_y || (ix+data->map.offset_x) >= data->map.data->size_x) continue;

            terrain_t* terrain = &data->map.data->terrain[iy+data->map.offset_y][ix+data->map.offset_x];
            if(terrain->height == 0) {
                wattron(data->map_w, COLOR_PAIR(CP_WATER));
                mvwprintw(data->map_w, iy, ix, "%c", WATER_SYMBOL);
                wattroff(data->map_w, COLOR_PAIR(CP_WATER));
            } else if(terrain->height == 10) {
                wattron(data->map_w, COLOR_PAIR(CP_MOUNTAIN));
                mvwprintw(data->map_w, iy, ix, "%c", MOUTAIN_SYMBOL);
                wattroff(data->map_w, COLOR_PAIR(CP_MOUNTAIN));
            } else {
                wattron(data->map_w, COLOR_PAIR(CP_GRASS));
                mvwprintw(data->map_w, iy, ix, " ");
                wattroff(data->map_w, COLOR_PAIR(CP_GRASS));
            }
        }
    }

    // Map-terrain (select)
    if(data->map.current_task != NULL) {
        mvwvline(data->map_w, data->map.selector_y-2, data->map.selector_x, '|', 2);
        mvwvline(data->map_w, data->map.selector_y+1, data->map.selector_x, '|', 2);
        mvwprintw(data->map_w, data->map.selector_y, data->map.selector_x-3, "---");
        mvwprintw(data->map_w, data->map.selector_y, data->map.selector_x+1, "---");
    }

    // Selectable description (bottom-left)
    mvwprintw(main_window, 18, 3, "%s", data->current_selectable->name.text.items);
    mvwprintw(data->selectable_desc_w, 0, 0, "%s", data->current_selectable->desciption.text.items);

    // Selected action description (bottom-right)
    mvwprintw(main_window, 18, 30, "%s %s", data->current_action->name.text.items, get_text("ml_action_desc_w"));
    mvwprintw(data->action_desc_w, 0, 0, "%s", data->current_action->description.text.items);
}

void close_main_level(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    loop_ctx->current_level->is_initialized = false;
    ml_data* data = (ml_data*)loop_ctx->current_level->data;
    deinit_map(data->map.data);
    free(data->map.data);

    for(int32_t i = 0; i < data->main_selectable.actions->count; i++) {
        free(data->main_selectable.actions->items[i]);
    }

    free(data->main_selectable.actions);

    delwin(data->actions_w);
    delwin(data->action_desc_w);
    delwin(data->selectable_desc_w);
    delwin(data->map_w);

    free(loop_ctx->current_level->data);
}

#endif
#endif