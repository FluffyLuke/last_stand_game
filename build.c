#define MIBS_IMPL
#include "mibs.h"

Mibs_Default_Allocator alloc = mibs_make_default_allocator();

bool build_main(void)
{
    Mibs_Cmd cmd = {0};
    mdefer { mibs_da_deinit(&alloc, &cmd); }


    #define OUT "./target/game"
    #define SOURCES "main.c", "game.c", "lang_parser.c"
    if(!mibs_path_exists("target")) {
        if(!mibs_create_folder_rec("target")) {
            mibs_log(MIBS_LL_INFO, "Cannot create target dir!\n");
            return false;
        }
    }

    mibs_cmd_append(&alloc, &cmd, "gcc", "-o", OUT, SOURCES, "-lncurses");

    return mibs_run_cmd(&alloc, &cmd, MIBS_CMD_SYNC, NULL).ok;
}

int main(int argc, char ** argv)
{
    mibs_rebuild(&alloc, argc, argv);

    if (!build_main()) return 1;

    return 0;
}