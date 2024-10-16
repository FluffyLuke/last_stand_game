#ifndef __MAIN_LEVEL
#define __MAIN_LEVEL

#include "../game.h"
#include <ncurses/curses.h>
#include <stdint.h>

#define GS_PAUSED_SYMBOL "||"
#define GS_NORMAL_SYMBOL ">"
#define GS_FASTER_SYMBOL ">>"
#define GS_FASTEST_SYMBOL ">>>"

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
    action_task_t* selected_action_task;
    action_task_t* running_action_task;

    action_task_t select_map_task;

    // Map data
    struct {
        map_data_t* data;
        int32_t offset_x, offset_y;
        int32_t selector_x, selector_y;
        map_task_t* current_task;
    } map;
} ml_data;


// void init_select_task(game_ctx_t* game_ctx, ml_data* data, action_task_t* task) {
//     memset(task, 0, sizeof(action_task_t));
//     task->finished = false;
//     task->canceled = false;
//     task->is_selected = false;
//     task->has_map_task = true;
    
//     task->name = get_text_by_id(&game_ctx->game_text, "ml_usa_name").value;
//     task->description = get_text_by_id(&game_ctx->game_text, "ml_usa_desc").value;
//     task->custom_action = select_task_action;
//     task->data = data;
//     task->selectable = NULL;

//     init_map_task(&task->map_task, get_text_by_id(&game_ctx->game_text, "select_task").value);
// }

// void deinit_select_task(action_task_t* task) {

// }

#ifdef __LEVELS

void refresh_map_selector(ml_data* data) {
    data->map.selector_x = data->map_w_x / 2;
    data->map.selector_y = data->map_w_y / 2;
}

bool running_map_task(ml_data* data) {
    return data->map.current_task == NULL;
}

void change_current_selectable(ml_data* data, selectable_t* new_selectable) {
    if(data->selected_action_task != NULL) {
        data->selected_action_task->is_selected = false;
        data->selected_action_task = NULL;
    }

    if(new_selectable != NULL) {
        data->current_selectable = new_selectable;
        if(new_selectable->actions.count > 0) {
            data->selected_action_task = new_selectable->actions.items[0];
            data->selected_action_task->is_selected = true;
        } 
        else data->selected_action_task = NULL;
    } else {
        data->current_selectable = &data->main_selectable;
        data->selected_action_task = data->main_selectable.actions.items[0];
        data->selected_action_task->is_selected = true;
    }
}

void change_running_action(ml_data* data, action_task_t* action) {
    if(data->running_action_task != NULL) 
        data->running_action_task->canceled = true;

    refresh_action_task(action);

    data->running_action_task = action;

    if(action->has_map_task) {
        if(data->map.current_task != NULL) 
            data->map.current_task->canceled = true;
        refresh_map_selector(data);
        data->map.current_task = &action->map_task;
    }
}

