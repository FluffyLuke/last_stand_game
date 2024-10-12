#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

double distance(point_t point_a, point_t point_b) {
	return sqrt(pow(point_b.x - point_a.x, 2)+pow(point_b.y-point_a.y, 2));
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
			n *= 32;
			n -= 12;

			map->terrain[y][x].passable = true;
			if(n <= 0.5) {
				n = 0;
				map->terrain[y][x].passable = false;
			} else if(n >= 9.5) {
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


unit_t* find_closest_unit(map_data_t* map_data, uint32_t lenght, point_t point) {
	unit_t* closest_unit = NULL;
	for(int32_t iy = 0; iy < lenght*2+1; iy++) {
		for(int32_t ix = 0; ix < lenght*2+1; ix++) {
			point_t temp_point = point;
			temp_point.x += (-lenght)+ix;
			temp_point.y += (-lenght)+iy;
			map_finding_result rc = find_on_map(map_data, temp_point);

			if(rc.unit == NULL) continue;
			if(compare_points(rc.unit->position, point)) continue;
			if(closest_unit == NULL) {
				closest_unit = rc.unit;
			}
			if(distance(closest_unit->position, rc.unit->position)
				> distance(point, rc.unit->position))
				closest_unit = rc.unit;
		}
	}
	return closest_unit;
}

typedef struct node_t {
	struct node_t* parent_node;
	point_t point;
	double f_cost; // Total cost -> g_cost + h_cost
	double g_cost; // How far from starting point
	double h_cost; // How far from end point
} node_t;

typedef Mibs_Da(node_t*) nodes;

// Only calculates f and g costs
// h cost is expected to be set before calling this function
void calculate_cost(node_t* p_node, node_t* n_node) {
	if(n_node->point.x > p_node->point.x && n_node->point.y > p_node->point.y) {
		// Down-right; 
		n_node->g_cost = sqrt(2) + p_node->g_cost;
		n_node->f_cost = n_node->g_cost + n_node->h_cost;
	} else if (n_node->point.x == p_node->point.x && n_node->point.y > p_node->point.y) {
		// Down; 
		n_node->g_cost = 1+p_node->g_cost;
		n_node->f_cost = n_node->g_cost + n_node->h_cost;
	} else if (n_node->point.x < p_node->point.x && n_node->point.y > p_node->point.y) {
		// Down-left; 
		n_node->g_cost = sqrt(2) + p_node->g_cost;
		n_node->f_cost = n_node->g_cost + n_node->h_cost;
	} else if (n_node->point.x < p_node->point.x && n_node->point.y == p_node->point.y) {
		// Left; 
		n_node->g_cost = 1+p_node->g_cost;
		n_node->f_cost = n_node->g_cost + n_node->h_cost;
	} else if (n_node->point.x < p_node->point.x && n_node->point.y < p_node->point.y) {
		// Up-left; 
		n_node->g_cost = sqrt(2) + p_node->g_cost;
		n_node->f_cost = n_node->g_cost + n_node->h_cost;
	} else if (n_node->point.x == p_node->point.x && n_node->point.y < p_node->point.y) {
		// Up; 
		n_node->g_cost = 1+p_node->g_cost;
		n_node->f_cost = n_node->g_cost + n_node->h_cost;
	} else if (n_node->point.x > p_node->point.x && n_node->point.y < p_node->point.y) {
		// Up-right; 
		n_node->g_cost = sqrt(2) + p_node->g_cost;
		n_node->f_cost = n_node->g_cost + n_node->h_cost;
	} else if (n_node->point.x > p_node->point.x && n_node->point.y == p_node->point.y) {
		// Right; 
		n_node->g_cost = 1+p_node->g_cost;
		n_node->f_cost = n_node->g_cost + n_node->h_cost;
	}
}

// https://www.youtube.com/watch?v=-L-WgKMFuhE
points find_path(game_ctx_t* game_ctx, map_data_t* map, point_t start, point_t end) {
	nodes open_nodes = {0};
	nodes closed_nodes = {0};

	// Starting node
	node_t* starting_node = (node_t*)malloc(sizeof(node_t));
	starting_node->parent_node = NULL;
	starting_node->point = start;
	starting_node->h_cost = 1;
	starting_node->g_cost = 1;
	starting_node->f_cost = 1;
	mibs_da_append(&game_ctx->alloc, &open_nodes, starting_node);

	node_t* end_node = NULL;
	int i = 0;
	
	//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Start node: y%d x%d\n\n", start.y, start.x);
	//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "End node: y%d x%d\n\n", end.y, end.x);

	while(true) {
		i++;
		//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Step: %d\n", i);
		node_t* c_node = NULL; // Current node
		// Check if there are still posible paths
		if(open_nodes.count > 0) {
			//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Still more possible nodes\n");
			c_node = open_nodes.items[0];
		} else {
			//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Path not found\n");
			break; // Path not found
		}

		// Get current node from open nodes
		int32_t current_node_index = 0;
		c_node = open_nodes.items[0];
		//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Checking open nodes\n\n");
		for(int32_t i = 0; i < open_nodes.count; i++) {
			//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "c_node (%f) vs next node in line (%f)\n", c_node->f_cost, open_nodes.items[i]->f_cost);
			if(open_nodes.items[i]->f_cost < c_node->f_cost) {
				c_node = open_nodes.items[i];
				current_node_index = i;
				//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Got current node\n");
			}
		}

		// Remove current node from open nodes and add it to closed nodes
		mibs_da_remove(&open_nodes, current_node_index);
		mibs_da_append(&game_ctx->alloc, &closed_nodes, c_node);

		//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Current node: y%d x%d\n\n", c_node->point.y, c_node->point.x);

		// Reached end
		if(compare_points(c_node->point, end)) {
			//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Path found\n");
			end_node = c_node;
			break; // Path found
		}

		// Check all neighbours (evil double loop in while loop)
		for(int32_t iy = -1; iy < 2; iy++) {
			for(int32_t ix = -1; ix < 2; ix++) {
				// Not a neighbour
				//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "\n\n");
				//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neightbour: iy%d ix%d\n", iy, ix);
				if(ix == 0 && iy == 0) continue;
				// Point of current neighbour
				point_t n_point = {
					.x = ix + c_node->point.x,
					.y = iy + c_node->point.y
				};

				//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Coords: y%d x%d\n", n_point.y, n_point.x);
				map_finding_result result = find_on_map(map, n_point);
				
				// Check if terrain is traversable
				bool building = result.building != NULL;
				bool unit = result.unit != NULL;
				bool terrain = result.terrain == NULL;
				//bool passable = !result.terrain->passable;
				if(building || unit || terrain) {
					//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour occupied\n");
					continue;
				}
				//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour height: %d\n", map->terrain[n_point.y][n_point.x].height);
				//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour passable: %d\n", map->terrain[n_point.y][n_point.x].passable);
				if(result.terrain->passable == false) {
					//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour not passable\n");
					continue;
				}

				// Check if neighbour is in closed nodes
				bool continue_flag = false;
				for(int32_t i = 0; i < closed_nodes.count; i++) {
					if(compare_points(closed_nodes.items[i]->point, result.position)) {
						//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour in closed nodes\n");
						continue_flag = true;
						break;
					}
				}
				// Break if neighbour is in closed nodes
				if(continue_flag) continue;

				node_t* n_node = NULL; // neighbour node
				bool add_flag = true; // add neighbour to open nodes it is not there
				// Finally get the neighbour
				for(int32_t i = 0; i < open_nodes.count; i++) {
					if(compare_points(open_nodes.items[i]->point, n_point)) {
						n_node = open_nodes.items[i];
						add_flag = false;
						//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour found in open list: x%d y%d\n", n_node->point.x, n_node->point.y);
						break;
					}
				}

				if(n_node == NULL) {
					// Initialize neighbour node if not present in open list
					n_node = (node_t*)malloc(sizeof(node_t));
					n_node->parent_node = c_node;
					n_node->point = n_point;
					n_node->h_cost = distance(n_node->point, end);
					calculate_cost(c_node, n_node);
					//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour node created\n");
					//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour h cost = %f\n", n_node->h_cost);
					//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour g cost = %f\n", n_node->g_cost);
					//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour f cost = %f\n", n_node->f_cost);
					//mibs_da_append(&game_ctx->alloc, &open_nodes, n_node);
				} else if(n_node->f_cost < c_node->f_cost) {
					// Change parent and f value if current_node is closer to the exit
					//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Recalculating neighbour costs\n");
					n_node->parent_node = c_node;
					calculate_cost(c_node, n_node);
				}

				if(add_flag == true) {
					//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Neighbour node added to open list\n");
					mibs_da_append(&game_ctx->alloc, &open_nodes, n_node);
				}
			}
		}
	}

	points path = {0};
	node_t* next_node = end_node;
	while(next_node != NULL) {
		//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Appending node %p to path\n", next_node);
		//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "Node position y%d x%d\n", next_node->point.y, next_node->point.x);
		mibs_da_append(&game_ctx->alloc, &path, next_node->point);
		//mibs_file_log(game_ctx->logs, MIBS_LL_DEBUG, "New next node %p\n", next_node);
		next_node = next_node->parent_node;
	}

	mibs_da_deinit(&game_ctx->alloc, &open_nodes);
	mibs_da_deinit(&game_ctx->alloc, &closed_nodes);

	return path;
}

