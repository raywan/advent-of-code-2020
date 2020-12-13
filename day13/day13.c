#include <stdio.h>
#include <stdint.h>

#define MAX_LINE_LEN 256

#define PART_1_ANSWER 205
#define PART_2_ANSWER 803025030761664

inline void parse_bus_ids(char line[MAX_LINE_LEN], uint32_t *buses, uint32_t *out_num_buses, uint32_t *time_offsets) {
    uint32_t num_buses = 0;
    int i = 0;
    int line_len = strlen(line);
    int offset = 0;
    while (i < line_len) {
        int j = i;
        while (line[j] >= '0' && line[j] <= '9') { j++; }
        int digit = 1;
        int num = 0;
        for (int k = j-1; k >= i; k--) {
            num += digit * (line[k] - '0');
            digit *= 10;
        }
        time_offsets[num_buses] = offset;
        buses[num_buses++] = num;
        i = j;
        while ((i < line_len) && !(line[i] >= '0' && line[i] <= '9')) { 
            i++; 
            if (line[i] == ',') offset++;
        }

        offset++;
    }
    *out_num_buses = num_buses;
}


// Part 1: 0.20us (-Od) / 0.10us (-O2)
// Part 2: 16.1us (-Od) / 13.8us (-O2)
int main(int argc, char *argv[]) {
    FILE *f = fopen("d13_input.txt", "rb");
    uint32_t earliest_departure_time = 0;
    fscanf(f, "%u\n", &earliest_departure_time);

    uint32_t buses[32];
    uint32_t time_offsets[32];
    uint32_t num_buses = 0;

    char line[MAX_LINE_LEN];
    fgets(line, MAX_LINE_LEN, f);

    parse_bus_ids(line, buses, &num_buses, time_offsets);

    uint32_t result_bus_id = 0; 
    uint32_t min_time_diff = UINT32_MAX; 

    for (int i = 0; i < num_buses; i++) {
        uint32_t next_departure_time = (earliest_departure_time/buses[i] + 1) * buses[i];
        uint32_t time_diff = next_departure_time - earliest_departure_time ;
        if (time_diff < min_time_diff) {
            result_bus_id = buses[i];
            min_time_diff = time_diff;
        }
    }

    uint64_t earliest_time_with_offsets = 0;
    for (uint64_t n = 1; n <= num_buses-1; n++) {
        uint64_t product = 1;
        for (uint64_t i = 0; i < n; i++) {
            product *= buses[i];
        }

        for (uint64_t check = earliest_time_with_offsets; ; check += product) {
            int found = 1;
            for (int i = 0; i < n+1; i++) {
                if (((check + time_offsets[i]) % buses[i]) != 0) {
                    found = 0;
                    break;
                }
            }

            if (found) {
                earliest_time_with_offsets = check;
                break;
            }
        }
    }

    printf("PART 1: %u (expected %u)\n", min_time_diff * result_bus_id, PART_1_ANSWER);
    printf("PART 2: %llu (expected %llu)\n", earliest_time_with_offsets, PART_2_ANSWER);

    fclose(f);
    return 0;
}
