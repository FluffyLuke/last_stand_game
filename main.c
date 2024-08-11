#include <ncurses.h>

#define MIBS_IMPL

#include "mibs.h"
#include "lang_parser.h"
#include "game.h"

int main(int argc, char** argv) {
    ctx_t ctx = {0};
    ctx.alloc = mibs_make_default_allocator();
    level_vec_t levels = prepare_levels(&ctx.alloc);
    ctx.current_level = levels.items[0];
    ctx.ticks = 0;
    ctx.is_paused = false;
    ctx.min_x = 70;
    ctx.min_y = 25;
    
    // Load default text
    if(!load_text(&ctx.game_text, English)) {
        printf("Cannot load game text!\n");
        return -1;
    }


    initscr();
    cbreak();
    //keypad(stdscr, TRUE);
    bool result = start_game_loop(&ctx);
    endwin();
    if(!result) {
        return -1;
    } else {
        return 0;
    }
}