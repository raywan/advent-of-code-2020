/*
 * CREATED: 2020/12/19
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/19
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PRODUCTION_RULES 135
#define MAX_INPUT_STRINGS 512
#define MAX_LINE_LEN 128

#define UNIVERSAL_TERMINAL 69

#define PART_1_ANSWER 107
#define PART_2_ANSWER 321

typedef struct ProductionRule {
    char terminal;
    uint8_t num_options;
    int options[3][2];
} ProductionRule;

typedef struct String {
    char *buf;
    uint32_t len;
} String;

void parse_rule(char *line, ProductionRule *rules) {
    int j = 0;
    int i = 0;
    int idx = 0;
    int found_terminal_rule = 0;
    int option = 0;
    ProductionRule *rule = NULL;
    while (i < strlen(line)) {
        j = i;

        // Go to the end of the current number
        while (line[j] >= '0' && line[j] <= '9') { j++; };
        int digit = 1;
        int num = 0;
        for (int k = j-1; k >= i; k--) {
            num += digit * (line[k] - '0');
            digit *= 10;
        }
        if (option == 0) {
            rule = &rules[num];
            option++;
        } else if (option == 1) {
            rule->options[0][idx++] = num;
            rule->num_options = 1;
        } else if (option == 2) {
            rule->options[1][idx++] = num;
            rule->num_options = 2;
        }

        // Go to the next number
        while (!(line[j] >= '0' && line[j] <= '9')) {
            if (line[j] == '|') {
                option++;
                idx = 0;
            }

            if (line[j] == '"') {
                found_terminal_rule = 1;
                rule->terminal = line[j+1];
                rule->num_options = 0;
                break;
            } else {
                j++; 
            }
            if (line[j] == '\n' || line[j] == '\r') {
                break;
            }
        };
        
        if (found_terminal_rule) {
            break;
        }

        i = j;
    }
}

void adjust_grammar_for_part_2(ProductionRule *rules, int *out_num_rules) {
    // Change the following rules to:
    // 8: 42 | 42 8
    // 11: 42 31 | 42 11 31
    // Since the number of change are small, we can manually convert to valid CNF
    int next_rule = *out_num_rules;
    rules[next_rule].num_options = 1;
    memset(rules[next_rule].options, -1, sizeof(int) * 3 * 2);
    rules[next_rule].options[0][0] = 11;
    rules[next_rule].options[0][1] = 31;

    rules[11].num_options = 2;
    rules[11].options[1][0] = 42;
    rules[11].options[1][1] = next_rule;
    *out_num_rules = next_rule+1;

    rules[8].num_options = 3;
    rules[8].options[2][0] = 42;
    rules[8].options[2][1] = 8;
}

// https://en.wikipedia.org/wiki/CYK_algorithm
// https://en.wikipedia.org/wiki/Chomsky_normal_form
static uint32_t cyk_table[MAX_LINE_LEN][MAX_LINE_LEN][MAX_PRODUCTION_RULES];
float timer = 0;
uint32_t is_string_valid(String s, ProductionRule *rules, uint32_t num_rules) {
    // Initialize the table
    memset(cyk_table, 0, sizeof(uint32_t) * MAX_LINE_LEN * MAX_LINE_LEN * MAX_PRODUCTION_RULES);
    for (int i = 1 ; i <= s.len; i++) {
        for (int k = 0 ; k < num_rules; k++) {
            if (rules[k].terminal && ((s.buf[i-1] == rules[k].terminal) || rules[k].terminal == UNIVERSAL_TERMINAL)) {
                cyk_table[1][i][k] = 1;
            }
        }
    }

    for (int l = 2; l <= s.len; l++) { // substring length
        for (int ss = 1; ss <= s.len-l+1; ss++) { // start of substring
            for (int p = 1; p <= l-1; p++) { // substring partition 
                for (int k = 0; k < num_rules; k++) {
                    if (!rules[k].terminal) {
                        for (int i = 0; i < rules[k].num_options; i++) {
                            if (cyk_table[p][ss][rules[k].options[i][0]] && 
                                cyk_table[l-p][ss+p][rules[k].options[i][1]]) {
                                cyk_table[l][ss][k] = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    return cyk_table[s.len][1][0];
}

// The performance of today's challenge is not great...
//
// I implemented the algorithm essentially as presented on Wikipedia
// CYK has a complexity of O(n^3 * |G|) where |G| is the size of the CNF grammar.
// With optimizations, here are some examples of the algorithm's runtime:
// len: 24 => ~700us (best case length)
// len: 40 => ~3ms
// len: 64 => ~13ms
// len: 80 => ~23ms (worst case length)
//
// I'd have to think about this problem some more but for sure I could
// be more intelligent about how I place/operate on the data.
// These results are very disappointing and wouldn't even be able to 
// run in real-time...
//
// Total runtimes which include initializing memory:
// Part 1: 4088.4ms (-Od) / 1587.47ms (-O2)
// Part 2: 4482.6ms (-Od) / 1508.09ms (-O2)
int main() {
    FILE *f = fopen("d19_input.txt", "rb");
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    char *input_buf = (char *) malloc(file_size + 1);
    fread(input_buf, file_size, 1, f);
    input_buf[file_size] = '\0';

    int num_strings = 0;
    String strings[MAX_INPUT_STRINGS];

    int num_rules = 0;
    ProductionRule rules[MAX_PRODUCTION_RULES];
    for (int i = 0; i < MAX_PRODUCTION_RULES; i++) {
        rules[i].num_options = 0;
        rules[i].terminal = 0;
        memset(rules[i].options, -1, sizeof(int) * 3 * 2);
    }

    char *line = strtok(input_buf, "\r\n");
    while (line) {
        if (line[0] >= 'a') {
            strings[num_strings].buf = line;
            strings[num_strings].len = strlen(line);
            ++num_strings;
        } else if (line[0] >= '0') {
            parse_rule(line, rules);
            ++num_rules;
        }

        line = strtok(NULL, "\r\n");
    }

    // Eliminate unit rules from the grammar (just rule 8) to have a valid CNF
    // Also adjust one of the rules to be considered terminal (rule 121)
    for (int i = 0; i < num_rules; i++) {
        int any_negative = 0;
        for (int j = 0; j < rules[i].num_options; j++) {
            for (int k = 0; k < 2; k++) {
                if (rules[i].options[j][k] == -1) {
                    any_negative = 1;
                }
            }
        }

        if (rules[i].num_options == 1 && any_negative) {
            memcpy(&rules[i], &rules[rules[i].options[0][0]], sizeof(ProductionRule));
        } else if (any_negative) {
            rules[i].terminal = UNIVERSAL_TERMINAL;
        }
    }

    uint32_t valid_strings_p1 = 0;
    for (int i = 0; i < num_strings; i++) {
        valid_strings_p1 += is_string_valid(strings[i], rules, num_rules);
    }

    adjust_grammar_for_part_2(rules, &num_rules); 

    uint32_t valid_strings_p2 = 0;
    for (int i = 0; i < num_strings; i++) {
        valid_strings_p2 += is_string_valid(strings[i], rules, num_rules);
    }

    printf("PART 1: %u (expected %u)\n", valid_strings_p1, PART_1_ANSWER);
    printf("PART 2: %u (expected %u)\n", valid_strings_p2, PART_2_ANSWER);

    free(input_buf);
    fclose(f);
    return 0;
}
