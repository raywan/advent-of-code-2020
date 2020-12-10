#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_INPUT_LINES 128

#define PART_1_ANSWER 2048
#define PART_2_ANSWER 1322306994176LLU

int cmp_func(const void *a, const void *b) {
    return (*(uint32_t *) a) - (*(uint32_t *) b);
}

inline uint64_t compute_num_ways(uint32_t adapters[MAX_INPUT_LINES], uint32_t num_adapters) {
    // DP solution which, at each index, represents the number of ways to reach that adapter
    uint64_t dp[MAX_INPUT_LINES] = {0};
    dp[0] = 1;
    for (int i = 1; i < num_adapters; i++) {
        // From my understanding, should be at most 3 back
        for (int j = i-1; adapters[i] - adapters[j] <= 3; j--) {
            dp[i] += dp[j];
        }
    }

    return dp[num_adapters-1];
}

// part 1: 6.1us (-Od)/4.8us (-O2) (includes sorting)
// part 2: 1.3us (-Od)/0.6us (-O2)
int main(int argc, char *argv[]) {
    FILE *f = fopen("d10_input.txt", "rb");

    int num_adapters;
    uint32_t adapters[MAX_INPUT_LINES];
    adapters[0] = 0;
    for (num_adapters = 1; fscanf(f, "%u", &adapters[num_adapters]) != EOF; num_adapters++) {}

    uint32_t num_1v_diff = 0;
    uint32_t num_3v_diff = 0;
    qsort(adapters, num_adapters, sizeof(uint32_t), cmp_func);

    // "your device's built-in adapter is always 3 higher than the highest adapter"
    uint32_t target_voltage = adapters[num_adapters-1] + 3;
    adapters[num_adapters++] = target_voltage;

    for (int i = 1; i < num_adapters; i++) {
        num_1v_diff += adapters[i]-adapters[i-1] == 1;
        num_3v_diff += adapters[i]-adapters[i-1] == 3;
    }

    uint64_t num_ways = compute_num_ways(adapters, num_adapters);

    printf("PART 1: %lu (expected %lu)\n", num_1v_diff * num_3v_diff, PART_1_ANSWER); 
    printf("PART 2: %llu (expected %llu)\n", num_ways, PART_2_ANSWER); 

    return 0;
}
