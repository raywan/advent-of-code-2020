#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define ABS(x) ((x) > 0 ? (x) : (-1 * (x)))

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

static int8_t cube_offsets[][3] = {
    // just z offset
    {0,0,1}, {0,0,-1},

    // z = 0 layer
    {0,1,0}, {0,-1,0},
    {1,0,0}, {-1,0,0},
    {1,1,0}, {-1,1,0},
    {1,-1,0}, {-1,-1,0},

    // z = 1 layer
    {0,1,1}, {0,-1,1},
    {1,0,1}, {-1,0,1},
    {1,1,1}, {-1,1,1},
    {1,-1,1}, {-1,-1,1},

    // z = -1 layer
    {0,1,-1}, {0,-1,-1},
    {1,0,-1}, {-1,0,-1},
    {1,1,-1}, {-1,1,-1},
    {1,-1,-1}, {-1,-1,-1}
};

typedef struct Cell {
    char *state;
    int32_t x, y, z, w;
} Cell;

typedef struct CellArray {
    uint32_t capacity;
    uint32_t len;
    Cell *buf;
} CellArray;

typedef struct Change {
    int32_t x, y, z, w;
} Change;

#define POCKET_EXTENT 128
#define POCKET_ORIGIN_X (POCKET_EXTENT/2)
#define POCKET_ORIGIN_Y (POCKET_EXTENT/2)
#define POCKET_ORIGIN_Z (POCKET_EXTENT/2)
#define POCKET_ORIGIN_W (POCKET_EXTENT/2)
#define LX(v) ((v) - POCKET_ORIGIN_X)
#define LY(v) ((v) - POCKET_ORIGIN_Y)
#define LZ(v) ((v) - POCKET_ORIGIN_Z)

#define NUM_SIMULATION_ROUNDS 6

static char w_dim_pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT];
static char pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT];
static CellArray instantiated_cells;
static int num_cells = 0;

inline void insert_cell(CellArray *ca, Cell c) {
    if (ca->len + 1 > ca->capacity) {
        ca->capacity *= 2;
        ca->buf = realloc(ca->buf, sizeof(Cell) * ca->capacity);
    }
    ca->buf[ca->len++] = c;
}

uint32_t count_and_initialize_neighbors(Cell *c, CellArray *instantiated_cells) {
    uint32_t num_neighbors = 0; 
    int k = 0;
    for (int i = 0; i < 26; i++) {
        int nx = c->x + cube_offsets[i][0];
        int ny = c->y + cube_offsets[i][1];
        int nz = c->z + cube_offsets[i][2];
        if (pocket[ny][nx][nz] == 0) {
            pocket[ny][nx][nz] = '.';
            Cell new_cell = {
                .state = &(pocket[ny][nx][nz]),
                .x = nx,
                .y = ny,
                .z = nz
            };
            insert_cell(instantiated_cells, new_cell);
        }
        num_neighbors += (pocket[ny][nx][nz] == '#');
    }
    return num_neighbors;
}

void simulate_pocket_dimension(char pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT], Change *change_list, CellArray *instantiated_cells) {
    // simulate each cell that we have
    uint32_t cur_num_cells = instantiated_cells->len; 
    uint32_t num_changes = 0;
    for (int i = 0; i < cur_num_cells; i++) {
        Cell *c = &(instantiated_cells->buf[i]);
        uint32_t n = count_and_initialize_neighbors(c, instantiated_cells);
        int change = 0;
        if (((*c->state == '#') && !(n == 2 || n == 3)) ||
            ((*c->state == '.') && (n == 3))) {
            change_list[num_changes].x = c->x;
            change_list[num_changes].y = c->y;
            change_list[num_changes].z = c->z;
            num_changes++;
            change = 1;
        }
    }

    // Apply changes
    for (int i = 0; i < num_changes; i++) {
        int x = change_list[i].x;
        int y = change_list[i].y;
        int z = change_list[i].z;
        char next_val = (pocket[y][x][z] == '#') ? '.' : '#';
        pocket[y][x][z] = next_val;
    }
}

void print_slice(char pocket[POCKET_EXTENT][POCKET_EXTENT][POCKET_EXTENT], int z) {
    printf("slice z = %d\n", z);
    for (int i = 0; i < POCKET_EXTENT; i++) {
        int anything_printed = 0;
        for (int j = 0; j < POCKET_EXTENT; j++) {
            char c = pocket[i][j][z + POCKET_ORIGIN_Z];
            if (c) {
                printf("%c", c);
                anything_printed = 1;
            }
        }
        if (anything_printed) puts("");
    }
}

int main() {
    memset(pocket, 0, POCKET_EXTENT*POCKET_EXTENT*POCKET_EXTENT);

    int ox = POCKET_ORIGIN_X;
    int oy = POCKET_ORIGIN_Y;
    int oz = POCKET_ORIGIN_Z;

    CellArray instantiated_cells;
    instantiated_cells.capacity = 2048;
    instantiated_cells.len = 0;
    instantiated_cells.buf = (Cell *) malloc(sizeof(Cell) * instantiated_cells.capacity);

    Change change_list[512];

    // Parse initial state
    for (int i = 0; i < INPUT_H; i++) {
        for (int j = 0; j < INPUT_W; j++) {
            pocket[oy+i][ox+j][oz] = input[i][j];
            Cell new_cell = {
                .state = &(pocket[oy+i][ox+j][oz]),
                .x = ox+j,
                .y = oy+i,
                .z = oz
            };
            insert_cell(&instantiated_cells, new_cell);
        }
    }

    // Need the cells surround the initial state to be initialized as well 
    uint32_t cur_num_cells = instantiated_cells.len; 
    for (int i = 0; i < cur_num_cells; i++) {
        Cell *c = &(instantiated_cells.buf[i]);
        count_and_initialize_neighbors(c, &instantiated_cells);
    }

    for (int i = 0; i < NUM_SIMULATION_ROUNDS; i++) {
        simulate_pocket_dimension(pocket, change_list, &instantiated_cells);
    }
    
    uint32_t num_active_cells = 0;
    for (int i = 0; i < instantiated_cells.len; i++) {
        num_active_cells += (*(instantiated_cells.buf[i].state) == '#');
    }

    printf("PART 1: %u\n", num_active_cells);

    puts("done");

    return 0;
}
