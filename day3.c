#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <limits.h>
#include <stdio.h>

typedef struct {
    long x, y;
} Position;

typedef struct {
    Position key;
    long value;
} VisitedPoint;

typedef struct {
    char dir;
    long len;
}  WireSegment;

int parse_file(FILE *f, WireSegment **wire1, WireSegment **wire2);
void solve(WireSegment *wire1, WireSegment *wire2);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    WireSegment *wire1 = NULL;
    WireSegment *wire2 = NULL;
    int parse_error = parse_file(f, &wire1, &wire2);
    if (!parse_error) {
        solve(wire1, wire2);
    } else {
        perror("parse error");
    }

    if (ferror(f)) {
        perror("read error");
    }

    fclose(f);
    return 0;
}

Position get_delta(char dir) {
    switch (dir) {
    case 'U':
        return (Position){0, -1};
        break;
    case 'D':
        return (Position){0, 1};
        break;
    case 'L':
        return (Position){-1, 0};
        break;
    case 'R':
        return (Position){1, 0};
        break;
    default:
        return (Position){0, 0};
    }
}

void solve(WireSegment *wire1, WireSegment *wire2) {
    VisitedPoint *visited = NULL;
    long part1 = LONG_MAX;
    long part2 = LONG_MAX;
    long steps = 0;
    Position curr = {0, 0};
    for (int i = 0; i < arrlen(wire1); ++i) {
        for (int j = 0; j < wire1[i].len; ++j) {
            Position d = get_delta(wire1[i].dir);
            curr.x += d.x;
            curr.y += d.y;
            steps += 1;
            if (hmgeti(visited, curr) == -1) {
                hmput(visited, curr, steps);
            }
        }
    }

    steps = 0;
    curr = (Position){0, 0};
    for (int i = 0; i < arrlen(wire2); ++i) {
        for (int j = 0; j < wire2[i].len; ++j) {
            Position d = get_delta(wire2[i].dir);
            curr.x += d.x;
            curr.y += d.y;
            steps += 1;
            if (hmgeti(visited, curr) != -1) {
                long other_steps = hmget(visited, curr);
                long dist = labs(curr.x) + labs(curr.y);
                long step_dist = other_steps + steps;
                if (dist < part1) {
                    part1 = dist;
                }
                if (step_dist < part2) {
                    part2 = step_dist;
                }
            }
        }
    }

    printf("%ld\n", part1);
    printf("%ld\n", part2);
}

int parse_wire(char *line, WireSegment **wire) {
    char *p = line;
    while (*p != '\0' && *p != '\n') {
        WireSegment seg;
        seg.dir = *p++;
        seg.len = strtol(p, &p, 10);
        arrput(*wire, seg);

        if (*p == ',')
            p++;
    }
    return 0;
}

int parse_file(FILE *f, WireSegment **wire1, WireSegment **wire2) {
    char line[4000];
    if (fgets(line, sizeof(line), f) == NULL)
        return 1;
    parse_wire(line, wire1);
    if (fgets(line, sizeof(line), f) == NULL)
        return 1;
    parse_wire(line, wire2);

    return 0;
}
