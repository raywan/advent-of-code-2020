/*
 * CREATED: 2020/12/7
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/7
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 256
#define MAX_RULES 594
#define HASH_TABLE_SIZE (1031)
#define HASH_TABLE_LINEAR_PROBE_ENTRIES 8

typedef struct String {
    char *buf;
    uint32_t len;
} String;

typedef struct HashEntry {
    String key;
    int value; // index in rules list
} HashEntry;

typedef struct BagRule {
    String bag;
    int contains_shiny_gold;
    uint32_t num_bags_contained;
    uint32_t quantities[8];
    String contained_bags[8];
} BagRule;

uint32_t djb2_hash_bag(String s) {
    uint32_t hash = 5381;
    for (int i = 0; i < s.len; i++) {
        char c = s.buf[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

String get_words(int num_words, char *buf, int *i) {
    String result;
    result.buf = buf+*i;
    result.len = 0;
    while (num_words) {
        result.len++;
        *i += 1;
        if (buf[*i] == '\n' || buf[*i] == ' ') num_words--;
    }
    *i += 1; // eat the space
    return result;
}

inline void skip_words(int num_words, char *buf, int *i) {
    while (num_words) {
        *i += 1;
        if (buf[*i] == ' ') num_words--;
    }
    *i += 1; // eat the space
}

int string_compare_const(String s, const char *b) {
    if (strlen(b) != s.len) return 0;
    for (int i = 0; i < s.len; i++) {
        if (s.buf[i] != b[i]) return 0;
    }
    return 1;
}

int string_compare(String a, String b) {
    if (a.len != b.len) return 0;
    for (int i = 0; i < a.len; i++) {
        if (a.buf[i] != b.buf[i]) return 0;
    }
    return 1;
}

int string_to_int(String s) {
    int result = 0;
    int digit = 1;
    for (int i = s.len-1; i >= 0; i--) {
        if (s.buf[i] >= '0' && s.buf[i] <= '9') {
            result += digit * (s.buf[i] - '0');
            digit *= 10;
        }
    }
    return result;
}

int insert_bag_ht(HashEntry ht[HASH_TABLE_SIZE][HASH_TABLE_LINEAR_PROBE_ENTRIES], String bag, int value) {
    uint32_t hash_idx = djb2_hash_bag(bag) % HASH_TABLE_SIZE;
    for (int i = 0; i < HASH_TABLE_LINEAR_PROBE_ENTRIES; i++) {
        if (!ht[hash_idx][i].key.len) {
            ht[hash_idx][i].key = bag;
            ht[hash_idx][i].value = value;
            //ht[hash_idx][i].contains_shiny_gold = 0;
            //printf("%.*s => HT IDX: %d, Slot: %d, Value: %d\n", bag.len, bag.buf, hash_idx, i, value);
            break;
        }
    }
    return 0;
}

HashEntry get_bag_entry(HashEntry ht[HASH_TABLE_SIZE][HASH_TABLE_LINEAR_PROBE_ENTRIES], String bag) {
    uint32_t hash_idx = djb2_hash_bag(bag) % HASH_TABLE_SIZE;
    for (int i = 0; i < HASH_TABLE_LINEAR_PROBE_ENTRIES; i++) {
        if (string_compare(ht[hash_idx][i].key, bag)) {
            return ht[hash_idx][i];
        }
    }
    return ht[0][0];
}

uint32_t count_total_bags(HashEntry ht[HASH_TABLE_SIZE][HASH_TABLE_LINEAR_PROBE_ENTRIES], 
                    BagRule bag_rules[MAX_RULES], String bag) {
    HashEntry he = get_bag_entry(ht, bag);
    uint32_t result = 0;
    for (int i = 0; i < bag_rules[he.value].num_bags_contained; i++) {
        result += bag_rules[he.value].quantities[i] + 
            (bag_rules[he.value].quantities[i] * count_total_bags(ht, bag_rules, bag_rules[he.value].contained_bags[i]));
    }
    return result;
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d07_input.txt", "r");

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    char *buf = (char *) malloc(file_size+1);
    fread(buf, file_size, 1, f);
    buf[file_size] = 0;

    HashEntry ht[HASH_TABLE_SIZE][HASH_TABLE_LINEAR_PROBE_ENTRIES] = {0};
    BagRule bag_rules[MAX_RULES] = {0};

    char *line_ptr = NULL;
    int line_n = 0;
    int offset = 0;
    // Insert rules into hash table
    while (line_n < MAX_RULES) {
        line_ptr = buf+offset;
        int i = 0;
        String bag = get_words(2, line_ptr, &i);
        skip_words(2, line_ptr, &i); // skip "bags contain"
        insert_bag_ht(ht, bag, line_n);
        bag_rules[line_n].bag = bag;
        bag_rules[line_n].contains_shiny_gold = -1; // We don't know yet

        for (;;) {
            String num = get_words(1, line_ptr, &i); // could be "no"
            if (string_compare_const(num, "no")) {
                bag_rules[line_n].num_bags_contained = 0;
                bag_rules[line_n].contains_shiny_gold = 0;
                break;
            }
            String contain_bag = get_words(2, line_ptr, &i);
            String separator_word = get_words(1, line_ptr, &i);
            if (string_compare_const(contain_bag, "shiny gold")) {
                bag_rules[line_n].contains_shiny_gold = 1;
            }
            bag_rules[line_n].quantities[bag_rules[line_n].num_bags_contained] = string_to_int(num);
            bag_rules[line_n].contained_bags[bag_rules[line_n].num_bags_contained] = contain_bag;
            bag_rules[line_n].num_bags_contained++;
            if (separator_word.buf[separator_word.len-1] == '.') {
                break;
            }
        }

        // Advance to the newline
        for (;buf[offset] != '\n'; offset++) {}
        // Go to the next start of the next line
        offset++;
        line_n++;
    }

    // Should converge because the rules form a DAG
    int any_unknowns_remaining = 0;
    do {
        any_unknowns_remaining = 0;
        for (int i = 0; i < MAX_RULES; i++) {
            HashEntry he = get_bag_entry(ht, bag_rules[i].bag);
            if (bag_rules[i].contains_shiny_gold == -1) {
                bag_rules[i].contains_shiny_gold = 0;
                for (int j = 0; j < bag_rules[i].num_bags_contained; j++) {
                    HashEntry contained_he = get_bag_entry(ht, bag_rules[i].contained_bags[j]);
                    BagRule contained_bag = bag_rules[contained_he.value];
                    if (contained_bag.contains_shiny_gold == 1) {
                        bag_rules[i].contains_shiny_gold = 1;
                    } else if (contained_bag.contains_shiny_gold == -1) {
                        bag_rules[i].contains_shiny_gold = -1;
                    }
                }
                if (bag_rules[i].contains_shiny_gold == -1) {
                    any_unknowns_remaining = 1;
                }
            }
        }
    } while (any_unknowns_remaining);

    uint32_t num_bags_with_shiny_golds = 0;
    for (int i = 0; i < MAX_RULES; i++) {
        num_bags_with_shiny_golds += bag_rules[i].contains_shiny_gold == 1;
    }

    printf("PART 1: %d\n", num_bags_with_shiny_golds);

    String s;
    s.buf = "shiny gold";
    s.len = 10;
    uint32_t num_bags_within_shiny_gold = count_total_bags(ht, bag_rules, s);
    printf("PART 2: %d\n", num_bags_within_shiny_gold);

    fclose(f);
    return 0;
}
