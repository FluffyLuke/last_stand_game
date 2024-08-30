#ifndef __OPTIONS
#define __OPTIONS

#include "../game.h"

typedef struct {
    custom_item_t language_switch;
    custom_item_t difficulty_switch;
    custom_item_t mm_return_button;

    custom_item_t* current_item;

    // Button number
    uint8_t bn;
} options_data;

#endif

#ifdef __LEVELS

void options_change_difficulty(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    int8_t i = *(int8_t*)data;
    if(i > 0) {
        if(game_ctx->difficulty != GD_HARD) game_ctx->difficulty++;
    } else {
        if(game_ctx->difficulty != GD_EASY) game_ctx->difficulty--;
    }
}

void options_change_language(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    // Only one language is available
    // int8_t i = *(int8_t*)data;
    // limited_num_t n = mibs_create_limited_num(game_ctx->difficulty, 0, DIFFICULTY_AMOUNT-1);
    // if(i > 0) {
    //     mibs_increase_limited_num(&n);
    //     game_ctx->language = n.num;
    // } else {
    //     mibs_decrease_limited_num(&n);
    //     game_ctx->language = n.num;
    // }
}

void options_return(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    loop_ctx->current_level->should_close = true;
    loop_ctx->current_level->next_level_id = LI_MAIN_MENU;
};

void init_options(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    options_data* data = (options_data*)malloc(sizeof(options_data));
    memset(data, 0, sizeof(options_data));

    data->language_switch.name = get_text_by_id(&game_ctx->game_text, "options_lang_switch").value;
    data->difficulty_switch.name = get_text_by_id(&game_ctx->game_text, "options_difficulty_switch").value;
    data->mm_return_button.name = get_text_by_id(&game_ctx->game_text, "options_return_button").value;

    data->language_switch.custom_action = options_change_language;
    data->difficulty_switch.custom_action = options_change_difficulty;
    data->mm_return_button.custom_action = options_return;

    data->current_item = &data->language_switch;
    data->current_item->is_selected = true;

    data->bn = 0;

    loop_ctx->current_level->data = data;
    loop_ctx->current_level->is_initialized = true;
    loop_ctx->current_level->should_close = false;
}

void run_options_logic(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    int input = 0;
    options_data* data = (options_data*)loop_ctx->current_level->data;

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
        data->current_item->is_selected = false;
        switch (data->bn) {
            case 0: data->current_item = &data->language_switch; break;
            case 1: data->current_item = &data->difficulty_switch; break;
            case 2: data->current_item = &data->mm_return_button; break;
        }
        data->current_item->is_selected = true;
    }

    static int32_t action_data;
    if(input == '\n' || input == KEY_RIGHT || input == 'd') {
        action_data = 1;
        data->current_item->custom_action(loop_ctx, game_ctx, &action_data);
    }

    if(input == KEY_LEFT || input == 'a') {
        action_data = -1;
        data->current_item->custom_action(loop_ctx, game_ctx, &action_data);
    }
}

void render_options(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    static int32_t size_y = MAIN_WINDOW_Y, size_x = MAIN_WINDOW_X;
    options_data* data = (options_data*)loop_ctx->current_level->data;
    WINDOW* main_window = loop_ctx->main_window;
    int32_t start_x = 28;
    int32_t offset_x = 0;
    int32_t start_y = 2;
    int32_t offset_y = 0;

    mvwvline(main_window, 0, 26, '|', size_y);
    mvwhline(main_window, 18, 0, '-', size_x);

    if(data->language_switch.is_selected) {
        wattron(main_window, A_STANDOUT);
    }
    offset_x = calculate_padding_ds(&data->language_switch.name.text, 40);
    mvwprintw(main_window, start_y+offset_y, start_x,"                                        ");
    mvwprintw(main_window, start_y+offset_y, start_x+offset_x, "%s", data->language_switch.name.text.items);
    offset_y++;

    offset_x = calculate_padding_cstr(game_lang_str[game_ctx->language], 40);
    mvwprintw(main_window,  start_y+offset_y, start_x+offset_x, "%s", game_lang_str[game_ctx->language]);
    offset_y++;
    if(data->language_switch.is_selected) {
        wattroff(main_window, A_STANDOUT);
    }

    offset_y++;

    if(data->difficulty_switch.is_selected) {
        wattron(main_window, A_STANDOUT);
    }
    offset_x = calculate_padding_ds(&data->difficulty_switch.name.text, 40);
    mvwprintw(main_window, start_y+offset_y, start_x,"                                        ");
    mvwprintw(main_window, start_y+offset_y, start_x+offset_x, "%s", data->difficulty_switch.name.text.items);
    offset_y++;

    offset_x = calculate_padding_cstr(game_difficulty_str[game_ctx->difficulty], 40);
    mvwprintw(main_window,  start_y+offset_y, start_x+offset_x, "%s", game_difficulty_str[game_ctx->difficulty]);
    offset_y++;
    if(data->difficulty_switch.is_selected) {
        wattroff(main_window, A_STANDOUT);
    }

    offset_y++;

    if(data->mm_return_button.is_selected) {
        wattron(main_window, A_STANDOUT);
    }
    offset_x = calculate_padding_ds(&data->mm_return_button.name.text, 40);
    mvwprintw(main_window, start_y+offset_y, start_x,"                                        ");
    mvwprintw(main_window, start_y+offset_y, start_x+offset_x, "%s", data->mm_return_button.name.text.items);
    if(data->mm_return_button.is_selected) {
        wattroff(main_window, A_STANDOUT);
    }
}

void close_options(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx) {
    loop_ctx->current_level->is_initialized = false;
    options_data* data = (options_data*)loop_ctx->current_level->data;
    data->current_item->is_selected = false;

    free(loop_ctx->current_level->data);
}

#endif