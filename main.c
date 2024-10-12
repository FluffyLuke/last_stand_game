#include <ncurses.h>
#include <locale.h>
#include <ncurses/curses.h>
#include <stdio.h>

#define MIBS_IMPL
#define __GAME_ASSETS

#include "mibs.h"
#include "lang_parser.h"
#include "game.h"

int main(int argc, char** argv) {
    srand(time(NULL));
    setlocale(LC_ALL, "");

    char game_path[1024] = {0};
    memcpy(game_path, argv[0], strlen(argv[0]));

    for(int32_t i = strlen(argv[0])-1; i >= 0; i--) {
        if(game_path[i] != '/' && game_path[i] != '\\') {
            game_path[i] = 0;
        } else {
            game_path[i] = 0;
            break;
        }
    }

    char logs_path[1024] = {0};
    memcpy(logs_path, game_path, strlen(game_path));
    sprintf(logs_path, "%s/%s", game_path, "logs.txt");
    FILE* logs = fopen(logs_path, "w");

    if(logs == NULL) {
        mibs_file_log(stderr, MIBS_LL_ERROR, "Cannot open logs file!\n");
    }

    game_ctx_t game_ctx = {0};
    memset(&game_ctx.game_text, 0, sizeof(game_text_t));
    game_ctx.alloc = mibs_make_default_allocator();
    game_ctx.logs = logs;
    game_ctx.levels = prepare_levels(&game_ctx.alloc);
    game_ctx.ticks = 0;
    game_ctx.speed = GS_NORMAL;
    game_ctx.min_x = MAIN_WINDOW_X;
    game_ctx.min_y = MAIN_WINDOW_Y;
    game_ctx.difficulty = GD_NORMAL;
    game_ctx.game_path = game_path;

    // // Load default text
    if(!load_text(game_ctx.game_path, &game_ctx.game_text, GL_English)) {
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

    // TODO changing color functionality is not working
    // for now use basic colors and pairs
    // if(!can_change_color()) {
    //     mibs_file_log(logs, MIBS_LL_ERROR, "Terminal does not support changing color!\n");
    //     mibs_file_log(stderr, MIBS_LL_ERROR, "Terminal does not support changing color!\n");
    //     mibs_file_log(stderr, MIBS_LL_ERROR, "Your terminal supports %d colors.\n", COLORS);
    //     fclose(logs);
    //     return -1;
    // }

    cbreak();
    noecho();

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