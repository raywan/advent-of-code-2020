/*
 * CREATED: 2020/12/02
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/2
*/

#include <stdio.h>
#include <stdint.h>

#define MAX_INPUT_STRING_LEN 64

uint8_t is_password_valid(uint32_t min, uint32_t max, char letter, char str[MAX_INPUT_STRING_LEN],
                          uint32_t *out_num_valid_part_1, uint32_t *out_num_valid_part_2) {
    uint32_t freq = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        freq += (uint32_t) (str[i] == letter);
    }
    *out_num_valid_part_1 += (freq <= max) && (freq >= min);
    *out_num_valid_part_2 += (str[min-1] == letter) ^ (str[max-1] == letter);
    return 1;
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d02_input.txt", "r");

    uint32_t min, max;
    char letter;
    char str[MAX_INPUT_STRING_LEN];
    uint32_t num_valid_passwords_part_1 = 0;
    uint32_t num_valid_passwords_part_2 = 0;
    int num_lines = 0;

    while (fscanf(f, "%d-%d %c: %s", &min, &max, &letter, str) != EOF) {
        is_password_valid(min, max, letter, str, &num_valid_passwords_part_1, &num_valid_passwords_part_2);
    }

    printf("PART 1: %d\n", num_valid_passwords_part_1);
    printf("PART 2: %d\n", num_valid_passwords_part_2);

    fclose(f);
    return 0;
}
