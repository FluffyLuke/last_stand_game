#include <ncurses/curses.h>
#include <stdint.h>
#include <ncursesw/ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "game.h"
#include "lang_parser.h"
#include "mibs.h"

#define __LEVELS
#include "levels/main_menu.h"
#include "levels/options.h"
#include "levels/main_level.h"

// Input window
WINDOW* input_window;

// For centering stuff
int32_t calculate_padding_ds(Mibs_DString* text, uint32_t size_x) {
    return (size_x - text->count) / 2;
}

int32_t calculate_padding_cstr(const char* text, uint32_t size_x) {
    return (size_x - strlen(text)) / 2;
}

// Used when no action needs to be provided
void empty_action(WINDOW* main_window, level_t* level, game_ctx_t* game_ctx, void* data) {
    mibs_file_log(game_ctx->logs, MIBS_LL_ERROR, "Usage of empty action!\n");
}

void center_window(WINDOW* parentwin, WINDOW* subwin) {
    int32_t subwin_x, subwin_y;
    getmaxyx(subwin, subwin_y, subwin_x);

    int32_t parent_x, parent_y;
    getmaxyx(parentwin, parent_y, parent_x);

    int32_t move_y = (parent_y - subwin_y) / 2;
    int32_t move_x = (parent_x - subwin_x) / 2;

    mvderwin(subwin, move_y, move_x);
}

void exit_game(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data) {
    loop_ctx->current_level->should_close = true;
    loop_ctx->current_level->next_level_id = LI_NONE;
}

void window_to_small() {
    const char* text1 = "Window too small.";
    const char* text2 = "Minimum dimensions are 70x25.";
    int32_t y, x, cursor_x_pos;
    
    getmaxyx(stdscr, y, x);

    cursor_x_pos = (x-strlen(text1))/2;
    if(cursor_x_pos < 0) {
        cursor_x_pos = 0;
    }
    mvwprintw(stdscr, y/2, cursor_x_pos, "%s", text1);

    cursor_x_pos = (x-strlen(text2))/2;
    if(cursor_x_pos < 0) {
        cursor_x_pos = 0;
    }
    mvwprintw(stdscr, y/2+1, cursor_x_pos, "%s", text2);
}

void custom_item_init(
    game_ctx_t* game_ctx,
    custom_item_t* item,
    const char* name_id,
    const char* desc_id,
    void (*custom_action)(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, void* data)
) {
    item->is_selected = false;
    item->is_clicked = false;
    item->name = get_text_by_id(&game_ctx->game_text, name_id).value;
    item->description = get_text_by_id(&game_ctx->game_text, desc_id).value;
    item->custom_action = custom_action;
}

level_vec_t prepare_levels(Mibs_Default_Allocator* alloc) {
    level_vec_t vec = {0};

    level_t* main_menu = (level_t*)malloc(sizeof(level_t));
    main_menu->name = "Main menu";
    main_menu->id = LI_MAIN_MENU;
    main_menu->init = init_main_menu;
    main_menu->render = render_main_menu;
    main_menu->close = close_main_menu;
    main_menu->run_logic = run_main_menu_logic;
    main_menu->should_close = false;
    main_menu->is_initialized = false;
    mibs_da_append(alloc, &vec, main_menu);

    level_t* options = (level_t*)malloc(sizeof(level_t));
    options->name = "Options";
    options->id = LI_OPTIONS;
    options->init = init_options;
    options->render = render_options;
    options->close = close_options;
    options->run_logic = run_options_logic;
    options->should_close = false;
    options->is_initialized = false;
    mibs_da_append(alloc, &vec, options);

    level_t* main_level = (level_t*)malloc(sizeof(level_t));
    main_level->name = "Last Stand";
    main_level->id = LI_MAIN_LEVEL;
    main_level->init = init_main_level;
    main_level->render = render_main_level;
    main_level->close = close_main_level;
    main_level->run_logic = run_main_level_logic;
    main_level->should_close = false;
    main_level->is_initialized = false;
    mibs_da_append(alloc, &vec, main_level);

    return vec;
}

