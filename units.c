#include "mibs.h"
#include "game.h"

void reacon_init(unit_t* unit) { }
void create_reacon(game_ctx_t* game_ctx, unit_t* unit) {
    unit->name = get_text_by_id(&game_ctx->game_text, "rc_name").value;
    unit->desciption = get_text_by_id(&game_ctx->game_text, "rc_desc").value;

    unit->type = UT_REACON;
    unit->enemy = false;
    unit->health = 3;
    unit->armor = 0;
    unit->size = 1;
    unit->sight_range = 20;
    unit->attack_range = 1;

    unit->deinit = reacon_init;
    unit->data = NULL;
    unit->game_ctx = game_ctx;
}

void create_unit(game_ctx_t* game_ctx, unit_t* unit, Unit_Type type) {
    switch(type) {
        case UT_REACON: { create_reacon(game_ctx, unit); break; }
        // case UT_RIFLEMAN: {

        //     break;
        // }
        // case UT_SNIPER: {

        //     break;
        // }

        default: {
            break;
        }
    }
}

