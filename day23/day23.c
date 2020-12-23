#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PART_1_ANSWER "72496583"
// Product of 195397 and 213851
#define PART_2_ANSWER 41785843847

#define SMALL_GAME_NUM_CUPS 9
#define SMALL_GAME_NUM_ROUNDS 100
#define BIG_GAME_NUM_CUPS 1000000
#define BIG_GAME_NUM_ROUNDS 10000000

typedef struct NumberNode {
    uint32_t val;
    struct NumberNode *next;
} NumberNode;

void play_small_game(int *input, uint32_t num_cups, int num_rounds) {
    uint32_t hold[3] = {0};
    int i = 0;
    for (int r = 1; r <= num_rounds; r++) {
        // Determine the destination cup
        uint32_t destination = (input[i]-1) % num_cups == 0 ? num_cups : (input[i]-1);

        // Pick the the next three cups
        int dest_picked_up = 0;
        for (int j = 0; j < 3; j++) {
            hold[j] = input[(i+j+1) % num_cups];
            if (destination == hold[j]) dest_picked_up = 1;
        }

        // Adjust the destination cup if necessary
        while (dest_picked_up) {
            destination = (destination-1) % num_cups == 0 ? num_cups : (destination-1);
            dest_picked_up = 0;
            for (int j = 0; j < 3; j++) {
                if (destination == hold[j]) dest_picked_up = 1;
            }
        }

        // move the cups
        int write_idx = (i+1) % num_cups;
        int num_rearranged = 0;
        for (int j = i+4; input[j % num_cups] != destination; j++) {
            input[write_idx] = input[j % num_cups];
            write_idx = (write_idx+1) % num_cups;
            num_rearranged++;
        }

        input[write_idx] = destination;
        write_idx = (write_idx+1) % num_cups;
        for (int j = 0; j < 3; j++) {
            input[write_idx] = hold[j];
            write_idx = (write_idx+1) % num_cups;
        }

        i = (i+1) % num_cups;
    }
}


void play_big_game(NumberNode *numbers, NumberNode *big_input_list, int num_rounds) {
    NumberNode *cur = big_input_list;
    for (int r = 1; r <= num_rounds; r++) {
        // Determine the destination cup
        uint32_t destination = (cur->val-1) % BIG_GAME_NUM_CUPS == 0 ? BIG_GAME_NUM_CUPS : (cur->val-1);

        // Pick the the next three cups
        NumberNode *hold_start = cur->next;
        NumberNode *hold_end = hold_start;
        for (int j = 0; j < 2; j++) {
            hold_end = hold_end->next;
        }

        int dest_picked_up = 0;
        NumberNode *hold_check = hold_start;
        for (int j = 0; j < 3; j++) {
            if (destination == hold_check->val) {
                dest_picked_up = 1;
                break;
            }
            hold_check = hold_check->next;
        }
        
        while (dest_picked_up) {
            hold_check = hold_start;
            destination = (destination-1) % BIG_GAME_NUM_CUPS == 0 ? BIG_GAME_NUM_CUPS : (destination-1);
            dest_picked_up = 0;
            for (int j = 0; j < 3; j++) {
                if (destination == hold_check->val) {
                    dest_picked_up = 1;
                    break;
                }
                hold_check = hold_check->next;
            }
        }

        // Now adjust
        NumberNode *new_start = hold_end->next;
        NumberNode *adjust_end = &numbers[destination];
        NumberNode *after_adjust_end = adjust_end->next;

        cur->next = new_start;
        adjust_end->next = hold_start;
        hold_end->next = after_adjust_end;

        cur = new_start;
    }
}

// The solution for part 1 manipulated the input array directly.
// This approach would be too slow for part 2. We actually spend most of the time
// shifting values in the input array. Depending on the current permutation, we
// would be shifting almost 1 million values per round (essentially the whole set).
// Multiplying this by the 10000000 rounds we need to simulate, this would take hours,
//
// After printing out a few rounds and trying to find a pattern, 
// I realized that it is only necessary to make a few adjustments per round.
// Specifically, if we convert the input into a linked list, we would
// only have to adjust a few pointers per round.
//
// After timing the result, the solution didn't run as fast as I thought it would.
// I'm sure there is a more optimal solution, but I haven't thought of it yet.
//
// Part 1: 8.7us (-Od) / 5.3us (-O2)
// Part 2: 1174.82ms (-Od) / 1009.14ms (-O2)
int main() {
    char raw_input[] = "315679824";

    uint32_t input[9];
    for (int i = 0; i < 9; i++) {
        input[i] = raw_input[i] - '0';
    }

    // Preallocate all the number nodes and create a linked list
    NumberNode *numbers = (NumberNode *) malloc(sizeof(NumberNode) * (BIG_GAME_NUM_CUPS+1));
    for (int i = 1; i <= BIG_GAME_NUM_CUPS; i++) {
        numbers[i].val = i; 
        numbers[i].next = NULL; 
    }

    NumberNode *big_input_list = &numbers[input[0]];
    NumberNode *cur = big_input_list;

    // Deal with the initial input
    for (int i = 1; i < 9; i++) {
        cur->next = &numbers[input[i]];
        cur = cur->next;
    }

    // Add the remaining numbers to the list
    for (int i = 10; i <= BIG_GAME_NUM_CUPS; i++) {
        cur->next = &numbers[i];
        cur = cur->next;
    }

    // Create a cycle from the last number to the first
    numbers[BIG_GAME_NUM_CUPS].next = big_input_list;

    play_small_game(input, SMALL_GAME_NUM_CUPS, SMALL_GAME_NUM_ROUNDS);

    play_big_game(numbers, big_input_list, BIG_GAME_NUM_ROUNDS);

    printf("PART 1: ");
    int one_idx = 0;
    for (int i = 0; i < SMALL_GAME_NUM_CUPS; i++) {
        if (input[i] == 1) {
            one_idx = i;
            break;
        }
    }

    for (int i = one_idx+1; input[i % SMALL_GAME_NUM_CUPS] != 1; i++) {
        printf("%d", input[i % SMALL_GAME_NUM_CUPS]);
    }
    printf(" (expected %s)\n", PART_1_ANSWER);

    uint64_t product = 1;
    cur = &numbers[1];
    for (int i = 0; i < 2; i++) {
        cur = cur->next;
        product *= cur->val;
    }

    printf("PART 2: %llu (expected %llu)\n", product, PART_2_ANSWER);

    return 0;
}
