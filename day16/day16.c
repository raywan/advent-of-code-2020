/*
 * CREATED: 2020/12/16
 * AUTHOR: Raymond Wan
 * https://adventofcode.com/2020/day/16
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PART_1_ANSWER 21081
#define PART_2_ANSWER 314360510573

#define MAX_NEARBY_TICKETS 264
#define MAX_NUM_TICKET_FIELDS 20

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct Interval {
    uint32_t start;
    uint32_t end;
} Interval;

typedef struct Field {
    Interval range_1;
    Interval range_2;
} Field;

typedef struct FieldPossibility {
    int field_idx;
    int num_possibilities;
    int cols[MAX_NUM_TICKET_FIELDS];
} FieldPossibility;

int interval_cmp_func(const void *a, const void *b) {
    Interval ia = *(Interval *) a;
    Interval ib = *(Interval *) b;
    return ia.start - ib.start;
}

int possibility_cmp_func(const void *a, const void *b) {
    FieldPossibility fpa = *(FieldPossibility *) a;
    FieldPossibility fpb = *(FieldPossibility *) b;
    return fpa.num_possibilities - fpb.num_possibilities;
}

inline void parse_ticket(char *line, uint32_t ticket_fields[MAX_NUM_TICKET_FIELDS]) {
    int j = 0;
    int num_fields = 0;
    for (int i = 0; num_fields < MAX_NUM_TICKET_FIELDS; i++) {
        j = i;
        while (line[j] >= '0' && line[j] <= '9') {j++;}
        int digit = 1;
        uint32_t num = 0;
        for (int k = j-1; k >= i; k--) {
            num += digit * (line[k] - '0');
            digit *= 10;
        }
        ticket_fields[num_fields] = num;
        num_fields++;
        i = j;
    }
}

uint32_t merge_field_intervals(Interval *merged_intervals, Interval *intervals, uint32_t num_intervals) {
    uint32_t num_merged = 0;
    Interval intermediate = intervals[0];
    for (int i = 1; i < num_intervals; i++) {
        if (intervals[i].start > intermediate.end) {
            merged_intervals[num_merged++] = intermediate;
            intermediate = intervals[i];
        } else {
            intermediate.end = MAX(intermediate.end, intervals[i].end);
        }
    }

    // Make sure to put the last one in
    merged_intervals[num_merged++] = intermediate;

    return num_merged;
}

inline int validate_ticket(uint32_t *out_ticket_scan_error_rate, uint32_t ticket[MAX_NUM_TICKET_FIELDS], uint32_t num_fields, Interval *merged_intervals, uint32_t num_merged) {
    int valid = 1;
    for (int i = 0; i < num_fields; i++) {
        if (ticket[i] < merged_intervals[0].start) {
            *out_ticket_scan_error_rate += ticket[i];
            valid = 0;
        } else if (ticket[i] > merged_intervals[num_merged-1].end) {
            *out_ticket_scan_error_rate += ticket[i];
            valid = 0;
        } else {
            for (int j = 0; j < num_merged; j++) {
                if ((ticket[i] < merged_intervals[j].end) && (ticket[i] < merged_intervals[j].start)) {
                    *out_ticket_scan_error_rate += ticket[i];
                    valid = 0;
                    break;
                } else if ((ticket[i] <= merged_intervals[j].end) && (ticket[i] >= merged_intervals[j].start)) {
                    break;
                }
            }
        }
    }
    return valid;
}

// There must be a smarter way to part 2...Although my approach has decent perf
// Part 1: 38.0us (-Od) / 11.3 (-O2)
// Part 2: 278.2us (-Od) / 152.9 (-O2)
int main() {
    FILE *f = fopen("d16_input.txt", "rb");
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    char *input_buf = (char *) malloc(file_size+1); 
    fread(input_buf, file_size, 1, f);
    input_buf[file_size] = '\0';

    uint32_t num_tickets = 0;
    // First ticket is yours
    uint32_t tickets[MAX_NEARBY_TICKETS+1][MAX_NUM_TICKET_FIELDS];
    // This is the transpose of the ticket matrix but only with valid ticket values
    uint32_t field_values[MAX_NUM_TICKET_FIELDS][MAX_NEARBY_TICKETS];

    uint32_t num_intervals = 0;
    Interval intervals[MAX_NUM_TICKET_FIELDS*2];

    uint32_t num_fields = 0;
    Field fields[MAX_NUM_TICKET_FIELDS];
    
    int section = 0;
    char *line = strtok(input_buf, "\n");
    while (line) {
        if (line[0] == '\r' || line[0] == '\n') { section++; }
        if (section == 0) {
            int j = 0;
            while (line[j] != ':') { j++; };
            j += 2;
            int a,b,c,d;
            sscanf(&line[j], " %d-%d or %d-%d", &a, &b, &c, &d); 
            intervals[num_intervals].start = a;
            intervals[num_intervals].end = b;
            fields[num_fields].range_1 = intervals[num_intervals];
            num_intervals++;

            intervals[num_intervals].start = c;
            intervals[num_intervals].end = d;
            fields[num_fields].range_2 = intervals[num_intervals];
            num_intervals++;

            num_fields++;
        } else if (line[0] >= '0' && line[0] <= '9') {
            parse_ticket(line, tickets[num_tickets]);
            num_tickets++;
        }

        line = strtok(NULL, "\n");
    }

    // Sort the intervals by start time and then merge them
    // This reduces the number of intervals we need to check
    // In the case of the input, merging reduces it to one interval
    qsort(intervals, num_intervals, sizeof(Interval), interval_cmp_func);

    Interval merged_intervals[MAX_NUM_TICKET_FIELDS*2];
    uint32_t num_merged = merge_field_intervals(merged_intervals, intervals, num_intervals);

    uint32_t ticket_scan_error_rate = 0;
    uint32_t num_valid_tickets = 0;
    for (int i = 1; i < num_tickets; i++) {
        int valid = validate_ticket(&ticket_scan_error_rate, tickets[i], num_fields, merged_intervals, num_merged);
        if (valid) {
            for (int j = 0; j < num_fields; j++) {
                field_values[j][num_valid_tickets] = tickets[i][j];
            }
            num_valid_tickets++;
        }
    }

    // For each ticket field position, we want to validate against each field interval to develop
    // a set of potential assignments. We then sort by the number of potential matches
    // and by a process of elimination, we can determine which field corresponds to which position
    // in the ticket.
    // The complexity of the following approach is as follows:
    // num_positions_in_ticket * num_valid_tickets * num_ticket_fields
    // For the input given, this is: 20 * 190 * 20 = 76000
    int assigned_fields[MAX_NUM_TICKET_FIELDS];
    memset(assigned_fields, -1, sizeof(int) * MAX_NUM_TICKET_FIELDS);

    FieldPossibility field_possibilities[MAX_NUM_TICKET_FIELDS];
    for (int i = 0; i < num_fields; i++) {
        field_possibilities[i].field_idx = i;
        int num_possibilities = 0;
        for (int j = 0; j < num_fields; j++) {
            int valid = 1;
            for (int k = 0; k < num_valid_tickets; k++) {
                uint32_t val = field_values[j][k];
                if (((val < fields[i].range_1.start) || (val > fields[i].range_1.end)) &&
                    ((val < fields[i].range_2.start) || (val > fields[i].range_2.end))) {
                    valid = 0;
                    break;
                }
            }
            if (valid) {
                field_possibilities[i].cols[num_possibilities++] = j;
            }
        }
        field_possibilities[i].num_possibilities = num_possibilities;
    }

    qsort(field_possibilities, num_fields, sizeof(FieldPossibility), possibility_cmp_func);
    for (int i = 0; i < num_fields; i++) {
        FieldPossibility fp = field_possibilities[i];
        for (int j = 0; j < fp.num_possibilities; j++) {
            if (assigned_fields[fp.cols[j]] == -1) {
                assigned_fields[fp.cols[j]] = fp.field_idx;
            }
        }
    }
    
    uint64_t departure_fields_product = 1;
    for (int i = 0; i < num_fields; i++) {
        if (assigned_fields[i] >= 0 && assigned_fields[i] <= 5) {
            departure_fields_product *= (uint64_t) tickets[0][i];
        }
    }

    printf("PART 1: %u (expected %u)\n", ticket_scan_error_rate, PART_1_ANSWER);
    printf("PART 2: %llu (expected %llu)\n", departure_fields_product, PART_2_ANSWER);

    free(input_buf);
    fclose(f);
    return 0;
}
