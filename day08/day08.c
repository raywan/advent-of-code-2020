/*
 * CREATED: 2020/12/8
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/8
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum Opcode {
    Opcode_NOP,
    Opcode_JMP,
    Opcode_ACC,
    Opcode_COUNT,
} Opcode;

typedef struct Instruction {
    Opcode opcode;
    int32_t operand;
    uint32_t last_pc;
} Instruction;

uint32_t string_to_num(char *arg) {
    uint32_t result = 0;
    char sign = arg[0];
    arg++;
    uint32_t num_digits = 0;
    for (num_digits = 0; arg[num_digits] >= '0' && arg[num_digits] <= '9'; num_digits++) {}

    uint32_t digit = 1;
    for (int i = num_digits-1; i >= 0; i--) {
        result += digit * (arg[i] - '0');
        digit *= 10;
    }
    result = sign == '-' ? -1 * result : result; 

    return result;
}

uint32_t string_to_opcode(char *opcode_str) {
    switch (opcode_str[0]) {
        case 'n':
            return Opcode_NOP;
        case 'a':
            return Opcode_ACC;
        case 'j':
            return Opcode_JMP;
        default:
            return Opcode_NOP;
    }
}

void reset_program(Instruction *instructions, uint32_t num_instructions) {
    for (uint32_t i = 0; i < num_instructions; i++) {
        Instruction *cur_instruction = &instructions[i];
        cur_instruction->last_pc = 0;
    }
}

uint32_t run_program(Instruction *instructions, uint32_t num_instructions, uint32_t *out_accum) {
    uint8_t loop_detected = 0;
    uint32_t accumulator_value = 0; 
    reset_program(instructions, num_instructions);
    for (uint32_t i = 0; !loop_detected && (i < num_instructions); i++) {
        Instruction *cur_instruction = &instructions[i];
        cur_instruction->last_pc = i+1;
        Opcode opcode = cur_instruction->opcode;
        switch (opcode) {
            case Opcode_NOP:
                break;
            case Opcode_ACC:
                accumulator_value += cur_instruction->operand;
                break;
            case Opcode_JMP: {
                int next_i = i + cur_instruction->operand;
                loop_detected = instructions[next_i].last_pc != 0;
                i = next_i - 1; // need to offset
            } break;
        }
    }
    *out_accum = accumulator_value;
    return !loop_detected;
}

uint32_t accumulator_before_first_loop(Instruction *instructions, uint32_t num_instructions) {
    int32_t accumulator_value = 0; 
    run_program(instructions, num_instructions, &accumulator_value);
    return accumulator_value;
}
uint32_t accumulator_after_termination(Instruction *instructions, uint32_t num_instructions) {
    int32_t swap_opcode_idx = -1;
    uint32_t accumulator_value = 0; 
    uint8_t successfully_terminated = 0; 
    while (!successfully_terminated && (swap_opcode_idx < (int32_t) num_instructions-1)) {
        int found_swap = 0;
        for (uint32_t i = swap_opcode_idx+1; !found_swap && (i < num_instructions); i++) {
            switch (instructions[i].opcode) {
                case Opcode_NOP:
                case Opcode_JMP:
                    swap_opcode_idx = i;
                    found_swap = 1;
                    break;
                default:
                    break;
            }
        }

        // Swap the opcode at the index 
        instructions[swap_opcode_idx].opcode ^= Opcode_JMP;

        successfully_terminated = run_program(instructions, num_instructions, &accumulator_value);

        // Swap the opcode back
        instructions[swap_opcode_idx].opcode ^= Opcode_JMP;
    }
    return accumulator_value;
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("d08_input.txt", "rb");
    fseek(f, 0ULL, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    char *buf = (char *) malloc(file_size + 1);
    fread(buf, 1, file_size, f);    
    buf[file_size] = '\0';

    uint32_t num_instructions = 0;
    Instruction instructions[1024];

    // Parse the buffer
    char *opcode_str;
    char *operand_str;
    for (int offset = 0; buf[offset] != '\0'; offset++) {
        opcode_str = buf + offset;
        // The first three characters consist of the opcode
        offset += 4;
        operand_str = buf + offset;

        instructions[num_instructions] = (Instruction) {0};
        instructions[num_instructions].opcode = string_to_opcode(opcode_str);
        instructions[num_instructions].operand = string_to_num(operand_str);

        ++num_instructions;

        // Go to the end of the line
        for (;buf[offset] != '\n'; offset++) {}
    }

    printf("PART 1: %d\n", accumulator_before_first_loop(instructions, num_instructions));
    printf("PART 2: %d\n", accumulator_after_termination(instructions, num_instructions));

    free(buf);
    fclose(f);
    return 0;
}
