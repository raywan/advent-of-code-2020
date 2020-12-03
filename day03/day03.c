/*
 * CREATED: 2020/12/03
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/3
*/

#include <stdio.h>
#include <stdint.h>

#define MAX_INPUT_COL 31

inline int is_tree(char line[MAX_INPUT_COL], int idx) {
    return line[idx % MAX_INPUT_COL] == '#';
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d03_input.txt", "r");
    char line[MAX_INPUT_COL];

    int j = 0;
    uint64_t r1d1 = 0;
    uint64_t r3d1 = 0;
    uint64_t r5d1 = 0;
    uint64_t r7d1 = 0;
    uint64_t r1d2 = 0;
    for (int i = 0; fscanf(f, "%s", line) != EOF; i++) {
        r1d1 += is_tree(line, i);
        r3d1 += is_tree(line, i*3);
        r5d1 += is_tree(line, i*5);
        r7d1 += is_tree(line, i*7);
        r1d2 += (i % 2 == 0) && is_tree(line, j++);
    }

    uint64_t product = r1d1 * r3d1 * r5d1 * r7d1 * r1d2;

    printf("PART 1: %llu\n", r3d1);
    printf("R1D1: %llu\n", r1d1);
    printf("R3D1: %llu\n", r3d1);
    printf("R5D1: %llu\n", r5d1);
    printf("R7D1: %llu\n", r7d1);
    printf("R1D2: %llu\n", r1d2);
    printf("PART 2: %llu\n", product);

    fclose(f);
    return 0;
}
