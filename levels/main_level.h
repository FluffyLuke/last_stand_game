#ifndef __MAIN_LEVEL
#define __MAIN_LEVEL

#include "../game.h"
#include <ncurses/curses.h>
#include <stdint.h>

#define get_text(id) get_text_by_id(&game_ctx->game_text, (id)).value.text.items

typedef struct {
    WINDOW* actions_w;
    WINDOW* map_w;
    WINDOW* selectable_desc_w;
    WINDOW* action_desc_w;

    selectable_t main_selectable;

    WINDOW* current_window;
    selectable_t* current_selectable;
    custom_item_t* current_action;
} ml_data;

#endif


#define __LEVELS
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


    level->data = data;
    level->is_initialized = true;
    level->should_close = false;
}

void run_main_level_logic(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    ml_data* data = (ml_data*)loop_ctx->current_level->data;
    int32_t input = 0;
    input = wgetch(input_window);

    static int32_t i = 0;

    if(input == ERR) {
        return;
    }

    switch(input) {
        case '1': { data->current_window = data->actions_w; break; }
        case '2': { data->current_window = data->map_w; break; }
        // case '3': { data->current_window = data->name_w; break; }
        // case '4': { data->current_window = data->desc_w; break; }
    }

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
        if(input == KEY_LEFT || input == 'd') {
            data->current_action->custom_action(loop_ctx, game_ctx, NULL);
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

    delwin(data->actions_w);
    delwin(data->action_desc_w);
    delwin(data->selectable_desc_w);
    delwin(data->map_w);

    free(loop_ctx->current_level->data);
}

#endif