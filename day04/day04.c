/*
 * CREATED: 2020/12/04
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/4
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 256
#define MAX_KEY_VALUE_LENGTH 64

typedef uint8_t PassportFieldFlags;
typedef enum PassportField {
    PassportField_BYR = 0x1,
    PassportField_IYR = 0x2,
    PassportField_EYR = 0x4,
    PassportField_HGT = 0x8,
    PassportField_HCL = 0x10,
    PassportField_ECL = 0x20,
    PassportField_PID = 0x40,
    PassportField_CID = 0x80,
} PassportField;

int is_valid_passport(PassportFieldFlags flags) {
    // We only need to check if the first 7 bits are set i.e 0x7F
    // For completeness, we can also check for all 8 bits
    return (flags & 0x7F) == 0x7F;
}

void set_passport_field_flag(char *key_value_pair, PassportFieldFlags *loose_flags, PassportFieldFlags *strict_flags) {
    char c1 = key_value_pair[0];
    char c2 = key_value_pair[1];
    char c3 = key_value_pair[2];
    char *value = key_value_pair + 4; // Skip the key plus : character
    if (c2 == 'y' && c3 == 'r') {
        int year = atoi(value);
        switch (c1) {
            case 'b': {
                *loose_flags |= PassportField_BYR;
                if (year >= 1920 && year <= 2002) {
                    *strict_flags |= PassportField_BYR;
                }
            } break;
            case 'i': {
                *loose_flags |= PassportField_IYR;
                if (year >= 2010 && year <= 2020) {
                    *strict_flags |= PassportField_IYR;
                }
            } break;
            case 'e': {
                *loose_flags |= PassportField_EYR;
                if (year >= 2020 && year <= 2030) {
                    *strict_flags |= PassportField_EYR;
                }
            } break;
        }
    } else if (c2 == 'i' && c3 == 'd') {
        switch (c1) {
            case 'p': {
                *loose_flags |= PassportField_PID;
                if (strlen(value) == 9) {
                    *strict_flags |= PassportField_PID;
                }
            } break;
            case 'c': {
                *loose_flags |= PassportField_CID;
                *strict_flags |= PassportField_CID;
            } break;
        }
    } else if (c2 == 'c' && c3 == 'l') {
        switch (c1) {
            case 'h': {
                *loose_flags |= PassportField_HCL;
                if (strlen(value) == 7) {
                    for (int i = 1; i < 7; i++) {
                        char c = value[i];
                        if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f')) return;
                    }
                    *strict_flags |= PassportField_HCL;
                }
            } break; 
            case 'e': {
                *loose_flags |= PassportField_ECL;
                if (strlen(value) == 3 &&
                    ((value[0] == 'a' && value[1] == 'm' && value[2] == 'b') || 
                    (value[0] == 'b' && value[1] == 'l' && value[2] == 'u') ||
                    (value[0] == 'b' && value[1] == 'r' && value[2] == 'n') ||
                    (value[0] == 'g' && value[1] == 'r' && value[2] == 'y') ||
                    (value[0] == 'g' && value[1] == 'r' && value[2] == 'n') ||
                    (value[0] == 'h' && value[1] == 'z' && value[2] == 'l') ||
                    (value[0] == 'o' && value[1] == 't' && value[2] == 'h'))) {
                    *strict_flags |= PassportField_ECL;
                }
            } break; 
        }
    } else {
        *loose_flags |= PassportField_HGT;
        if (strlen(value) >= 4) {
            char unit[2];
            unit[0] = value[strlen(value)-2];
            unit[1] = value[strlen(value)-1];
            value[strlen(value)-2] = '\0';
            int measurement = atoi(value);
            if (((unit[0] == 'c' && unit[1] == 'm') && (measurement >= 150 && measurement <= 193)) ||
                ((unit[0] == 'i' && unit[1] == 'n') && (measurement >= 59 && measurement <= 76))) {
                *strict_flags |= PassportField_HGT;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d04_input.txt", "r");

    char key_value_pair[MAX_KEY_VALUE_LENGTH];
    char line[MAX_LINE_LENGTH];
    uint32_t num_valid_passports_loose = 0;
    uint32_t num_valid_passports_strict = 0;
    PassportFieldFlags loose_flags = 0;
    PassportFieldFlags strict_flags = 0;

    while (fgets(line, MAX_LINE_LENGTH, f)) {
        if (line[0] == '\n') {
            // This is the end of the passport's entries, check the flags now
            num_valid_passports_loose += is_valid_passport(loose_flags);
            num_valid_passports_strict += is_valid_passport(strict_flags);
            // Reset flags for the new passport
            loose_flags = 0;
            strict_flags = 0;
        } else {
            int line_len = strlen(line); 
            uintptr_t line_offset = 0;
            char *line_ptr = line;
            while (sscanf(line_ptr, "%s", key_value_pair)) {
                set_passport_field_flag(key_value_pair, &loose_flags, &strict_flags);
                line_offset += strlen(key_value_pair)+1;
                if (line_offset >= line_len) break;
                line_ptr = line+line_offset;
            }
        }
    }

    // Make sure we process the last passport
    num_valid_passports_loose += is_valid_passport(loose_flags);
    num_valid_passports_strict += is_valid_passport(strict_flags);

    printf("PART 1: %d\n", num_valid_passports_loose);
    printf("PART 2: %d\n", num_valid_passports_strict);

    fclose(f);
    return 0;
}
