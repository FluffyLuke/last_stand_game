#include "lang_parser.h"
#include "mibs.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void free_game_text(game_text_t* vec) {
    Mibs_Default_Allocator alloc = mibs_make_default_allocator();
    for(int32_t i = 0; i < vec->count; i++) {
        mibs_da_deinit(&alloc, &vec->items[i].id);
        mibs_da_deinit(&alloc, &vec->items[i].text);
        mibs_da_remove(vec, i);
    }
}

bool load_text(game_text_t* vec, Game_Language lang) {

    free_game_text(vec);

    Mibs_Default_Allocator alloc = mibs_make_default_allocator();
    Mibs_Read_File_Result rfr;

    switch (lang) {
        case GL_English: {
            rfr = mibs_read_file(&alloc, "./text/en.lang");
            break;
        }
        default: {
            rfr = mibs_read_file(&alloc, "./text/en.lang");
            break;
        }
    }

    Mibs_String_Builder content = rfr.value;
    if(!rfr.ok) {
        return false; 
    }
    mdefer { mibs_da_deinit(&alloc, &content); }

    Mibs_String_Array rows = mibs_split_cstr(&alloc, content.items, "\n-");
    mdefer { mibs_da_deinit(&alloc, &rows); }

    text_unit_t tnf = {
        .id = mibs_ds_from_cstr(&alloc, "TNF"),
        .text = mibs_ds_from_cstr(&alloc, "Text not found")
    };

    mibs_da_append(&alloc, vec, tnf);

    for (int32_t i = 0; i < rows.count; i++) {

        if(rows.items[i][0] == '#') {
            continue;
        }

        Mibs_String_Array line = mibs_split_cstr(&alloc, rows.items[i], "~");


        Mibs_DString id = mibs_ds_from_cstr(&alloc, line.items[0]);
        Mibs_DString text = mibs_ds_from_cstr(&alloc, line.items[1]);
        //printf("ID: \"%s\"\n", id.items);
        //printf("TEXT: \"%s\"\n", text.items);
        text_unit_t gt = {
            .id = id,
            .text = text
        };
        mibs_da_append(&alloc, vec, gt);

        mibs_da_deinit(&alloc, &line);
    }

    return true;
}

text_getting_result get_text_by_id(game_text_t* vec, const char* id) {
    Mibs_Default_Allocator alloc = mibs_make_default_allocator();
    text_getting_result res = {0};
    res.ok = false;
    // Text not found item;
    res.value = vec->items[0];

    if(id == NULL) return res;

    for(int32_t i = 1; i < vec->count; i++) {
        if(strcmp(vec->items[i].id.items, id) == 0) {
            res.ok = true;
            res.value = vec->items[i];
            break;
        }
    }
    return res;
}