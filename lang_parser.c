#include "lang_parser.h"
#include <stdbool.h>
#include <stdio.h>

bool load_text(text_vec_t* vec, language lang) {
    Mibs_Default_Allocator alloc = mibs_make_default_allocator();
    Mibs_Read_File_Result rfr;

    switch (lang) {
        case English: {
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

    // TODO check if I need to free the memory of string and rows at the same time
    Mibs_String_Array rows = mibs_split_cstr(&alloc, content.items, "\n-");
    mdefer { mibs_da_deinit(&alloc, &rows); }

    for (int32_t i = 0; i < rows.count; i++) {

        if(rows.items[i][0] == '#') {
            continue;
        }

        Mibs_String_Array line = mibs_split_cstr(&alloc, rows.items[i], "~");


        Mibs_DString id = mibs_ds_from_cstr(&alloc, line.items[0]);
        Mibs_DString text = mibs_ds_from_cstr(&alloc, line.items[1]);
        printf("ID: \"%s\"\n", id.items);
        printf("TEXT: \"%s\"\n", text.items);
        text_t t = {
            .id = id,
            .text = text
        };
        mibs_da_append(&alloc, vec, t);

        mibs_da_deinit(&alloc, &line);
    }

    return true;
}



text_finding_result find_text_by_id(text_vec_t* vec, Mibs_DString id) {
    text_finding_result res = {0};
    res.ok = false;
    for(int32_t i = 0; i < vec->count; i++) {
        if(strcmp(vec->items->id.items, id.items) == 0) {
            res.ok = true;
            res.value = vec->items[i];
            break;
        }
    }
    return res;
}