#include <stdint.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

#include "game.h"
#include "mibs.h"

void window_to_small() {
    const char* text1 = "Window too small.";
    const char* text2 = "Minimum dimensions are 70x25.";
    int32_t y, x, cursor_x_pos;
    
    getmaxyx(stdscr, y, x);
    curs_set(0);
    erase();

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

void run_main_level_logic(level_t* level, ctx_t* ctx) {

}

void render_main_level(level_t* level, ctx_t* ctx) {
    int32_t size_y, size_x, c_pos_y, c_pos_x;
    static WINDOW *main_window, *upper_panel, *left_pannel, *right_panel; 
    getmaxyx(stdscr, size_y, size_x);
    erase();
    curs_set(0);

    if(!level->is_initialized) {
        main_window = subwin(stdscr, 10, 10, 3, 3);
        wborder(main_window,'A','A','A','A','A','A','A','A');
        level->is_initialized = true;
    } else {
        wborder(main_window,'A','B','B','B','A','A','A','A');
    }

    //int32_t ch = getch();
    if(0 == KEY_BACKSPACE) {
        exit(-1);
    }
    mvwprintw(main_window, 1, 1, "nugger");
}

void close_main_level(level_t* level, ctx_t* ctx) {

}

level_vec_t prepare_levels(Mibs_Default_Allocator* alloc) {
    level_vec_t vec = {0};

    level_t* main_level = (level_t*)malloc(sizeof(level_t));
    main_level->name = "Main Level";
    main_level->id = MAIN_LEVEL_ID;
    main_level->render_level = render_main_level;
    main_level->close_level = close_main_level;
    main_level->run_logic = run_main_level_logic;
    main_level->should_close = false;
    main_level->is_initialized = false;
    mibs_da_append(alloc, &vec, main_level);

    return vec;
}



bool start_game_loop(ctx_t* ctx) {
    while(true) {
        getmaxyx(stdscr, ctx->size_y, ctx->size_x);

        // Check if window too small
        if(ctx->size_y < ctx->min_y || ctx->size_x < ctx->min_x) {
            window_to_small();
            refresh();
            continue;
        }


        level_t* current_lvl = ctx->current_level; 

        // Run level logic
        current_lvl->run_logic(current_lvl, ctx);

        // Render level
        current_lvl->render_level(current_lvl, ctx);
        
        // Check if level had ended
        if(current_lvl->should_close) {
            // Gracefully close it
            current_lvl->close_level(current_lvl, ctx);

            // Check what is next level
            // If none, close the app
            if(current_lvl->next_level_id == NONE){
                return true;
            }
            for(int32_t i; i < ctx->levels.count; i++) {
                if(ctx->levels.items[i]->id == current_lvl->next_level_id) {
                    ctx->current_level = ctx->levels.items[i];
                    break;
                }
            }
        }
        refresh();
    }

    return true;
}