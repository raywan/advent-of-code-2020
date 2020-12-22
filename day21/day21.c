/*
 * CREATED: 2020/12/21
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/21
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../timer.h"

#define MAX_LINE_LEN 512
#define MAX_INGREDENT_LIST_LEN 128
#define MAX_ALLERGEN_LIST_LEN 8
#define MAX_INGREDIENT_LISTS 48

#define PART_1_ANSWER 2635
#define PART_2_ANSWER "xncgqbcp,frkmp,qhqs,qnhjhn,dhsnxr,rzrktx,ntflq,lgnhmx"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

typedef enum Allergen {
    Allergen_DAIRY,
    Allergen_FISH,
    Allergen_NUTS,
    Allergen_PEANUTS,
    Allergen_SESAME,
    Allergen_SHELLFISH,
    Allergen_SOY,
    Allergen_WHEAT,
    Allergen_COUNT,
} Allergen;

typedef struct String {
    char *buf;
    uint32_t len;
} String;

typedef struct IngredientList { 
    uint32_t num_ingredients;
    uint32_t num_allergens;
    String ingredients[MAX_INGREDENT_LIST_LEN];
    Allergen allergens[MAX_ALLERGEN_LIST_LEN]; 
} IngredientList;

int string_compare_const(String s, const char *str) {
    if (s.len != strlen(str)) return 0;
    for (int i = 0; i < s.len; i++) {
        if (s.buf[i] != str[i]) return 0;
    }
    return 1;
}

int string_compare_string(String a, String b) {
    if (a.len != b.len) return 0;
    for (int i = 0; i < a.len; i++) {
        if (a.buf[i] != b.buf[i]) return 0;
    }
    return 1;
}

inline void parse_ingredients(char *line, IngredientList *ingredient_list) {
    int len = strlen(line);
    int end = 0;
    if (line[len-1] == ')') {
        for (end = len-1; line[end] != '('; end--) {}
    } else {
        end = len;
    }

    int j = 0;
    int n = 0;
    for (int i = 0; i < end; i++) {
        j = i;
        while (line[j] >= 'a' && line[j] <= 'z') { j++; }
        String s;
        s.buf = &line[i];
        s.len = j-i;
        ingredient_list->ingredients[n++] = s;
        i = j;
    }

    ingredient_list->num_ingredients = n;

    j = 0;
    n = 0;
    for (int i = end+10; i < len; i++) {
        j = i;
        while (line[j] >= 'a' && line[j] <= 'z') { j++; }
        String s;
        s.buf = &line[i];
        s.len = j-i;
        if (string_compare_const(s, "dairy")) {
            ingredient_list->allergens[n++] = Allergen_DAIRY;
        } else if (string_compare_const(s, "wheat")) {
            ingredient_list->allergens[n++] = Allergen_WHEAT;
        } else if (string_compare_const(s, "fish")) {
            ingredient_list->allergens[n++] = Allergen_FISH;
        } else if (string_compare_const(s, "shellfish")) {
            ingredient_list->allergens[n++] = Allergen_SHELLFISH;
        } else if (string_compare_const(s, "peanuts")) {
            ingredient_list->allergens[n++] = Allergen_PEANUTS;
        } else if (string_compare_const(s, "soy")) {
            ingredient_list->allergens[n++] = Allergen_SOY;
        } else if (string_compare_const(s, "nuts")) {
            ingredient_list->allergens[n++] = Allergen_NUTS;
        } else if (string_compare_const(s, "sesame")) {
            ingredient_list->allergens[n++] = Allergen_SESAME;
        }
        i = j+1;
    }

    ingredient_list->num_allergens = n;
}

// After starting to write the solution, I realized that it was taking too 
// long to test and validate my solution idea in C.
// I sucked it up and quickly wrote the inital solution in Python.
// After answers were submitted, I went back to complete the C solution. 
// Absolutely nothing smart is being done here.
// No hash tables used (contrary to Python), just straight up nested loops everywhere.
// 
// This is the total time to deduce the allergen ingredients,
// build the mapping, and then count all the non-allergen ingredients
// 2107.9us (-Od) /  265.8us (-O2)
int main() {
    FILE *f = fopen("d21_input.txt", "rb");
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    char *input_buf = (char *) malloc(file_size+1);
    fread(input_buf, file_size, 1, f);
    input_buf[file_size] = 0;
    char *line = strtok(input_buf, "\r\n");

    int num_lists = 0;
    IngredientList ingredient_lists[MAX_INGREDIENT_LISTS];
    while (line) {
        parse_ingredients(line, &ingredient_lists[num_lists++]);
        line = strtok(NULL, "\r\n");
    }

    uint32_t intersection_lens[Allergen_COUNT] = {0};
    String intersections[Allergen_COUNT][MAX_INGREDENT_LIST_LEN];

    // Find the intersection of the allergens starting with ingredient lists
    // that contain 1 allergen, then 2, 3, etc.
    // This deduces the ingredients that could possibly contain allergens
    for (int a = 0; a < Allergen_COUNT; a++) {
        for (int c = 1; c <= 3; c++) {
            int nl = 0;
            int list_indices[32] = {0};
            for (int j = 0; j < num_lists; j++) {
                IngredientList list = ingredient_lists[j];
                if (list.num_allergens == c) {
                    for (int k = 0; k < c; k++) {
                        if (list.allergens[k] == (Allergen) a) {
                            list_indices[nl++] = j;
                            break;
                        }
                    }
                }
            }

            if (!nl) continue;
            int ni = 0;
            if (c == 1) {
                memcpy(intersections[a], 
                       ingredient_lists[list_indices[0]].ingredients, 
                       sizeof(String) * ingredient_lists[list_indices[0]].num_ingredients);
                ni = ingredient_lists[list_indices[0]].num_ingredients;
            } else {
                ni = intersection_lens[a];
            }

            String tmp[MAX_INGREDENT_LIST_LEN];
            int tni = 0;
            for (int i = (c == 1 ? 1 : 0); i < nl; i++) {
                int idx = list_indices[i];
                for (int j = 0; j < ni; j++) {
                    for (int k = 0; k < ingredient_lists[idx].num_ingredients; k++) {
                        if (string_compare_string(intersections[a][j], ingredient_lists[idx].ingredients[k])) {
                            tmp[tni++] = intersections[a][j];
                        }
                    }
                }
                memcpy(intersections[a], tmp, sizeof(String) * tni);
                ni = tni;
                tni = 0;
            }

            intersection_lens[a] = ni;
        }
    }

    int max_intersection_len = 0;
    int min_intersection_len = MAX_INGREDENT_LIST_LEN;
    for (int i = 0; i < Allergen_COUNT; i++) {
        max_intersection_len = MAX(max_intersection_len, intersection_lens[i]);
        min_intersection_len = MIN(min_intersection_len, intersection_lens[i]);
    }

    // Determine which allergen maps to which ingredients
    // We do this by a process of elimination.
    // Start with allergens that contain only ingredient possibility,
    // Then move on with 2, then 3, and then 4
    String allergen_ingredient_mapping[Allergen_COUNT];
    for (int c = min_intersection_len; c <= max_intersection_len; c++) {
        for (int i = 0; i < Allergen_COUNT; i++) {
            if (c == intersection_lens[i]) {
                if (c == 1) {
                    allergen_ingredient_mapping[i] = intersections[i][0];
                } else {
                    for (int j = 0; j < c; j++) {
                        // check if we're already mapped to an allergen
                        int found_match = 0;
                        for (int k = 0; k < Allergen_COUNT; k++) {
                            if (string_compare_string(allergen_ingredient_mapping[k], intersections[i][j])) {
                                found_match = 1;
                            }
                        }
                        if (!found_match) {
                            allergen_ingredient_mapping[i] = intersections[i][j];
                            break;
                        }
                    }
                }
            }
        }
    }

    uint32_t num_no_allergen_ingredients = 0;
    for (int i = 0; i < num_lists; i++) {
        for (int j = 0; j < ingredient_lists[i].num_ingredients; j++) {
            int found = 0;
            for (int k = 0; k < Allergen_COUNT; k++) {
                if (string_compare_string(ingredient_lists[i].ingredients[j], allergen_ingredient_mapping[k])) {
                    found = 1;
                    break;
                }
            }
            num_no_allergen_ingredients += !found;
        }
    }

    char death_list[128] = {0};
    memcpy(death_list, allergen_ingredient_mapping[0].buf, allergen_ingredient_mapping[0].len);
    for (int i = 1; i < Allergen_COUNT; i++) {
        int len = strlen(death_list);
        death_list[len++] = ',';
        memcpy(death_list+len, allergen_ingredient_mapping[i].buf, allergen_ingredient_mapping[i].len);
    }

    printf("PART 1: %u (expected %u)\n", num_no_allergen_ingredients, PART_1_ANSWER);
    printf("PART 2: %s (expected %s)\n", death_list, PART_2_ANSWER);

    free(input_buf);
    fclose(f);
    return 0;
}

