#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "../game.h"

#define perm_size 256

// https://rtouti.github.io/graphics/perlin-noise-algorithm
int32_t permutation[perm_size*2];

double dot_product(vector2_t v1, vector2_t v2) {
    return (v1.x * v2.x) + (v1.y * v2.y);
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

    // Shuffle perm table
	// for(int32_t e = perm_size-1; e > 0; e--) {
	// 	int32_t index = rand() % e;
	// 	int32_t temp = permutation[e];
	// 	permutation[e] = permutation[index];
	// 	permutation[index] = temp;
	// }

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
    int32_t px = (int32_t)floor(x) % 255;
	int32_t py = (int32_t)floor(y) % 255;

	double xf = x-floor(x);
	double yf = y-floor(y);

	vector2_t tr = vector2(xf-1.0, yf-1.0);
	vector2_t tl = vector2(xf, yf-1.0);
	vector2_t br = vector2(xf-1.0, yf);
	vector2_t bl = vector2(xf, yf);

   printf("\nmy point : %f %f\n", x, y);

    printf("vec tr: %f %f\n", tr.x, tr.y);
    printf("vec tl: %f %f\n", tl.x, tl.y);
    printf("vec bt: %f %f\n", br.x, br.y);
    printf("vec bl: %f %f\n", bl.x, bl.y);

    int32_t value_tr = permutation[permutation[px+1]+py+1];
	int32_t value_tl = permutation[permutation[px]+py+1];
	int32_t value_br = permutation[permutation[px+1]+py];
	int32_t value_bl = permutation[permutation[px]+py];

    double dot_tr = dot_product(tr, constant_vec(value_tr));
	double dot_tl = dot_product(tl, constant_vec(value_tl));
	double dot_br = dot_product(br, constant_vec(value_br));
	double dot_bl = dot_product(bl, constant_vec(value_bl));

    printf("\nDot product tr: %f\n", dot_tr);
    printf("\nDot product tl: %f\n", dot_tl);
    printf("\nDot product bt: %f\n", dot_br);
    printf("\nDot product bl: %f\n", dot_bl);

    double u = Fade(xf);
	double v = Fade(yf);
	
	return Lerp(u,
		Lerp(v, dot_bl, value_tl),
		Lerp(v, dot_br, dot_tr)
	);
}

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
            double n = noise2d(x*0.15, y*0.15);
            //n *= 100;

            //map->terrain[y][x].height = (int32_t)round(n);
            map->terrain[y][x].height = n;
            map->terrain[y][x].passable = true;
        }
    }
}

void deinit_map(map_data_t* map) {
    for(int32_t i = 0; i < map->size_y; i++) {
        free(map->terrain[i]);
    }
    free(map->terrain);
}


