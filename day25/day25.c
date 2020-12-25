#include <stdio.h>
#include <stdint.h>

#define ANSWER 11707042

#define INIT_SUBJECT_NUM 7
#define TRANSFORM_MOD 20201227  
#define PUB_KEY_1 15335876
#define PUB_KEY_2 15086442

uint64_t transform(uint64_t subject_num, uint64_t loop_size) {
    uint64_t result = 1;
    for (int i = 0; i < loop_size; i++) {
        result = (result * subject_num) % TRANSFORM_MOD;
    }
    return result;
}

uint64_t find_loop(uint64_t key) {
    uint64_t result = 1;
    for (uint64_t i = 1; i < 100000000; i++) {
        result = (result * INIT_SUBJECT_NUM) % TRANSFORM_MOD;
        if (result == key) return i;
    }
    return 0;
}

// 4846.0us (-Od) / 1470.0us (-O2)
int main() {
    // We only need to find the number of loops for one of the public keys
    uint64_t num_loops = find_loop(PUB_KEY_1);
    uint64_t encryption_key = transform(PUB_KEY_2, num_loops);
    printf("ANSWER: %llu\n", encryption_key);
    return 0;
}
