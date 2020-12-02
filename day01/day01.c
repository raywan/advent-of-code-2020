#include <stdio.h>
#include <stdint.h>

#define DAY01_INPUT_SIZE 200
#define TARGET_SUM 2020

int cmp_func(const void *a, const void *b) {
    return (*(uint32_t *) a) - (*(uint32_t *) b);
}

int main(int argc, char *argv[]) {
    uint32_t nums[DAY01_INPUT_SIZE]; 
    FILE *f = fopen("d01_input.txt", "r");
    uint32_t n;
    for (int i = 0; fscanf(f, "%d", &n) != EOF; i++) {
        nums[i] = n;
    }

    qsort(nums, DAY01_INPUT_SIZE, sizeof(uint32_t), cmp_func);

    int i = 0;
    int j = DAY01_INPUT_SIZE - 1;
    while (i < j) {
        uint32_t sum = nums[i] + nums[j];
        if (sum < TARGET_SUM) {
            i++;
        } else if (sum > TARGET_SUM) {
            j--;
        } else {
            printf("PART 1: %d\n", nums[i] * nums[j]);
            break;
        }
    }

    for (int i = 0 ; i < DAY01_INPUT_SIZE - 1; i++) {
        for (int j = i+1 ; j < DAY01_INPUT_SIZE; j++) {
            int32_t diff = TARGET_SUM - nums[i] - nums[j];
            if (diff > 0) {
                for (int k = 0 ; k < DAY01_INPUT_SIZE; k++) {
                    if ((k != i) && (k != j) && (diff == nums[k])) {
                        printf("PART 2: %d\n", nums[i] * nums[j] * nums[k]);
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}
