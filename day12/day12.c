#include <stdio.h>
#include <stdint.h>

#define PART_1_ANSWER 1589
#define PART_2_ANSWER 23960
#define MAX_INPUT_LINES 1024

#define ABS(x) ((x) >= 0 ? (x) : (-1 * (x)))

typedef enum Direction {
    Direction_N,
    Direction_E,
    Direction_S,
    Direction_W,
    Direction_COUNT
} Direction;

inline void apply_waypoint_rotation(char dir, int32_t angle, int32_t *out_wx, int32_t *out_wy) {
    if (angle == 180) {
        *out_wx *= -1;
        *out_wy *= -1;
        return;
    } else if (angle == 270) {
        dir = dir == 'L' ? 'R' : 'L';
    }

    int32_t w_tmp = *out_wx;
    switch (dir) {
        case 'L': 
            *out_wx = -1 * (*out_wy);
            *out_wy = w_tmp;
            break;
        case 'R':
            *out_wx = *out_wy;
            *out_wy = -1 * w_tmp;
            break;
        default:
            break;
    }
}

// Part 1 and 2 are computed in the same pass
// Timings are a total of the inner loop times to avoid including file I/O
// 21.5us (-Od) / 16.3us (-O2)
int main(int argc, char *argv[]) {
    FILE *f = fopen("d12_input.txt", "rb");

    // We start at facing east at 0, 0 
    Direction facing_dir = Direction_E;
    // Part 1 ship positioning
    int32_t p1_px = 0;
    int32_t p1_py = 0;
    // Waypoint starting position (relative to the ship)
    int32_t wx = 10;
    int32_t wy = 1;
    // Part 2 ship positioning (using the waypoint for movement)
    int32_t p2_px = 0;
    int32_t p2_py = 0;

    char action;
    int32_t magnitude;
    while (fscanf(f, "%c%d\n", &action, &magnitude) != EOF) {
        switch (action) {
            case 'R': {
                int32_t steps = magnitude/90;
                facing_dir = (facing_dir + steps) % Direction_COUNT;
                apply_waypoint_rotation(action, magnitude, &wx, &wy);
            } break;
            case 'L': {
                int32_t steps = -magnitude/90;
                if (facing_dir + steps < 0) {
                    facing_dir = Direction_COUNT + (facing_dir+steps);
                } else {
                    facing_dir = (facing_dir + steps);
                }
                apply_waypoint_rotation(action, magnitude, &wx, &wy);
            } break;
            case 'N': {
                p1_py += magnitude;
                wy += magnitude;
            } break;
            case 'S': {
                p1_py -= magnitude;
                wy -= magnitude;
            } break;
            case 'E': {
                p1_px += magnitude;
                wx += magnitude;
            } break;
            case 'W': {
                p1_px -= magnitude;
                wx -= magnitude;
            } break;
            case 'F': {
                switch (facing_dir) {
                    case Direction_N: 
                        p1_py += magnitude;
                        break;
                    case Direction_E:
                        p1_px += magnitude;
                        break;
                    case Direction_S:
                        p1_py -= magnitude;
                        break;
                    case Direction_W:
                        p1_px -= magnitude;
                        break;
                }
                p2_px += wx * magnitude;
                p2_py += wy * magnitude;
            } break;
            default:
                break;
        }
    }

    printf("PART 1: %u (expected %u)\n", ABS(p1_px) + ABS(p1_py), PART_1_ANSWER);
    printf("PART 2: %u (expected %u)\n", ABS(p2_px) + ABS(p2_py), PART_2_ANSWER);

    fclose(f);
    return 0;
}
