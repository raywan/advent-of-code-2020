/*
 * CREATED: 2020/12/06
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/6
*/

#include <stdio.h>
#include <stdint.h>
#if defined(_WIN32)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#define MAX_LINE_LEN 64
#define ALL_QUESTION_FLAGS_SET 0x3FFFFFF

// https://en.wikipedia.org/wiki/Hamming_weight
/* const uint32_t m1  = 0x55555555; // binary: 0101... */
/* const uint32_t m2  = 0x33333333; // binary: 00110011... */
/* const uint32_t m4  = 0x0f0f0f0f; // binary: 4 zeros, 4 ones ... */
/* const uint32_t h01 = 0x01010101; // the sum of 256 to the power of 0,1,2,3... */
inline uint32_t count_bits(uint32_t flags) {
#if 0
    uint32_t count = 0;
    for (count = 0; flags; count++) {
        flags &= flags - 1;
    }
    return count;
#else
    return _mm_popcnt_u32(flags);
#endif
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d06_input.txt", "r");

    char line[MAX_LINE_LEN];
    uint32_t sum_group_any_yeses = 0;
    uint32_t sum_group_unanimous_yeses = 0;
    uint32_t any_yes_flags = 0;
    uint32_t unanimous_yes_flags = ALL_QUESTION_FLAGS_SET;
    while (fgets(line, MAX_LINE_LEN, f)) {
        if (line[0] == '\n') {
            sum_group_any_yeses += count_bits(any_yes_flags);; 
            sum_group_unanimous_yeses += count_bits(unanimous_yes_flags); 
            // Reset the flags for the next group
            any_yes_flags = 0;
            unanimous_yes_flags = ALL_QUESTION_FLAGS_SET;
        } else {
            uint32_t yeses = 0;
            for (int i = 0; line[i] >= 'a' && line[i] <= 'z'; i++) {
                yeses |= 1 << (line[i]-'a');
            }
            any_yes_flags |= yeses;
            unanimous_yes_flags &= yeses;
        }
    }

    // Make sure to count the last group
    sum_group_any_yeses += count_bits(any_yes_flags);; 
    sum_group_unanimous_yeses += count_bits(unanimous_yes_flags); 

    printf("PART 1: %u\n", sum_group_any_yeses);
    printf("PART 2: %u\n", sum_group_unanimous_yeses);

    fclose(f);
    return 0;
}