void map_select_action(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, action_task_t* task) {
    if(task->map_task.finished != true || task->canceled == true || task->map_task.canceled == true) {
        return;
    }
    map_finding_result result = task->map_task.result;
    ml_data* level_data = (ml_data*)task->data;

    if(result.building != NULL) 
        change_current_selectable(level_data, (selectable_t*)result.building);
    else if(result.unit != NULL)
        change_current_selectable(level_data, (selectable_t*)result.unit);
    else {
        change_current_selectable(level_data, NULL);
    }

    task->finished = true;
}

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

    action_task_t* quit_action = (action_task_t*)malloc(sizeof(action_task_t));
    memset(quit_action, 0, sizeof(action_task_t));
    data->selected_action_task = quit_action;
    data->selected_action_task->is_selected = true;
    quit_action->name = get_text_by_id(&game_ctx->game_text, "ml_ms_quit_a_name").value;
    quit_action->description = get_text_by_id(&game_ctx->game_text, "ml_ms_quit_a_desc").value;
    quit_action->custom_action = exit_game;

    memset(&data->main_selectable.actions, 0, sizeof(actions_vec));
    mibs_da_append(&game_ctx->alloc, &data->main_selectable.actions, quit_action);
    data->current_selectable = &data->main_selectable;
    data->map.offset_x = 0;
    data->map.offset_y = 0;
    data->all_ticks = 0;

    uint32_t map_size_y, map_size_x;
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

    
    init_action_task(
        &data->select_map_task,
        NULL,
        data, 
        true,
        get_text_unit("ml_usa_name"),
        map_select_action,
        get_text_unit("ml_usa_name"),
        get_text_unit("ml_usa_desc")
    );
    data->map.data = (map_data_t*)malloc(sizeof(map_data_t));
    init_map(data->map.data);
    generate_map(data->map.data, map_size_y, map_size_x);

    unit_t* unit = (unit_t*)malloc(sizeof(unit_t));
	create_unit(game_ctx, data->map.data, unit, UT_REACON);
    set_unit_positionyx(unit, 0, 0);
    add_unit(data->map.data, unit);

    unit = (unit_t*)malloc(sizeof(unit_t));
	create_unit(game_ctx, data->map.data, unit, UT_CRAWLER);
    set_unit_positionyx(unit, 5, 5);
    add_unit(data->map.data, unit);

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
            if((i+1) < data->current_selectable->actions.count) {
                i++;
                data->selected_action_task->is_selected = false;
                data->selected_action_task = data->current_selectable->actions.items[i];
                data->selected_action_task->is_selected = true;
            }
        }
        else if(input == KEY_DOWN || input == 's') {
            if(i > 0) {
                i--;
                data->selected_action_task->is_selected = false;
                data->selected_action_task = data->current_selectable->actions.items[i];
                data->selected_action_task->is_selected = true;
            }
        }
        else if(input == KEY_RIGHT || input == 'd') {
            change_running_action(data, data->selected_action_task);
            if(data->running_action_task->has_map_task)
                data->current_window = data->map_w;
        } 
        else if(input == 'p') {
            change_current_selectable(data, &data->main_selectable);
        }
    }

    if(data->running_action_task != NULL) {
        data->running_action_task->custom_action(loop_ctx, game_ctx, data->running_action_task);
        if(data->running_action_task->finished || data->running_action_task->canceled)
            data->running_action_task = NULL;
    }

    if(data->current_window == data->map_w) {
        // Normal map
        if(data->map.current_task == NULL) {
            if(input == KEY_UP || input == 'w') {
            data->map.offset_y--;
            }
            else if(input == KEY_DOWN || input == 's') {
                data->map.offset_y++;
            }
            else if(input == KEY_RIGHT || input == 'd') {
                data->map.offset_x++;
            }
            else if(input == KEY_LEFT || input == 'a') {
                data->map.offset_x--;
            }
            else if(input == 'r' && data->map.current_task == NULL) {
                change_running_action(data, &data->select_map_task);
            } else if(input == 'p') {
                change_current_selectable(data, NULL);
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
                int32_t x, y;
                x = data->map.offset_x + data->map.selector_x;
                y = data->map.offset_y + data->map.selector_y;
                point_t point = point2(x, y);
                data->map.current_task->result = find_on_map(data->map.data, point);
                data->map.current_task = NULL;
            }
            else if(input == 'p') {
                data->map.current_task->finished = true;
                data->map.current_task->canceled = true;
                data->map.current_task = NULL;
            }
        }
    }

    // Run unit logic

    for(int32_t i = 0; i < data->map.data->units->count; i++) {
        unit_t* unit = data->map.data->units->items[i];

        if(unit->health <= 0) {
            mibs_file_log(game_ctx->logs, MIBS_LL_INFO, "Unit \"%s\" died\n", unit->name.text.items);
            unit->deinit(game_ctx, unit);
            free(unit);
            mibs_da_remove(data->map.data->units, i);
            i--;
            continue;
        }
        unit->run_logic(game_ctx, loop_ctx, unit);
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

    for(int32_t i = 0; i < data->current_selectable->actions.count; i++) {
        action_task_t* item = data->current_selectable->actions.items[i];
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


    // Map-terrain 
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

    // Map-units
    units_vec* units = data->map.data->units;
    for(int32_t i = 0; i < units->count; i++) {
        unit_t* unit = units->items[i];

        int32_t color;
        if(unit->enemy) color = COLOR_PAIR(CP_ENEMY);
        else color = COLOR_PAIR(CP_ALLY);



        int32_t unit_y = unit->position.y-data->map.offset_y;
        int32_t unit_x = unit->position.x-data->map.offset_x;

    
        wattron(data->map_w, color);
        if(unit->fighting == true) wattron(data->map_w, A_BLINK);
        mvwprintw(data->map_w, unit_y, unit_x, "%c", unit->symbol);
        wattroff(data->map_w, color);
        wattroff(data->map_w, A_BLINK);
    }

    // Select tool
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
    if(data->current_selectable->actions.count > 0) {
        mvwprintw(main_window, 18, 30, "%s %s", data->selected_action_task->name.text.items, get_text("ml_action_desc_w"));
        mvwprintw(data->action_desc_w, 0, 0, "%s", data->selected_action_task->description.text.items);
    }
}

void close_main_level(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    loop_ctx->current_level->is_initialized = false;
    ml_data* data = (ml_data*)loop_ctx->current_level->data;
    deinit_map(data->map.data);
    free(data->map.data);

    for(int32_t i = 0; i < data->main_selectable.actions.count; i++) {
        free(data->main_selectable.actions.items[i]);
    }

    // deinit_select_task(&data->select_map_task);

    delwin(data->actions_w);
    delwin(data->action_desc_w);
    delwin(data->selectable_desc_w);
    delwin(data->map_w);

    free(loop_ctx->current_level->data);
}

#endif
#endif