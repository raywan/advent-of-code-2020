/*
 * CREATED: 2020/12/05
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/5
*/

#include <stdio.h>
#include <stdint.h>

#define MAX_SEAT_CODE_LEN 11
#define MAX_INPUT_SEATS 771

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

inline uint32_t compute_seat_id(uint32_t row, uint32_t col) {
    return (row * 8) + col;
}

uint32_t process_seat_code(char seat[MAX_SEAT_CODE_LEN]) {
    uint32_t row_low = 0;
    uint32_t row_high = 127;
    uint32_t col_low = 0;
    uint32_t col_high = 7;
    uint32_t mid = 0;
    // printf("seat: %s => ", seat);
    for (int i = 0; i < MAX_SEAT_CODE_LEN; i++) {
        switch (seat[i]) {
            case 'F': {
                mid = (row_low + row_high)/2;
                row_high = mid;
            } break;
            case 'B': {
                // We need to round up for this one
                mid = (uint32_t) ((row_low + row_high)/2.0f + 0.5);
                row_low = mid;
            } break;
            case 'L': {
                mid = (col_low + col_high)/2;
                col_high = mid;
            } break;
            case 'R': {
                mid = (col_low + col_high)/2;
                col_low = mid;
            } break;
        }
    }

    return compute_seat_id(MIN(row_low, row_high), MAX(col_low, col_high));
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d05_input.txt", "r");

    char seat[MAX_SEAT_CODE_LEN];
    uint32_t max_seat_id = 0;
    uint32_t min_seat_id = UINT32_MAX;
    uint32_t my_seat_id = 0;
    for (int i = 0; fscanf(f, "%s", seat) != EOF; i++) {
        uint32_t seat_id = process_seat_code(seat);
        max_seat_id = MAX(max_seat_id, seat_id);
        min_seat_id = MIN(min_seat_id, seat_id);
        my_seat_id += seat_id;
    }

    // Compute the arithmetic sum from the min and max seat ids
    // Then subtract the sum of the seat ids to get your seat id
    uint32_t arithmetic_sum = ((max_seat_id-min_seat_id+1)/2) * (2 * min_seat_id + (max_seat_id - min_seat_id));
    my_seat_id = arithmetic_sum - my_seat_id;

    printf("PART 1: %u\n", max_seat_id);
    printf("PART 2: %d\n", my_seat_id);

    fclose(f);
    return 0;
}
