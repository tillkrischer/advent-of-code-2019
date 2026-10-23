#include <limits.h>
#include <stdio.h>
#include "uthash.h"
#include "utarray.h"

struct string_entry {
    char key[20];
    long steps;
    UT_hash_handle hh;
};

struct wire_segment {
    char dir;
    long len;
};
UT_icd wire_segement_icd = {sizeof(struct wire_segment), NULL, NULL, NULL};
UT_array *wire1;
UT_array *wire2;

int parse_file(FILE *f);
void solve();
void get_deltas(char dir, long *dx, long *dy);
int visit(struct string_entry **visited, long x, long y, long dist);
void print_visited(struct string_entry **visited);
long is_visited(struct string_entry **visited, long x, long y);

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

    int parse_error = parse_file(f);
    if (!parse_error) {
        solve();
    } else {
        perror("parse error");
    }

    if (ferror(f)) {
        perror("read error");
    }

    fclose(f);
    return 0;
}

void solve() {
    struct string_entry *visited = NULL; 
    long steps = 0;
    long x = 0, y = 0;
    long dx, dy;
    long other_steps;
    struct wire_segment *p; 
    long dist, step_dist;
    long part1 = LONG_MAX;
    long part2 = LONG_MAX;

    for (p = (struct wire_segment*)utarray_front(wire1); p != NULL; p = utarray_next(wire1, p)) {
        get_deltas(p->dir, &dx, &dy) ;
        for (long i = 0; i < p->len; i++) {
            x += dx;
            y += dy;
            steps += 1;
            visit(&visited, x, y, steps);
        }
    }

    steps = 0;
    x = 0, y = 0;
    for (p = (struct wire_segment*)utarray_front(wire2); p != NULL; p = utarray_next(wire2, p)) {
        get_deltas(p->dir, &dx, &dy) ;
        for (long i = 0; i < p->len; i++) {
            x += dx;
            y += dy;
            steps += 1;
            other_steps = is_visited(&visited, x, y);
            if (other_steps > 0) {
                dist = labs(x) + labs(y);
                step_dist = steps + other_steps;
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

    // print_visited(&visited);
}

int visit(struct string_entry **visited, long x, long y, long steps) {
    struct string_entry *s;
    char key[20];
    sprintf(key, "%ld,%ld", x, y);
    HASH_FIND_STR(*visited, key, s);
    if (s == NULL) {
        s = malloc(sizeof *s);
        if (!s)
            return 1;
        sprintf(s->key, "%ld,%ld", x, y);
        s->steps = steps;
        HASH_ADD_STR(*visited, key, s);
    }
    return 0;
}

long is_visited(struct string_entry **visited, long x, long y) {
    struct string_entry *s;
    char key[20];
    sprintf(key, "%ld,%ld", x, y);
    HASH_FIND_STR(*visited, key, s);
    if (s == NULL) 
        return 0;
    return s->steps;
}

void get_deltas(char dir, long *dx, long *dy) {
    *dx = 0;
    *dy = 0;
    if (dir == 'R')
        *dx = 1;
    else if (dir == 'L')
        *dx = -1;
    else if (dir == 'U')
        *dy = -1;
    else if (dir == 'D')
        *dy = 1;
}

void print_visited(struct string_entry **visited) {
    struct string_entry *s;
    for (s = *visited; s != NULL; s = s->hh.next) {
        printf("%s: %ld\n", s->key, s->steps);
    }
}

int parse_wire(char *line, UT_array **wire) {
    struct wire_segment seg; 
    utarray_new(*wire, &wire_segement_icd);
    char *token = strtok(line, ",");
    while (token != NULL) {
        seg.dir = token[0];
        seg.len = strtol(token+1, NULL, 10);
        utarray_push_back(*wire, &seg);
        token = strtok(NULL, ",");
    }
    return 0;
}

int parse_file(FILE *f) {
    char line[4000];

    if (fgets(line, sizeof(line), f) == NULL)
        return 1;
    parse_wire(line, &wire1);
    if (fgets(line, sizeof(line), f) == NULL)
        return 1;
    parse_wire(line, &wire2);

    return 0;
}
