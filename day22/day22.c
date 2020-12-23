#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define DECK_SIZE 25
#define QUEUE_SIZE 64

#define PART_1_ANSWER 31314
#define PART_2_ANSWER 32760

typedef struct Deck {
    uint32_t cards[QUEUE_SIZE];
    uint32_t r, w;
    uint32_t len;
} Deck;

typedef struct History {
    uint32_t p1_num_cards;
    uint32_t p2_num_cards;
    uint8_t p1_cards[DECK_SIZE*2];
    uint8_t p2_cards[DECK_SIZE*2];
} History;

typedef struct HistoryArray {
    uint32_t len;
    uint32_t capacity;
    History *buf;
} HistoryArray;

HistoryArray ha_init(uint32_t cap) {
    HistoryArray result;
    result.len = 0;
    result.capacity = cap;
    result.buf = (History *) malloc(sizeof(History) * cap);
    return result;
}

void ha_free(HistoryArray ha) {
    free(ha.buf);
}

void ha_insert(HistoryArray *ha, History h) {
    if (ha->len + 1 > ha->capacity) {
        ha->capacity *= 2;
        ha->buf = realloc(ha->buf, sizeof(History) * ha->capacity);
    }
    ha->buf[ha->len++] = h;
}

uint32_t deck_pop(uint32_t deck[QUEUE_SIZE], uint32_t *r, uint32_t *len) {
    uint32_t result = deck[*r];
    *r = (*r + 1) % QUEUE_SIZE;
    *len = *len - 1;
    return result;
}

void deck_push(uint32_t val, uint32_t deck[QUEUE_SIZE], uint32_t *w, uint32_t *len) {
    deck[*w] = val;
    *w = (*w + 1) % QUEUE_SIZE;
    *len = *len + 1;
}

uint32_t play_combat(uint32_t d1[DECK_SIZE], uint32_t d1_len, uint32_t d2[DECK_SIZE], uint32_t d2_len) {
    uint32_t p1_deck[QUEUE_SIZE] = {0};
    uint32_t p2_deck[QUEUE_SIZE] = {0};
    memcpy(p1_deck, d1, sizeof(uint32_t) * d1_len);
    memcpy(p2_deck, d2, sizeof(uint32_t) * d2_len);
    uint32_t p1_r = 0;
    uint32_t p1_w = d1_len;
    uint32_t p1_deck_len = d1_len;

    uint32_t p2_r = 0;
    uint32_t p2_w = d2_len;
    uint32_t p2_deck_len = d2_len;
    int r = 0;
    while (p1_deck_len && p2_deck_len) {
        uint32_t p1_card = deck_pop(p1_deck, &p1_r, &p1_deck_len);
        uint32_t p2_card = deck_pop(p2_deck, &p2_r, &p2_deck_len);
        if (p1_card > p2_card) {
            deck_push(p1_card, p1_deck, &p1_w, &p1_deck_len);
            deck_push(p2_card, p1_deck, &p1_w, &p1_deck_len);
        } else {
            deck_push(p2_card, p2_deck, &p2_w, &p2_deck_len);
            deck_push(p1_card, p2_deck, &p2_w, &p2_deck_len);
        }
    }

    uint32_t score = 0;
    if (p1_deck_len == 0) {
        int len = p2_deck_len;
        for (int i = 0; p2_deck_len; i++) {
            uint32_t card = deck_pop(p2_deck, &p2_r, &p2_deck_len);
            score += (len - i) * card;
        }
    } else if (p2_deck_len == 0) {
        int len = p1_deck_len;
        for (int i = 0; p1_deck_len; i++) {
            uint32_t card = deck_pop(p1_deck, &p1_r, &p1_deck_len);
            score += (len - i) * card;
        }
    }
    return score;
}

int is_game_state_repeated(HistoryArray ha, Deck *d1, Deck *d2) {
    int found_match = 0;
    for (int i = 0; i < ha.len; i++) {
        History h = ha.buf[i];
        if (h.p1_num_cards == d1->len && h.p2_num_cards == d2->len) {
            int p1_match = 1;
            int p2_match = 1;
            for (int j = 0; j < h.p1_num_cards; j++) {
                if (d1->cards[d1->r + j % QUEUE_SIZE] != h.p1_cards[j]) {
                    p1_match = 0;
                    break;
                }
            }
            if (p1_match) {
                for (int j = 0; j < h.p2_num_cards; j++) {
                    if (d2->cards[d2->r + j % QUEUE_SIZE] != h.p2_cards[j]) {
                        p2_match = 0;
                        break;
                    }
                }
            }
            
            found_match = p1_match && p2_match;
            if (found_match) {
                return 1;
            }
        }
    }
    return 0;
}

