#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../timer.h"

#define MAX_INPUT_LINES 1024
#define NUM_HISTORY 25

#define PART_1_ANSWER 1398413738
#define PART_2_ANSWER 169521051

inline void find_extents(uint64_t nums[MAX_INPUT_LINES], uint32_t num_nums, uint32_t end_before_idx,
                         uint64_t *out_min1, uint64_t *out_min2, uint64_t *out_max1, uint64_t *out_max2) {
    uint64_t min1 = INT64_MAX;
    uint64_t min2 = INT64_MAX;
    uint64_t max1 = 0;
    uint64_t max2 = 0;

    for (int i = end_before_idx - NUM_HISTORY; i < end_before_idx; i++) {
        if (nums[i] < min1) {
            min2 = min1;
            min1 = nums[i];
        } else if (nums[i] < min2 && nums[i] != min1) {
            min2 = nums[i];
        }

        if (nums[i] > max2) {
            max1 = max2;
            max2 = nums[i];
        } else if (nums[i] > max1) {
            max1 = nums[i];
        }
    }

    *out_min1 = min1;
    *out_min2 = min2;
    *out_max1 = max1;
    *out_max2 = max2;
}

inline int is_valid_in_sequence(uint64_t n, uint64_t min1, uint64_t min2, uint64_t max1, uint64_t max2) {
    return (n >= (min1+min2)) && (n <= (max1+max2));
}

inline uint64_t find_min_max_sum(uint64_t nums[MAX_INPUT_LINES], uint32_t num_nums, uint32_t end_before_idx) {
    // Calculating and using the cumulative sums reduces our time complexity to O(n*n)
    uint64_t cum_sum[MAX_INPUT_LINES];
    int appropriate_start_idx = 0;
    cum_sum[0] = nums[0];
    for (int i = 1; i < end_before_idx; i++) {
        cum_sum[i] = cum_sum[i-1] + nums[i];
        if (!appropriate_start_idx && cum_sum[i] > nums[end_before_idx]) {
            appropriate_start_idx = i;
        }
        
    }

    // Naive iteration averages a time of 365us (-Od) for this input set
    // Instead, we want to start at the index where the the cumulative sum becomes
    // greater than or equal to our target
    // Using this starting value reduced this to 32us (-Od)
    // Optimized builds (-O2) go from 101.4us to 7.5us
    uint64_t min = INT64_MAX;
    uint64_t max = 0;
    for (int i = appropriate_start_idx; i < end_before_idx-1; i++) {
        for (int j = 0; j < i; j++) {
            uint64_t sum = cum_sum[i] - cum_sum[j];
            if (sum == nums[end_before_idx]) {
                for (int k = j+1; k <= i; k++) {
                    if (nums[k] < min) min = nums[k];
                    if (nums[k] > max) max = nums[k];
                }
                return min + max;
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d09_input.txt", "rb");

    uint32_t num_nums = 0;
    uint64_t nums[MAX_INPUT_LINES];
    for (int i = 0; fscanf(f, "%llu", &nums[i]) != EOF; i++) { num_nums++; }

    uint32_t invalid_number_idx = 0;
    uint64_t min1, min2, max1, max2;
    for (int i = NUM_HISTORY; i < num_nums; i++) {
        find_extents(nums, num_nums, i, &min1, &min2, &max1, &max2);
        if (!is_valid_in_sequence(nums[i], min1, min2, max1, max2)) {
            invalid_number_idx = i;
            break;
        }
    }

    uint64_t min_max_sum = find_min_max_sum(nums, num_nums, invalid_number_idx);

    printf("PART 1: %llu (expected %d)\n", nums[invalid_number_idx], PART_1_ANSWER);
    printf("PART 2: %llu (expected %d)\n", min_max_sum, PART_2_ANSWER);

    fclose(f);
    return 0;
}
