const int size_y = 15;
const int size_x = 15;

#include "game.h"
int main() {
    srand(time(NULL));
    map_data_t map;
    generate_map(&map, size_y, size_x);
    for(int iy = 0; iy < size_y; iy++) {
        for(int ix = 0; ix < size_x; ix++) {
            printf("%f ", map.terrain[iy][ix].height);
        }
        printf("\n");
    }
}