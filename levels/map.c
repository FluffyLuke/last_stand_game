#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../game.h"

#define perm_size 256

// https://rtouti.github.io/graphics/perlin-noise-algorithm
int32_t permutation[perm_size*2];

double dot_product(vector2_t v1, vector2_t v2) {
    return (v1.x * v2.x) + (v1.y * v2.y);
}

bool compare_points(point_t point_a, point_t point_b) {
	return (point_a.x == point_b.x && point_a.y == point_b.y);
}

vector2_t constant_vec(int32_t v) {
	// v is the value from the permutation table
	int32_t h = v % 3;
    vector2_t vec;
	if(h == 0) {
		vec.x = 1.0; vec.y = 1.0;
        return vec;
    } else if(h == 1) {
        vec.x = -1.0; vec.y = 1.0;
        return vec;
    } else if(h == 2) {
		vec.x = -1.0; vec.y = -1.0;
        return vec;
    } else {
		vec.x = 1.0; vec.y = -1.0;
        return vec;
    }
}

void create_permutation() {
    memset(permutation, 0, perm_size*2);
    for(int32_t i = 0; i < perm_size; i++) {
		permutation[i] = i;
	}

    for(int32_t e = perm_size-1; e > 0; e--) {
		float r = (float)rand() / (float)RAND_MAX;
        int64_t index = rintf(r*(e-1));
		int64_t temp = permutation[e];
		permutation[e] = permutation[index];
		permutation[index] = temp;
	}
	
	for(int32_t i = perm_size; i < perm_size*2; i++) {
		permutation[i] = permutation[i-perm_size];
	}
}

// Used to smooth out the noise
double Fade(double t) {
	return ((6*t - 15)*t + 10)*t*t*t;
}

double Lerp(double t, double a1, double a2) {
	return a1 + t*(a2-a1);
}

double noise2d(double x, double y) {
	//printf("\n=======\n");
    int32_t px = (int32_t)floor(x) % 255;
	int32_t py = (int32_t)floor(y) % 255;

	double xf = x-floor(x);
	double yf = y-floor(y);

    //printf("Fade xf: %f\n", xf);
    //printf("Fade yf: %f\n\n", yf);

	vector2_t tr = vector2(xf-1.0, yf-1.0);
	vector2_t tl = vector2(xf, yf-1.0);
	vector2_t br = vector2(xf-1.0, yf);
	vector2_t bl = vector2(xf, yf);

	//printf("tr vec: %fx %fy \n", tr.x, tr.y);
	//printf("tl vec: %fx %fy \n", tl.x, tl.y);
	//printf("br vec: %fx %fy \n", br.x, br.y);
	//printf("bl vec: %fx %fy \n", bl.x, bl.y);

    int32_t value_tr = permutation[permutation[px+1]+py+1];
	int32_t value_tl = permutation[permutation[px]+py+1];
	int32_t value_br = permutation[permutation[px+1]+py];
	int32_t value_bl = permutation[permutation[px]+py];

    double dot_tr = dot_product(tr, constant_vec(value_tr));
	double dot_tl = dot_product(tl, constant_vec(value_tl));
	double dot_br = dot_product(br, constant_vec(value_br));
	double dot_bl = dot_product(bl, constant_vec(value_bl));

	//printf("\n\ndp tr %f \n", dot_tr);
	//printf("dp tl %f \n", dot_tl);
	//printf("dp br %f \n", dot_br);
	//printf("dp bl %f \n", dot_bl);

    double u = Fade(xf);
	double v = Fade(yf);
	
	return Lerp(u,
		Lerp(v, dot_bl, dot_tl),
		Lerp(v, dot_br, dot_tr)
	);
}

// TODO sometimes there is too much water and map can be split
void generate_map(map_data_t* map, int32_t size_y, int32_t size_x) {
    create_permutation();

    map->size_x = size_x;
    map->size_y = size_y;
	map->terrain = (terrain_t**)malloc(size_y * sizeof(terrain_t*));
    for(int32_t i = 0; i < size_y; i++) {
        map->terrain[i] = (terrain_t*)malloc(size_x * sizeof(terrain_t));
    }

    for(int32_t y = 0; y < size_y; y++) {
        for(int32_t x = 0; x < size_x; x++) {
            double n = noise2d((x+1)*0.05, (y+1)*0.05);
            n += 1.0;
		    n /= 2.0;
			n *= 31;
			n -= 9;

			map->terrain[y][x].passable = true;
			if(n < 0) {
				n = 0;
				map->terrain[y][x].passable = false;
			} else if(n > 10) {
				n = 10;
				map->terrain[y][x].passable = false;
			}

            map->terrain[y][x].height = (int32_t)round(n);
            //map->terrain[y][x].height = n;
        }
		//printf("\n");
    }
}

void init_map(map_data_t* map) {
	map->units = (units_vec*)malloc(sizeof(units_vec));
	memset(map->units, 0, sizeof(units_vec));
	map->buildings = (buildings_vec*)malloc(sizeof(buildings_vec));
	memset(map->buildings, 0, sizeof(buildings_vec));

	map->terrain = NULL;
}

void deinit_map(map_data_t* map) {
	// If map was generated
    if(map->terrain != NULL) {
		for(int32_t i = 0; i < map->size_y; i++) {
			free(map->terrain[i]);
		}
		free(map->terrain);
	}

	free(map->units);
	free(map->buildings);
}

void add_unit(map_data_t* map, unit_t* unit) {
	mibs_da_append(&mibs_make_default_allocator(), map->units, unit);
}

void remove_unit(map_data_t* map, unit_t* unit) {
	for(int32_t i = 0; i < map->units->count; i++) {
		if(map->units->items[i] == unit) mibs_da_remove(map->units, i);
	}
}

map_finding_result find_on_map(map_data_t* map, point_t position) {
	map_finding_result result;
	result.building = NULL;
	result.terrain = NULL;
	result.unit = NULL;

	result.position = position;
	if(position.x >= 0 && position.y >= 0 && position.x < map->size_x && position.y < map->size_y)
		result.terrain = &map->terrain[position.y][position.x];

	//mibs_log(MIBS_LL_DEBUG, "Marker position: y-%d, x-%d\n", position.y, position.x);
	for(int32_t i = 0; i < map->units->count; i++) {
		unit_t* unit = map->units->items[i];
		//mibs_log(MIBS_LL_DEBUG, "Unit position: y-%d, x-%d\n", unit->position.y, unit->position.x);
		if(compare_points(unit->position, position)) {
			result.unit = map->units->items[i];
			break;
		}
	}

	for(int32_t i = 0; i < map->buildings->count; i++) {
		if(compare_points(map->buildings->items[i]->position, position)) {
			result.building = map->buildings->items[i];
			break;
		}
	}

	return result;
}