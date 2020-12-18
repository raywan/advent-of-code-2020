#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define MAX_INPUT_LINE_LEN 256

#define PART_1_ANSWER 4940631886147
#define PART_2_ANSWER 283582817678281

inline char process_input(char *line, char processed_input[MAX_INPUT_LINE_LEN], uint32_t *out_num_chars) {
    uint32_t num_chars = 0;
    for (int i = 0; line[i]; i++) {
        if ((line[i] >= '0' && line[i] <= '9') || 
            (line[i] == '+' || line[i] == '*') || 
            (line[i] == '(' || line[i] == ')')) {
            processed_input[num_chars++] = line[i];
        }
    }
    *out_num_chars = num_chars;
    return 0;
}

inline int find_matching_bracket(char processed_input[MAX_INPUT_LINE_LEN], int idx) {
    int bracket_weight = 1;
    int i = 0;        
    for (i = idx+1; bracket_weight != 0; i++) {
        if (processed_input[i] == '(') {
            bracket_weight++;
        } else if (processed_input[i] == ')') {
            bracket_weight--;
        }
    }
    return i-1;
}

uint64_t calculate_expr_basic(char processed_input[MAX_INPUT_LINE_LEN], uint32_t l, uint32_t r) {
    uint64_t result = 0;
    while (l < r) {
        uint64_t left = 0;
        uint64_t right = 0;
        if (processed_input[l] == '(') {
            int matching_closing = find_matching_bracket(processed_input, l);            
            left = calculate_expr_basic(processed_input, l+1, matching_closing-1);
            l = matching_closing+1;
        } else if (!result) {
            left = (uint64_t) (processed_input[l++] - '0');
        } else {
            left = result;
        }

        char op = processed_input[l++];

        if (processed_input[l] == '(') {
            int matching_closing = find_matching_bracket(processed_input, l);            
            right = calculate_expr_basic(processed_input, l+1, matching_closing-1);
            l = matching_closing+1;
        } else {
            right = (uint64_t) (processed_input[l++] - '0');
        }
        switch (op) {
            case '*':
                result = left * right;
                break;;
            case '+':
                result = left + right;
                break;;
        }
    }

    return result;
}

uint64_t calculate_expr_advanced(char processed_input[MAX_INPUT_LINE_LEN], uint32_t num_chars) {
    int op_stack_ptr = 0;
    char op_stack[32];
    int write_ptr = 0;
    char output_queue[MAX_INPUT_LINE_LEN];

    // Use the shunting-yard algorithm to parse the input into RPN
    for (int i = 0; i < num_chars; i++) {
        char c = processed_input[i];
        if (c >= '0' && c <= '9') {
            output_queue[write_ptr++] = c;
        } else if (c == '+' || c == '*') {
            while ((op_stack_ptr > 0) && 
                   (op_stack[op_stack_ptr-1] != '(') &&
                   (c == '*' && op_stack[op_stack_ptr-1] == '+')) {
                output_queue[write_ptr++] = op_stack[op_stack_ptr-1];
                op_stack_ptr--;
            }
            op_stack[op_stack_ptr++] = c;
        } else if (c == '(') {
            op_stack[op_stack_ptr++] = c;
        } else if (c == ')') {
            while ((op_stack_ptr > 0) && (op_stack[op_stack_ptr-1] != '(')) {
                output_queue[write_ptr++] = op_stack[op_stack_ptr-1];
                op_stack_ptr--;
            }
            op_stack_ptr--;
        }
    }

    // Push the remaining operators
    while (op_stack_ptr > 0) {
        output_queue[write_ptr++] = op_stack[op_stack_ptr-1];
        op_stack_ptr--;
    }

    // Now compute the result
    int sp = 0;
    uint64_t compute_stack[8] = {0};
    for (int i = 0; i < write_ptr; i++) {
        char c = output_queue[i];
        if (c >= '0' && c <= '9') {
            compute_stack[sp++] = c - '0';
        } else {
            uint64_t left = compute_stack[sp-2];
            uint64_t right = compute_stack[sp-1];
            switch (c) {
                case '+':
                    compute_stack[sp-2] = left + right;
                    break;
                case '*':
                    compute_stack[sp-2] = left * right;
                    break;
            }
            sp -= 1;
        }
    }

    return compute_stack[0];
}


// Part 1: 145.1us (-Od) / 119.7us (-O2)
// Part 2: 279.5us (-Od) / 194.2us (-O2)
int main() {
    FILE *f = fopen("d18_input.txt", "rb");
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    char *input_buf = (char *) malloc(file_size+1);
    fread(input_buf, file_size, 1, f);
    input_buf[file_size] = '\0';

    uint32_t num_chars = 0;
    char processed_input[MAX_INPUT_LINE_LEN] = {0};
    char *line = strtok(input_buf, "\n");
    uint64_t basic_sum = 0;
    uint64_t advanced_sum = 0;
    while (line) {
        if (line[0] == '\n' || line[0] == '\r') break;
        process_input(line, processed_input, &num_chars);
        uint64_t expr_result_basic = calculate_expr_basic(processed_input, 0, num_chars);
        uint64_t expr_result_advanced = calculate_expr_advanced(processed_input, num_chars);
        basic_sum += expr_result_basic;
        advanced_sum += expr_result_advanced;
         
        line = strtok(NULL, "\n");
    }

    printf("PART 1: %llu (expected %llu)\n", basic_sum, PART_1_ANSWER);
    printf("PART 2: %llu (expected %llu)\n", advanced_sum, PART_2_ANSWER);

    fclose(f);
    return 0;
}
