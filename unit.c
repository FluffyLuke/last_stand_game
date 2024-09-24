#include "lang_parser.h"
#include "mibs.h"
#include "game.h"

void move_action(loop_ctx_t* loop_ctx, game_ctx_t* game_ctx, action_task_t* data) {

    if(data->map_task.finished == false) {
        return;
    }
    if(data->map_task.canceled == true) {
        data->finished = true;
    }

    unit_t* unit = (unit_t*)data->selectable;
    // unit->position = data->map_task.result.position;

    move_point_t new_move_point;
    new_move_point.point = data->map_task.result.position;
    init_tick_timer(&new_move_point.move_timer, 100);
    mibs_da_append(&game_ctx->alloc, &unit->path, new_move_point);
    
    data->finished = true;
}

void reacon_deinit(game_ctx_t* game_ctx, unit_t* unit) { 
    free(unit->data);
    for(int32_t i = 0; i < unit->actions.count; i++) {
        free(unit->actions.items[0]);
    }
    mibs_da_deinit(&game_ctx->alloc, &unit->actions);
    mibs_da_deinit(&game_ctx->alloc, &unit->path);
}

void reacon_logic(game_ctx_t* game_ctx, loop_ctx_t* loop_ctx, unit_t* unit) { 
    if(unit->path.count > 0) {
        if(add_ticks_to_timer(&unit->path.items[0].move_timer, loop_ctx->delta_ticks)) {
            unit->position = unit->path.items[0].point;
            mibs_da_remove(&unit->path, 0);
        }
    }
}

void create_reacon(game_ctx_t* game_ctx, unit_t* unit) {
    memset(unit, 0, sizeof(unit_t));

    unit->name = get_text_by_id(&game_ctx->game_text, "rc_name").value;
    unit->desciption = get_text_by_id(&game_ctx->game_text, "rc_desc").value;
    unit->symbol = 'r';
    unit->type = UT_REACON;
    unit->enemy = false;
    unit->position.x = 0;
    unit->position.y = 0;

    unit->data = NULL;

    unit->health = 3;
    unit->armor = 0;
    unit->size = 1;
    unit->sight_range = 20;
    unit->attack_range = 5;

    action_task_t* move = (action_task_t*)malloc(sizeof(action_task_t));
    memset(move, 0, sizeof(action_task_t));
    init_action_task(
        move,
        unit,
        NULL, 
        true,
        get_text_unit("move_unit"),
        move_action,
        get_text_unit("move_unit"),
        get_text_unit("move_unit_desc")
    );

    mibs_da_append(&game_ctx->alloc, &unit->actions, move);

    unit->run_logic = reacon_logic;
    unit->deinit = reacon_deinit;
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

void set_unit_position(unit_t* unit, point_t* new_position) {
    unit->position = *new_position;
}

void set_unit_positionyx(unit_t* unit, int32_t y, int32_t x) {
    unit->position.y = y;
    unit->position.x = x;
}