bool start_game_loop(game_ctx_t* game_ctx) {
    loop_ctx_t loop_ctx = {0};
    loop_ctx.current_level = game_ctx->levels.items[0];

    loop_ctx.main_window = subwin(stdscr, MAIN_WINDOW_Y, MAIN_WINDOW_X, 0, 0);
    input_window = newwin(0, 0, 0, 0);
    curs_set(0);
    keypad(input_window, TRUE);
    nodelay(input_window, TRUE);

    // Color pairs
    init_pair(CP_RED_TEXT, COLOR_RED, COLOR_BLACK);

    // Tick calculation initialization
    struct timeval tv;
    double current_tick = 0, previous_tick = 0;
    gettimeofday(&tv, NULL);
    previous_tick = (double)(tv.tv_sec * 1000000 + tv.tv_usec);


    mibs_file_log(game_ctx->logs, MIBS_LL_INFO, "Game loop ready\n");
    while(loop_ctx.current_level != NULL) {
        erase();
        werase(loop_ctx.main_window);

        getmaxyx(stdscr, game_ctx->size_y, game_ctx->size_x);

        // Check if window too small
        if(game_ctx->size_y < game_ctx->min_y || game_ctx->size_x < game_ctx->min_x) {
            window_to_small();
            refresh();
            continue;
        }

        // TODO fix the bug, where after "Window too small" main window takes all the available space?
        // This is just the quick fix
        wresize(loop_ctx.main_window, MAIN_WINDOW_Y, MAIN_WINDOW_X);

        // Get current delta time in microsecs and calculate number of ticks that passed
        gettimeofday(&tv, NULL);
        current_tick = (tv.tv_sec * 1000000 + tv.tv_usec);
        // Make it so 1 sec = 50 ticks; can be modified with game speed
        loop_ctx.delta_ticks = (double)(current_tick - previous_tick) / (double)20000;
        loop_ctx.delta_ticks *= game_ctx->speed;
        previous_tick = current_tick;

        // Check if size was changed;
        center_window(stdscr, loop_ctx.main_window);
        box(loop_ctx.main_window, 0, 0);

        level_t* current_lvl = loop_ctx.current_level; 

        wattron(loop_ctx.main_window, COLOR_PAIR(CP_RED_TEXT));
        mvwprintw(loop_ctx.main_window, 0, 3, "%s", loop_ctx.current_level->name);
        wattroff(loop_ctx.main_window, COLOR_PAIR(CP_RED_TEXT));

        // Init level (if necessary)
        if(!current_lvl->is_initialized)
            current_lvl->init(&loop_ctx, game_ctx);

        // Run level logic
        if(game_ctx->speed != GS_PAUSED)
            current_lvl->run_logic(&loop_ctx, game_ctx);

        // Render level
        current_lvl->render(&loop_ctx, game_ctx);

        // Render popup
        // Popup must pause the game on initialization, otherwise game will break
        if(game_ctx->popups.count != 0) {
            loop_ctx.current_popup = game_ctx->popups.items[0];

            if(!loop_ctx.current_popup->is_initialized)
                loop_ctx.current_popup->init(&loop_ctx, game_ctx);

            loop_ctx.current_popup->run_logic(&loop_ctx, game_ctx);

            loop_ctx.current_popup->render(&loop_ctx, game_ctx);

            if(loop_ctx.current_popup->should_close) {
                loop_ctx.current_popup->close(&loop_ctx, game_ctx);
                mibs_da_remove(&game_ctx->popups, 0);
                free(loop_ctx.current_popup);
                loop_ctx.current_popup = NULL;
            }
        }


        
        // Check if level had ended
        if(current_lvl->should_close) {
            mibs_file_log(game_ctx->logs, MIBS_LL_INFO, "Closing %s level\n", current_lvl->name);
            // Gracefully close it
            current_lvl->close(&loop_ctx, game_ctx);

            // Check what is next level
            // If none, close the app
            if(current_lvl->next_level_id == LI_NONE){
                mibs_file_log(game_ctx->logs, MIBS_LL_INFO, "Closing game\n");
                // Close popup if needed
                if(loop_ctx.current_popup != NULL) {
                    loop_ctx.current_popup->close(&loop_ctx, game_ctx);
                    free(loop_ctx.current_popup);
                }
                return true;
            }
            mibs_file_log(game_ctx->logs, MIBS_LL_INFO, "Next level id: %d\n", current_lvl->next_level_id);
            for(int32_t i = 0; i < game_ctx->levels.count; i++) {
                if(game_ctx->levels.items[i]->id == current_lvl->next_level_id) {
                    mibs_file_log(game_ctx->logs, MIBS_LL_INFO, "Starting \"%s\" level\n", game_ctx->levels.items[i]->name);
                    loop_ctx.current_level = game_ctx->levels.items[i];
                    break;
                }
            }
            // Check if level had changed
            if(loop_ctx.current_level == current_lvl) {
                mibs_file_log(game_ctx->logs, MIBS_LL_ERROR, "Cannot open new level!");
                return false;
            }
        }
        refresh();
    }

    return true;
}