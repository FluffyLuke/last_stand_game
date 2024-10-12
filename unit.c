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

    points path = find_path(game_ctx, unit->map_data, unit->position, data->map_task.result.position);
    
    map_finding_result result = find_on_map(unit->map_data, path.items[0]);
    mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "\n\n\n Selected Coords: y%d x%d\nPassable: %d, height: %d\n\n", 
    result.position.y, result.position.x, result.terrain->passable, result.terrain->height);

    mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Amount of steps: %zu\n", path.count);

    for(int32_t i = path.count-1; i >= 0; i--) {
        mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "New step number %d\n", i);
        move_point_t new_move_point;
        new_move_point.point = path.items[i];
        init_tick_timer(&new_move_point.move_timer, 100);
        mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Appending point to path\n");
		mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Node point y%d x%d\n", new_move_point.point.y, new_move_point.point.x);
        mibs_da_append(&game_ctx->alloc, &unit->path, new_move_point);
    }

    mibs_da_deinit(&game_ctx->alloc, &path);
    
    data->finished = true;
}

typedef struct {
    map_data_t* map_data;
    unit_t* target;
    tick_timer_t attack_timer;
} reacon_data;

void reacon_deinit(game_ctx_t* game_ctx, unit_t* unit) { 
    free(unit->data);
    for(int32_t i = 0; i < unit->actions.count; i++) {
        free(unit->actions.items[0]);
    }
    mibs_da_deinit(&game_ctx->alloc, &unit->actions);
    mibs_da_deinit(&game_ctx->alloc, &unit->path);
}

void reacon_logic(game_ctx_t* game_ctx, loop_ctx_t* loop_ctx, unit_t* unit) {
    reacon_data* unit_data = (reacon_data*)unit->data;

    if(unit->path.count > 0) {
        if(add_ticks_to_timer(&unit->path.items[0].move_timer, loop_ctx->delta_ticks)) {
            mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Unit new position: y%d x%d", unit->position.y, unit->position.x);
            unit->position = unit->path.items[0].point;
            mibs_da_remove(&unit->path, 0);
            mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Amount of steps left: %zu\n", unit->path.count);
        }
    }

    if(unit_data->target == NULL) {
        unit->fighting = false;
        unit_data->target = find_closest_unit(unit_data->map_data, unit->attack_range, unit->position);
        init_tick_timer(&unit_data->attack_timer, 50);
    } else if(unit->attack_range > distance(unit->position, unit_data->target->position)) {
        unit_data->target = NULL;
        mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "B\n");
        unit->fighting = false;
    } else {
        mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "p %p\n", unit_data->target);
        mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "he %d\n", unit_data->target->health);
        unit->fighting = true;
        if(add_ticks_to_timer(&unit_data->attack_timer, loop_ctx->delta_ticks)) {
            unit_data->target->health--;
            refresh_tick_timer(&unit_data->attack_timer);
            if(unit_data->target->health <= 0) {
                unit_data->target = NULL;
                unit->fighting = false;
            }
        }
    }
}

void create_reacon(game_ctx_t* game_ctx, map_data_t* map_data, unit_t* unit) {
    memset(unit, 0, sizeof(unit_t));

    unit->name = get_text_by_id(&game_ctx->game_text, "rc_name").value;
    unit->desciption = get_text_by_id(&game_ctx->game_text, "rc_desc").value;
    unit->symbol = 'r';
    unit->type = UT_REACON;
    unit->enemy = false;
    unit->position.x = 0;
    unit->position.y = 0;

    unit->data = (reacon_data*)malloc(sizeof(reacon_data));
    memset(unit->data, 0, sizeof(reacon_data));
    ((reacon_data*)unit->data)->target = NULL;
    ((reacon_data*)unit->data)->map_data = map_data;
    unit->map_data = map_data;

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

void crawler_deinit(game_ctx_t* game_ctx, unit_t* unit) { 
    //free(unit->data);
    // for(int32_t i = 0; i < unit->actions.count; i++) {
    //     free(unit->actions.items[0]);
    // }
    // mibs_da_deinit(&game_ctx->alloc, &unit->actions);
    // mibs_da_deinit(&game_ctx->alloc, &unit->path);
}

void crawler_logic(game_ctx_t* game_ctx, loop_ctx_t* loop_ctx, unit_t* unit) {
    // if(unit->path.count > 0) {
    //     if(add_ticks_to_timer(&unit->path.items[0].move_timer, loop_ctx->delta_ticks)) {
    //         mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Unit new position: y%d x%d", unit->position.y, unit->position.x);
    //         unit->position = unit->path.items[0].point;
    //         mibs_da_remove(&unit->path, 0);
    //         mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Amount of steps left: %zu\n", unit->path.count);
    //     }
    // }
}

void create_crawler(game_ctx_t* game_ctx, map_data_t* map_data, unit_t* unit) {
    memset(unit, 0, sizeof(unit_t));

    unit->name = get_text_by_id(&game_ctx->game_text, "cr_name").value;
    unit->desciption = get_text_by_id(&game_ctx->game_text, "cr_desc").value;
    unit->symbol = 'c';
    unit->type = UT_CRAWLER;
    unit->enemy = true;
    unit->position.x = 0;
    unit->position.y = 0;

    unit->data = NULL;
    unit->map_data = map_data;

    unit->health = 3;
    unit->armor = 0;
    unit->size = 1;
    unit->sight_range = 5;
    unit->attack_range = 1;

    unit->run_logic = crawler_logic;
    unit->deinit = crawler_deinit;
    unit->game_ctx = game_ctx;
}

void create_unit(game_ctx_t* game_ctx, map_data_t* map_data, unit_t* unit, Unit_Type type) {
    switch(type) {
        case UT_REACON: { create_reacon(game_ctx, map_data, unit); break; }
        case UT_CRAWLER: { create_crawler(game_ctx, map_data, unit); break; }
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

