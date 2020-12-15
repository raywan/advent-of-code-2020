/*
 * CREATED: 2020/12/11
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/11
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_INPUT_COLS 128
#define MAX_INPUT_LINES 128
#define PART_1_ANSWER 2270
#define PART_2_ANSWER 2042

#define SIMULATE_CELL_RULE_SIG(name) char name(char grid[MAX_INPUT_LINES][MAX_INPUT_COLS], int r, int c, uint32_t num_rows, uint32_t num_cols)
typedef SIMULATE_CELL_RULE_SIG(SimulateCellFunc);

typedef char Grid[MAX_INPUT_LINES][MAX_INPUT_COLS];

static int dirs[][2] = {
    {-1,0}, {1,0},
    {0,-1}, {0,1},
    {-1,1}, {1,1},
    {-1,-1}, {1,-1}
};

SIMULATE_CELL_RULE_SIG(adjacency_rule) {
    uint8_t num_occupied = 0;
    for (int i = 0 ; i < 8; i++) {
        int new_c = c + dirs[i][0];
        int new_r = r + dirs[i][1];
        num_occupied += ((new_c >= 0) && (new_c < num_cols) && (new_r < num_rows) && (new_r >= 0)) && 
                        grid[new_r][new_c] == '#';
    }

    switch (grid[r][c]) {
        case '#':
            return num_occupied >= 4 ? 'L' : '#';
        case 'L':
            return num_occupied == 0 ? '#' : 'L';
        default:
            return '.';
    }
}

SIMULATE_CELL_RULE_SIG(visibility_rule) {
    uint8_t num_visible_occupied = 0;
    for (int i = 0; i < 8; i++) {
        int len = 1;
        int new_c = c + (dirs[i][0] * len);
        int new_r = r + (dirs[i][1] * len);
        while ((new_c >= 0) && (new_c < num_cols) && (new_r < num_rows) && (new_r >= 0)) {
            if (grid[new_r][new_c] == 'L') break;
            else if (grid[new_r][new_c] == '#') {
                num_visible_occupied++;
                break;
            }

            len++;
            new_c = c + (dirs[i][0] * len);
            new_r = r + (dirs[i][1] * len);
        }
    }

    switch (grid[r][c]) {
        case '#':
            return num_visible_occupied >= 5 ? 'L' : '#';
        case 'L':
            return num_visible_occupied == 0 ? '#' : 'L';
        default:
            return '.';
    }
}

uint32_t run_simulation(char input_grid[MAX_INPUT_LINES][MAX_INPUT_COLS],
                        uint32_t num_rows, uint32_t num_cols, SimulateCellFunc *apply_sim_func) {
    // Surprisingly, ping ponging between two buffers performs worse than 
    // doing a memcpy each simulation step. I did not investigate this further.
 
    char result_grid[MAX_INPUT_LINES][MAX_INPUT_COLS];
    char work_grid[MAX_INPUT_LINES][MAX_INPUT_COLS];
    memcpy(result_grid, input_grid, MAX_INPUT_COLS * MAX_INPUT_LINES);

    uint32_t num_changes = 1;
    uint32_t num_occupied = 0;
    while (num_changes > 0) {
        num_changes = 0;
        num_occupied = 0;
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                if (result_grid[i][j] == '.') continue;
                work_grid[i][j] = apply_sim_func(result_grid, i, j, num_rows, num_cols);
                num_changes += (work_grid[i][j] != result_grid[i][j]);
                num_occupied += (work_grid[i][j] == '#'); 
            }
        }
        memcpy(result_grid, work_grid, MAX_INPUT_COLS * MAX_INPUT_LINES);
    }

    return num_occupied;
}

// Disappointing
// Part 1: 28944.5us (-Od) / 10999.0us (-O2)
// Part 2: 52601.0us (-Od) / 21037.7us (-O2)
int main() {
    FILE *f = fopen("d11_input.txt", "rb");

    // Parse input
    char input_grid[MAX_INPUT_LINES][MAX_INPUT_COLS];
    uint32_t num_rows = 0;
    for (num_rows = 0; fscanf(f, "%s", &input_grid[num_rows]) != EOF; num_rows++) {}
    uint32_t num_cols = strlen(input_grid[0]);

    uint32_t num_adjacent_occupied_seats_after_convergence = run_simulation(input_grid, num_rows, num_cols, adjacency_rule);
    uint32_t num_visible_occupied_seats_after_convergence = run_simulation(input_grid, num_rows, num_cols, visibility_rule);

    printf("PART 1: %d (expected %d)\n", num_adjacent_occupied_seats_after_convergence, PART_1_ANSWER);
    printf("PART 2: %d (expected %d)\n", num_visible_occupied_seats_after_convergence, PART_2_ANSWER);

    fclose(f);
    return 0;
}
