#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define INPUT_W 8
#define INPUT_H 8

char input[INPUT_H][INPUT_W] = {
"#.#..###",
".#....##",
".###...#",
"..####..",
"....###.",
"##.#.#.#",
"..#..##.",
"#.....##"
};

// Generated
static int hypercube_offsets[][4] = {
    {-1, -1, -1, -1},
    {0, -1, -1, -1},
    {1, -1, -1, -1},
    {-1, 0, -1, -1},
    {0, 0, -1, -1},
    {1, 0, -1, -1},
    {-1, 1, -1, -1},
    {0, 1, -1, -1},
    {1, 1, -1, -1},
    {-1, -1, 0, -1},
    {0, -1, 0, -1},
    {1, -1, 0, -1},
    {-1, 0, 0, -1},
    {0, 0, 0, -1},
    {1, 0, 0, -1},
    {-1, 1, 0, -1},
    {0, 1, 0, -1},
    {1, 1, 0, -1},
    {-1, -1, 1, -1},
    {0, -1, 1, -1},
    {1, -1, 1, -1},
    {-1, 0, 1, -1},
    {0, 0, 1, -1},
    {1, 0, 1, -1},
    {-1, 1, 1, -1},
    {0, 1, 1, -1},
    {1, 1, 1, -1},
    {-1, -1, -1, 0},
    {0, -1, -1, 0},
    {1, -1, -1, 0},
    {-1, 0, -1, 0},
    {0, 0, -1, 0},
    {1, 0, -1, 0},
    {-1, 1, -1, 0},
    {0, 1, -1, 0},
    {1, 1, -1, 0},
    {-1, -1, 0, 0},
    {0, -1, 0, 0},
    {1, -1, 0, 0},
    {-1, 0, 0, 0},
    {1, 0, 0, 0},
    {-1, 1, 0, 0},
    {0, 1, 0, 0},
    {1, 1, 0, 0},
    {-1, -1, 1, 0},
    {0, -1, 1, 0},
    {1, -1, 1, 0},
    {-1, 0, 1, 0},
    {0, 0, 1, 0},
    {1, 0, 1, 0},
    {-1, 1, 1, 0},
    {0, 1, 1, 0},
    {1, 1, 1, 0},
    {-1, -1, -1, 1},
    {0, -1, -1, 1},
    {1, -1, -1, 1},
    {-1, 0, -1, 1},
    {0, 0, -1, 1},
    {1, 0, -1, 1},
    {-1, 1, -1, 1},
    {0, 1, -1, 1},
    {1, 1, -1, 1},
    {-1, -1, 0, 1},
    {0, -1, 0, 1},
    {1, -1, 0, 1},
    {-1, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
    {-1, 1, 0, 1},
    {0, 1, 0, 1},
    {1, 1, 0, 1},
    {-1, -1, 1, 1},
    {0, -1, 1, 1},
    {1, -1, 1, 1},
    {-1, 0, 1, 1},
    {0, 0, 1, 1},
    {1, 0, 1, 1},
    {-1, 1, 1, 1},
    {0, 1, 1, 1},
    {1, 1, 1, 1}
};

#define POCKET_EXTENT 128
#define POCKET_ORIGIN_X (POCKET_EXTENT/2)
#define POCKET_ORIGIN_Y (POCKET_EXTENT/2)
#define POCKET_ORIGIN_Z (POCKET_EXTENT/2)
#define POCKET_ORIGIN_W (POCKET_EXTENT/2)

#define NUM_SIMULATION_ROUNDS 6

// w, z, y, x
static char pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT];
static char tmp_pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT];

uint32_t count_neighbors(char pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT], int x, int y, int z, int w) {
    uint32_t result = 0;
    for (int i = 0; i < 80; i++) {
        int nx = x + hypercube_offsets[i][0];
        int ny = y + hypercube_offsets[i][1];
        int nz = w + hypercube_offsets[i][2];
        int nw = z + hypercube_offsets[i][3];
        if ((nx >= 0 && nx < POCKET_EXTENT) &&
            (ny >= 0 && ny < POCKET_EXTENT) &&
            (nz >= 0 && nz < POCKET_EXTENT) &&
            (nw >= 0 && nw < POCKET_EXTENT)) {
            result += (pocket[nw][nz][ny][nx] == '#');
        }
    }
    return result;
}

void simulate_pocket_dimension(char pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT], 
                               char tmp_pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT]) {
    memcpy(tmp_pocket, pocket, POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT);
    for (int w = 0; w < POCKET_EXTENT; w++) {
        for (int z = 0; z < POCKET_EXTENT; z++) {
            for (int y = 0; y < POCKET_EXTENT; y++) {
                for (int x = 0; x < POCKET_EXTENT; x++) {
                    uint32_t n = count_neighbors(pocket, x, y, z, w);
                    char val = pocket[w][z][y][x];
                    if (((val == '#') && !(n == 2 || n == 3)) ||
                        ((val == 0) && (n == 3))) {
                        char next_val = val == '#' ? 0 : '#';
                        tmp_pocket[w][z][y][x] = next_val;
                    } else {
                        tmp_pocket[w][z][y][x] = val;
                    }
                }
            }
        }
    }
    memcpy(pocket, tmp_pocket, POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT);
}

uint32_t count_active_cells(char pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT]) {
    uint32_t result = 0;
    for (int w = 0; w < POCKET_EXTENT; w++) {
        for (int z = 0; z < POCKET_EXTENT; z++) {
            for (int y = 0; y < POCKET_EXTENT; y++) {
                for (int x = 0; x < POCKET_EXTENT; x++) {
                    result += (pocket[w][z][y][x] == '#');
                }
            }
        }
    }
    return result;
}

int main() {
    memset(pocket, 0, POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT);
    memset(tmp_pocket, 0, POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT);

    int ox = POCKET_ORIGIN_X;
    int oy = POCKET_ORIGIN_Y;
    int oz = POCKET_ORIGIN_Z;
    int ow = POCKET_ORIGIN_W;

    // Parse initial state
    for (int i = 0; i < INPUT_H; i++) {
        for (int j = 0; j < INPUT_W; j++) {
            if (input[i][j] == '#') {
                pocket[ow][oz][oy+i][ox+j] = input[i][j];
            }
        }
    }

    for (int i = 0; i < NUM_SIMULATION_ROUNDS; i++) {
        printf("SIMULATION ROUND: %d\n", i+1);
        simulate_pocket_dimension(pocket, tmp_pocket);
    }
    
    uint32_t num_active_cells = count_active_cells(pocket);

    printf("PART 2: %u\n", num_active_cells);

    return 0;
}
