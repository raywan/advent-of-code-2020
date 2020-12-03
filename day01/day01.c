/*
 * CREATED: 2020/12/02
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/1
*/

#include <stdio.h>
#include <stdint.h>

#define DAY01_INPUT_SIZE 200
#define TARGET_SUM 2020

int cmp_func(const void *a, const void *b) {
    return (*(uint32_t *) a) - (*(uint32_t *) b);
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d01_input.txt", "r");
    uint32_t nums[DAY01_INPUT_SIZE]; 
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

    for (int i = 0 ; i < DAY01_INPUT_SIZE - 2; i++) {
        uint32_t hold = nums[i];
        int j = i+1;
        int k = DAY01_INPUT_SIZE-1;
        while (j < k) {
            uint32_t sum = hold + nums[j] + nums[k];
            if (sum == TARGET_SUM) {
                printf("PART 2: %d\n", nums[i] * nums[j] * nums[k]);
                return 0;
            } else if (sum < 0) {
                j++; 
            } else {
                k--;
            }
        }
    }
    
    fclose(f);
    return 0;
}
