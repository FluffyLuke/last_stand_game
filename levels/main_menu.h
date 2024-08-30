#ifndef __MAIN_MENU
#define __MAIN_MENU

#include "../game.h"
#include <ncurses/curses.h>
#include <stddef.h>

// Main menu data
typedef struct {
    custom_item_t play_button;
    custom_item_t options_button;
    custom_item_t exit_button;

    custom_item_t* current_button;

    // Button number
    uint8_t bn;
} mm_data;

// Exit popup data
typedef struct {
    WINDOW* popup_window;
    Game_Speed previous_speed;

    custom_item_t text;
    custom_item_t confirm_button;
    custom_item_t return_button;

    custom_item_t* current_button;
} ep_data;

#endif

#ifdef __LEVELS

void ep_confirm(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    exit_game(loop_ctx, game_ctx, NULL);
    loop_ctx->current_level->should_close = true;
}

void ep_return(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    loop_ctx->current_popup->should_close = true;
}

void init_exit_popup(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    ep_data* data = (ep_data*)malloc(sizeof(ep_data));
    memset(data, 0, sizeof(ep_data));
    popup_t* popup = loop_ctx->current_popup;
    data->previous_speed = game_ctx->speed;
    game_ctx->speed = GS_PAUSED;

    // Window placement will be calculated later in "render"
    data->popup_window = subwin(loop_ctx->main_window, 10, 50, 0, 0);
    
    data->text.name = get_text_by_id(&game_ctx->game_text, "mmep_text").value;
    data->confirm_button.name = get_text_by_id(&game_ctx->game_text, "mmep_confirm_button").value;
    data->return_button.name = get_text_by_id(&game_ctx->game_text, "mmep_return_button").value;

    data->confirm_button.custom_action = ep_confirm;

    data->return_button.is_selected = true;
    data->current_button = &data->return_button;
    data->return_button.custom_action = ep_return;

    popup->data = data;
    popup->is_initialized = true;
}

void run_exit_popup(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    ep_data* data = (ep_data*)loop_ctx->current_popup->data;
    int32_t input = wgetch(input_window);

    if(input == ERR) {
        return;
    }

    if(input == KEY_UP || input == 'a') {
        data->current_button->is_selected = false;
        data->current_button = &data->confirm_button;
        data->current_button->is_selected = true;
    }
    if(input == KEY_DOWN || input == 'd') {
        data->current_button->is_selected = false;
        data->current_button = &data->return_button;
        data->current_button->is_selected = true;
    }
    if(input == '\n' || input == 'w') {
        data->current_button->custom_action(loop_ctx, game_ctx, NULL);
    }
}

void render_exit_popup(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    uint32_t size_y, size_x;
    ep_data* data = (ep_data*)loop_ctx->current_popup->data;
    center_window(loop_ctx->main_window, data->popup_window);

    werase(data->popup_window);

    getyx(data->popup_window, size_y, size_x);
    box(data->popup_window, 0, 0);

    mvwprintw(data->popup_window, 2, 2, "%s", data->text.name.text.items);

    if(data->confirm_button.is_selected) {
        wattron(data->popup_window, A_STANDOUT);
    }
    mvwprintw(data->popup_window, 5, 2, "%s", data->confirm_button.name.text.items);
    if(data->confirm_button.is_selected) {
        wattroff(data->popup_window, A_STANDOUT);
    }

    if(data->return_button.is_selected) {
        wattron(data->popup_window, A_STANDOUT);
    }
    mvwprintw(data->popup_window, 5, 8, "%s", data->return_button.name.text.items);
    if(data->return_button.is_selected) {
        wattroff(data->popup_window, A_STANDOUT);
    }
}

void close_exit_popup(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    ep_data* data = (ep_data*)loop_ctx->current_popup->data;
    game_ctx->speed = data->previous_speed;
    loop_ctx->current_popup->is_initialized = false;
    free(loop_ctx->current_popup->data);
}

void mm_play(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    loop_ctx->current_level->should_close = true;
    loop_ctx->current_level->next_level_id = LI_MAIN_LEVEL;
}

void mm_show_options(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    loop_ctx->current_level->should_close = true;
    loop_ctx->current_level->next_level_id = LI_OPTIONS;
}