// Returns who won the round
uint32_t play_recursive_combat(Deck *p1_deck, Deck *p2_deck, HistoryArray *ha) {
    while (p1_deck->len && p2_deck->len) {
        int round_winner = 0;
        if (is_game_state_repeated(*ha, p1_deck, p2_deck)) {
            return 1;
        } 

        History h;
        h.p1_num_cards = p1_deck->len;
        h.p2_num_cards = p2_deck->len;
        for (int i = 0; i < p1_deck->len; i++) {
            h.p1_cards[i] = p1_deck->cards[(p1_deck->r + i) % QUEUE_SIZE];
        }
        for (int i = 0; i < p2_deck->len; i++) {
            h.p2_cards[i] = p2_deck->cards[(p2_deck->r + i) % QUEUE_SIZE];
        }
        ha_insert(ha, h);

        if (p1_deck->len == 0) {
            return 2;
        } else if (p2_deck->len == 0) {
            return 1;
        }
        uint32_t p1_card = deck_pop(p1_deck->cards, &p1_deck->r, &p1_deck->len);
        uint32_t p2_card = deck_pop(p2_deck->cards, &p2_deck->r, &p2_deck->len);
        if (p1_card <= p1_deck->len && p2_card <= p2_deck->len) {
            // Copy the remaining cards to a new deck
            Deck copy_d1; 
            copy_d1.r = 0;
            copy_d1.w = p1_card;
            copy_d1.len = p1_card;
            for (int i = 0; i < p1_card; i++) {
                copy_d1.cards[i] = p1_deck->cards[(p1_deck->r + i) % QUEUE_SIZE];
            }

            Deck copy_d2; 
            copy_d2.r = 0;
            copy_d2.w = p2_card;
            copy_d2.len = p2_card;
            for (int i = 0; i < p2_card; i++) {
                copy_d2.cards[i] = p2_deck->cards[(p2_deck->r + i) % QUEUE_SIZE];
            }

            HistoryArray new_ha = ha_init(512);
            round_winner = play_recursive_combat(&copy_d1, &copy_d2, &new_ha);
            ha_free(new_ha);
        } else {
            round_winner = p1_card > p2_card ? 1 : 2;
        }

        if (round_winner == 1) {
            deck_push(p1_card, p1_deck->cards, &p1_deck->w, &p1_deck->len);
            deck_push(p2_card, p1_deck->cards, &p1_deck->w, &p1_deck->len);
        } else if (round_winner == 2) {
            deck_push(p2_card, p2_deck->cards, &p2_deck->w, &p2_deck->len);
            deck_push(p1_card, p2_deck->cards, &p2_deck->w, &p2_deck->len);
        }
    }

    if (p1_deck->len == 0) {
        return 2;
    }

    return 1;
}

// The history check for the recursive game is VERY inefficient
// Part 1: 15.2us (-Od) / 2.9us (-O2)
// Part 2: 5549.39ms (-Od) / 1021.42ms (-O2)
int main() {
    FILE *f = fopen("d22_input.txt", "rb");
    
    uint32_t p1_deck[DECK_SIZE] = {0};
    uint32_t p2_deck[DECK_SIZE] = {0};
    uint32_t p1_deck_len = 0;
    uint32_t p2_deck_len = 0;

    char buf[16];
    fgets(buf, 16, f);

    uint32_t *deck_ptr = p1_deck;
    uint32_t *deck_len_ptr = &p1_deck_len;
    int i = 0; 
    while (fgets(buf, 16, f) != NULL) {
        if (buf[0] == 'P') {
            deck_ptr = p2_deck;
            *deck_len_ptr -= 1;
            deck_len_ptr = &p2_deck_len;
            i = 0;
        } else {
            deck_ptr[i++] = atoi(buf);
            *deck_len_ptr += 1;
        }
    }

    uint32_t combat_score = play_combat(p1_deck, p1_deck_len, p2_deck, p2_deck_len);

    HistoryArray ha = ha_init(512);
    Deck p1d;
    p1d.r = 0;
    p1d.w = p1_deck_len;
    p1d.len = p1_deck_len;
    memcpy(p1d.cards, p1_deck, sizeof(uint32_t) * p1_deck_len);
    Deck p2d;
    p2d.r = 0;
    p2d.w = p2_deck_len;
    p2d.len = p2_deck_len;
    memcpy(p2d.cards, p2_deck, sizeof(uint32_t) * p2_deck_len);

    uint32_t recursive_combat_winner = play_recursive_combat(&p1d, &p2d, &ha);

    uint32_t recursive_combat_score = 0;
    if (recursive_combat_winner == 1) {
        int len = p1d.len;
        for (int i = 0; p1d.len; i++) {
            uint32_t card = deck_pop(p1d.cards, &p1d.r, &p1d.len);
            recursive_combat_score += (len - i) * card;
        }
    } else if (recursive_combat_winner == 2) {
        int len = p2d.len;
        for (int i = 0; p2d.len; i++) {
            uint32_t card = deck_pop(p2d.cards, &p2d.r, &p2d.len);
            recursive_combat_score += (len - i) * card;
        }
    }

    printf("PART 1: %u (expected %u)\n", combat_score, PART_1_ANSWER);
    printf("PART 2: %u (expected %u)\n", recursive_combat_score, PART_2_ANSWER);

    ha_free(ha);
    fclose(f);
    return 0;
}
