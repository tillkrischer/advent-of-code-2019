#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

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

static int parse_file(FILE *f, WireSegment **wire1, WireSegment **wire2);
static void solve(const WireSegment *wire1, const WireSegment *wire2);

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
        fprintf(stderr, "Failed to parse input\n");
    }

    if (ferror(f)) {
        perror("read error");
    }
    arrfree(wire1);
    arrfree(wire2);

    fclose(f);
    return 0;
}

Position get_delta(char dir) {
    switch (dir) {
    case 'U':
        return (Position){0, -1};
    case 'D':
        return (Position){0, 1};
    case 'L':
        return (Position){-1, 0};
    case 'R':
        return (Position){1, 0};
    default:
        return (Position){0, 0};
    }
}

static void solve(const WireSegment *wire1, const WireSegment *wire2) {
    VisitedPoint *visited = NULL;
    long part1 = LONG_MAX;
    long part2 = LONG_MAX;
    long steps = 0;
    Position curr = {0, 0};
    for (ptrdiff_t i = 0; i < arrlen(wire1); ++i) {
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
    for (ptrdiff_t i = 0; i < arrlen(wire2); ++i) {
        for (int j = 0; j < wire2[i].len; ++j) {
            Position d = get_delta(wire2[i].dir);
            curr.x += d.x;
            curr.y += d.y;
            steps += 1;
            const VisitedPoint *point = hmgetp_null(visited, curr);
            if (point != NULL) {
                long dist = labs(curr.x) + labs(curr.y);
                long step_dist = point->value + steps;
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

    hmfree(visited);
}

static void parse_wire(char *line, WireSegment **wire) {
    char *p = line;
    while (*p != '\0' && *p != '\n') {
        WireSegment seg;
        seg.dir = *p++;
        seg.len = strtol(p, &p, 10);
        arrput(*wire, seg);

        if (*p == ',') {
            p++;
        }
    }
}

static int parse_file(FILE *f, WireSegment **wire1, WireSegment **wire2) {
    char line[4000];
    if (fgets(line, sizeof(line), f) == NULL) {
        return 1;
    }
    parse_wire(line, wire1);
    if (fgets(line, sizeof(line), f) == NULL) {
        return 1;
    }
    parse_wire(line, wire2);

    return 0;
}
