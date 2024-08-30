#include <ncurses.h>
#include <locale.h>
#include <stdio.h>

#define MIBS_IMPL
#define __GAME_ASSETS

#include "mibs.h"
#include "lang_parser.h"
#include "game.h"

const char * const game_difficulty_str[] = {
    [GD_EASY] = "Easy",
    [GD_NORMAL] = "Normal",
    [GD_HARD]  = "Hard"
};

const char * const game_lang_str[] = {
    [GL_English] = "English",
};


int main(int argc, char** argv) {
    srand(time(NULL));
    setlocale(LC_ALL, "");

    FILE* logs = fopen("logs.txt", "w");

    if(logs == NULL) {
        mibs_file_log(stderr, MIBS_LL_ERROR, "Cannot open logs file!\n");
    }

    game_ctx_t game_ctx = {0};
    game_ctx.alloc = mibs_make_default_allocator();
    game_ctx.logs = logs;
    game_ctx.levels = prepare_levels(&game_ctx.alloc);
    game_ctx.ticks = 0;
    game_ctx.speed = GS_NORMAL;
    game_ctx.min_x = MAIN_WINDOW_X;
    game_ctx.min_y = MAIN_WINDOW_Y;
    game_ctx.difficulty = GD_NORMAL;

    // Load default text
    if(!load_text(&game_ctx.game_text, GL_English)) {
        mibs_file_log(logs, MIBS_LL_ERROR, "Cannot read game text!\n");
        mibs_file_log(stderr, MIBS_LL_ERROR, "Cannot read game text!\n");
        fclose(logs);
        return -1;
    }

    printf("%s\n", get_text_by_id(&game_ctx.game_text, "mm_quit_button").value.text.items);

    initscr();

    // Start color
    if(!has_colors()) {
        mibs_file_log(logs, MIBS_LL_ERROR, "Terminal does not support color!\n");
        mibs_file_log(stderr, MIBS_LL_ERROR, "Terminal does not support color!\n");
        fclose(logs);
        return -1;
    }

    start_color();

    cbreak();
    noecho();
    

    //keypad(stdscr, TRUE);
    mibs_file_log(game_ctx.logs, MIBS_LL_INFO, "Starting game loop\n");
    bool result = start_game_loop(&game_ctx);
    endwin();
    fclose(logs);
    if(!result) {
        return -1;
    } else {
        return 0;
    }
}