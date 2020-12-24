#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../timer.h"

#define MAX_INPUT_LINE_LEN 64

#define NUM_SIMULATION_ROUNDS 100
#define GRID_W 256
#define GRID_H 256
#define ORIGIN_X (GRID_W/2)
#define ORIGIN_Y (GRID_H/2)

static int axial_dirs[6][2] = {
    {1,0}, {1,-1}, {0,-1},
    {-1,0}, {-1,1}, {0,1}
};

typedef enum HexDirection {
    HexDirection_E,
    HexDirection_NE,
    HexDirection_NW,
    HexDirection_W,
    HexDirection_SW,
    HexDirection_SE,
    HexDirection_COUNT
} HexDirection;

typedef uint8_t HexGrid[GRID_H][GRID_W];

typedef struct Change {
    int hx, hy;
} Change;

typedef struct ChangeList {
    uint32_t len;
    uint32_t capacity;
    Change *buf;
} ChangeList;

void insert_change(ChangeList *cl, int i, int j) {
    if (cl->len + 1 > cl->capacity) {
        cl->capacity *= 2;
        cl->buf = realloc(cl->buf, sizeof(Change) * cl->capacity);
    }
    cl->buf[cl->len++] = (Change) {i, j};
}

inline int run_instruction(char *line, HexGrid hex_grid) {
    int hx = ORIGIN_X; 
    int hy = ORIGIN_Y; 
    for (int i = 0; line[i] >= 'a' && line[i] <= 'z'; i++) {
        char dir_char = line[i];
        HexDirection dir;
        switch (dir_char) {
            case 'n': {
                char sub_dir_char = line[i+1];
                i++;
                if (sub_dir_char == 'e') {
                    dir = HexDirection_NE;
                } else if (sub_dir_char == 'w') {
                    dir = HexDirection_NW;
                }
            } break;
            case 's': {
                char sub_dir_char = line[i+1];
                i++;
                if (sub_dir_char == 'e') {
                    dir = HexDirection_SE;
                } else if (sub_dir_char == 'w') {
                    dir = HexDirection_SW;
                }
            } break;
            case 'e': {
                dir = HexDirection_E;
            } break;
            case 'w': {
                dir = HexDirection_W;
            } break;
        }

        // Move the current position based on the direction we've moved
        hx += axial_dirs[dir][0];
        hy += axial_dirs[dir][1];
    }

    // 0 = white, 1 = black
    int last_value = hex_grid[hx][hy];
    hex_grid[hx][hy] = !hex_grid[hx][hy];
    if (last_value == 0) {
        return 1;
    }

    return -1;
}

inline uint32_t get_num_black_hex_neighbors(HexGrid hex_grid, int hx, int hy) {
    uint32_t result = 0;
    for (HexDirection dir = 0; dir < HexDirection_COUNT; dir++){
        int new_hx = hx + axial_dirs[dir][0];
        int new_hy = hy + axial_dirs[dir][1];
        if ((new_hx >= 0 && new_hx < GRID_W) && (new_hy >= 0 && new_hy < GRID_H)) {
            result += hex_grid[new_hx][new_hy];
        }
    }
    return result;
}

uint32_t simulate_art_exhibit(HexGrid hex_grid) {
    uint32_t num_black_tiles = 0;
    ChangeList change_list;
    change_list.len = 0;
    change_list.capacity = 2048;
    change_list.buf = (Change *) malloc(sizeof(Change) * change_list.capacity);

    for (int r = 1; r <= NUM_SIMULATION_ROUNDS; r++) { 
        change_list.len = 0;
        for (int i = 0; i < GRID_H; i++) {
            for (int j = 0; j < GRID_W; j++) {
                uint32_t nn = get_num_black_hex_neighbors(hex_grid, i, j);
                if ((hex_grid[i][j] == 1 && (nn == 0 || nn > 2)) || 
                    (hex_grid[i][j] == 0 && nn == 2)) {
                    insert_change(&change_list, i, j);
                }
            }
        }

        // Flip the tiles that need to be flipped based on the simulation
        for (int c = 0; c < change_list.len; c++) {
            int hx = change_list.buf[c].hx;
            int hy = change_list.buf[c].hy;
            hex_grid[hx][hy] = !hex_grid[hx][hy];
        }

        num_black_tiles = 0;
        for (int i = 0; i < GRID_H; i++) {
            for (int j = 0; j < GRID_W; j++) {
                num_black_tiles += (hex_grid[i][j] == 1);
            }
        }
    }

    return num_black_tiles;
}

// https://www.redblobgames.com/grids/hexagons/
// Based on what I read above, I decided to use axial coordinates
// and preallocate a 2d array to represent a rectangular hex map. 
// The reference hex tile starts at the center of the grid. 
// The simulation is not the most efficient
// since I'm linearly iterating through the rectangular hex map.
// Based on the few rounds I printed out, the growth pattern 
// leaves a lot of dead areas. I would have rathered to try and maintain
// a set of potential tiles that need to be simulated per round, 
// which is something I did for the conway cubes puzzle.
//
// Part 1: 86.5us (-Od) / 45.0us (-O2)
// Part 2: 189.66ms (avg. 1.9ms/round) (-Od) / 35.285ms (avg. 0.35ms/round) (-O2)
int main() {
    FILE *f = fopen("d24_input.txt", "rb");
    int32_t num_initial_black_tiles = 0;
    char line[MAX_INPUT_LINE_LEN];
    HexGrid hex_grid;
    memset(hex_grid, 0, sizeof(uint8_t) * GRID_W * GRID_H);

    while (fscanf(f, "%s", line) != EOF) {
        num_initial_black_tiles += run_instruction(line, hex_grid);
    }

    uint32_t num_black_after_sim = simulate_art_exhibit(hex_grid);

    printf("PART 1: %d\n", num_initial_black_tiles);
    printf("PART 2: %u\n", num_black_after_sim);

    fclose(f);
    return 0;
}