void mm_exit_game(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    popup_t* popup = (popup_t*)malloc(sizeof(popup_t));
    memset(popup, 0, sizeof(popup_t));

    popup->name = "Exiting game";
    popup->init = init_exit_popup;
    popup->run_logic = run_exit_popup;
    popup->render = render_exit_popup;
    popup->close = close_exit_popup;
    mibs_da_append(&game_ctx->alloc, &game_ctx->popups, popup);
}

void init_main_menu(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    mm_data* data = (mm_data*)malloc(sizeof(mm_data));
    level_t* level = loop_ctx->current_level;
    memset(data, 0, sizeof(mm_data));

    data->play_button.name = get_text_by_id(&game_ctx->game_text, "mm_play_button").value;
    data->options_button.name = get_text_by_id(&game_ctx->game_text, "mm_options_button").value;
    data->exit_button.name = get_text_by_id(&game_ctx->game_text, "mm_quit_button").value;
    
    data->current_button = &data->play_button;
    data->current_button->is_selected = true;

    data->play_button.custom_action = mm_play;
    data->options_button.custom_action = mm_show_options;
    data->exit_button.custom_action = mm_exit_game;

    data->bn = 0;

    level->data = data;
    level->is_initialized = true;
    level->should_close = false;
}

void run_main_menu_logic(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    int input = 0;

    mm_data* data = (mm_data*)loop_ctx->current_level->data;
    input = wgetch(input_window);

    if(input == ERR) {
        return;
    }

    bool flag = false;

    if(input == KEY_UP || input == 'w') {
        if(data->bn == 0) { data->bn = 2; }
        else { data->bn--; }
        flag = true;
    }
    if(input == KEY_DOWN || input == 's') {
        if(data->bn == 2) { data->bn = 0; }
        else { data->bn++; }
        flag = true;
    }

    if(flag) {
        data->current_button->is_selected = false;
        switch (data->bn) {
            case 0: data->current_button = &data->play_button; break;
            case 1: data->current_button = &data->options_button; break;
            case 2: data->current_button = &data->exit_button; break;
        }
        data->current_button->is_selected = true;
    }

    if(input == '\n' || input == KEY_LEFT || input == 'd') {
        data->current_button->custom_action(loop_ctx, game_ctx, NULL);
    }
}

void render_main_menu(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    static int32_t size_y = MAIN_WINDOW_Y, size_x = MAIN_WINDOW_X;
    WINDOW* main_window = loop_ctx->main_window;
    mm_data* data = (mm_data*)loop_ctx->current_level->data;

    //getmaxyx(main_window, size_y, size_x);

    mvwvline(main_window, 0, 26, '|', size_y);
    mvwhline(main_window, 18, 0, '-', size_x);

    if(data->play_button.is_selected) {
        wattron(main_window, A_STANDOUT);
        mvwprintw(main_window, 2, 2, "                       ");
        mvwprintw(main_window, 2, 5, "%s", data->play_button.name.text.items);
        wattroff(main_window, A_STANDOUT);
    } else {
        mvwprintw(main_window, 2, 2, "%s", data->play_button.name.text.items);
    }

    if(data->options_button.is_selected) {
        wattron(main_window, A_STANDOUT);
        mvwprintw(main_window, 4, 2, "                       ");
        mvwprintw(main_window, 4, 5, "%s", data->options_button.name.text.items);
        wattroff(main_window, A_STANDOUT);
    } else {
        mvwprintw(main_window, 4, 2, "%s", data->options_button.name.text.items);
    }

    if(data->exit_button.is_selected) {
        wattron(main_window, A_STANDOUT);
        mvwprintw(main_window, 6, 2, "                       ");
        mvwprintw(main_window, 6, 5, "%s", data->exit_button.name.text.items);
        wattroff(main_window, A_STANDOUT);
    } else {
        mvwprintw(main_window, 6, 2, "%s", data->exit_button.name.text.items);
    }
}

void close_main_menu(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    loop_ctx->current_level->is_initialized = false;
    mm_data* data = (mm_data*)loop_ctx->current_level->data;
    data->current_button->is_selected = false;

    free(loop_ctx->current_level->data);
}

#endif