/*
 * CREATED: 2020/12/15
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/15
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PART_1_NUM_ROUNDS 2020
#define PART_2_NUM_ROUNDS 30000000

#define PART_1_ANSWER 468
#define PART_2_ANSWER 1801753

typedef struct History {
    int has_been_spoken;
    uint32_t last_spoken_idx;
    uint32_t last_last_spoken_idx;
} History;

uint32_t run_memory_game(uint32_t *input, uint32_t input_len, uint32_t num_rounds, History *age_arr) {
    memset(age_arr, 0, sizeof(History) * PART_2_NUM_ROUNDS);

    for (int i = 0; i < input_len; i++) {
        age_arr[input[i]].has_been_spoken = 1;
        age_arr[input[i]].last_spoken_idx = i+1;
        age_arr[input[i]].last_last_spoken_idx = i+1;
    }

    uint32_t last_number_spoken = input[input_len-1]; // the (i-1)th number
    for (int i = input_len+1; i <= num_rounds; i++) {
        if (age_arr[last_number_spoken].has_been_spoken == 1) {
            last_number_spoken = 0;
        } else if (age_arr[last_number_spoken].has_been_spoken > 1) {
            last_number_spoken = (i-1) - age_arr[last_number_spoken].last_last_spoken_idx;
        }
        age_arr[last_number_spoken].has_been_spoken += 1;
        age_arr[last_number_spoken].last_last_spoken_idx = age_arr[last_number_spoken].last_spoken_idx;
        age_arr[last_number_spoken].last_spoken_idx = i;
    }

    return last_number_spoken;
}

// Bruteforcing...I don't quite see the pattern yet
// Part 1: 15.3us (-Od) / 9.9us (-O2)
// Part 2: 826.2ms (-Od) / 740.8ms (-O2)
// Trying to implement and use a simple hash table increased time by ~2x 
// likely because it's not as cache friendly. 
// Not that the linear array is much better in that regard...
// Since after printing out the full part 2 sequence, the access pattern looks wild
int main() {
    History *age_arr = (History *) malloc(sizeof(History) * PART_2_NUM_ROUNDS);

    uint32_t input[] = {6,19,0,5,7,13,1};
    uint32_t input_len = sizeof(input)/sizeof(uint32_t);

    uint32_t p1_answer = run_memory_game(input, input_len, PART_1_NUM_ROUNDS, age_arr);
    uint32_t p2_answer = run_memory_game(input, input_len, PART_2_NUM_ROUNDS, age_arr);

    printf("PART 1: %u (expected %u)\n", p1_answer, PART_1_ANSWER);
    printf("PART 2: %u (expected %u)\n", p2_answer, PART_2_ANSWER);

    free(age_arr);
    return 0;
}
