#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "uthash.h"
#include "utarray.h"

struct string_entry {
    char key[20];
    UT_hash_handle hh;
};

struct wire_segment {
    char dir;
    long len;
};

UT_array *wire1;
UT_array *wire2;

int parse_file(FILE *f);

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

    if (parse_file(f)) {
        perror("parse error");
        return 1;
    }

    if (ferror(f)) {
        perror("read error");
    }

    fclose(f);
    return 0;
}

int visit(struct string_entry **visited, long x, long y) {
    struct string_entry *s;
    char key[20];
    sprintf(key, "%ld,%ld", x, y);
    HASH_FIND_STR(*visited, key, s);
    if (s == NULL) {
        s = malloc(sizeof *s);
        if (!s)
            return 1;
        sprintf(s->key, "%ld,%ld", x, y);
        HASH_ADD_STR(*visited, key, s);
    }
    return 0;
}

bool is_visited(struct string_entry **visited, long x, long y) {
    struct string_entry *s;
    char key[20];
    sprintf(key, "%ld,%ld", x, y);
    HASH_FIND_STR(*visited, key, s);
    return s != NULL;
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
        printf("%s\n", s->key);
    }
}

int parse_wire(char *line, UT_array *wire) {
    struct wire_segment seg; 
    UT_icd wire_segement_icd = {sizeof(struct wire_segment), NULL, NULL, NULL};
    utarray_new(wire, &wire_segement_icd);
    char *token = strtok(line, ",");
    while (token != NULL) {
        seg.dir = token[0];
        seg.len = strtol(token+1, NULL, 10);
        utarray_push_back(wire1, &seg);
        token = strtok(NULL, ",");
    }
    return 0;
}

int parse_file(FILE *f) {
    char delim;
    long dx, dy, dist;
    long x = 0;
    long y = 0;
    long min_dist = LONG_MAX;
    struct string_entry *visited = NULL; 
    UT_icd wire_segement_icd = {sizeof(struct wire_segment), NULL, NULL, NULL};
    struct wire_segment seg, *p; 
    char line[4000];
    // utarray_new(wire1, &wire_segement_icd);
    // while (fscanf(f, "%c%ld", &seg.dir, &seg.len) == 2) {
    //     utarray_push_back(wire1, &seg);
    //
    //     delim = fgetc(f);
    //     if (delim != ',')
    //         break;
    // }
    //
    // utarray_new(wire2, &wire_segement_icd);
    // while (fscanf(f, "%c%ld", &seg.dir, &seg.len) == 2) {
    //     utarray_push_back(wire2, &seg);
    //
    //     delim = fgetc(f);
    //     if (delim != ',')
    //         break;
    // }
    
    if (fgets(line, sizeof(line), f) == NULL) return 1;
    parse_wire(line, wire1);
    // if (fgets(line, sizeof(line), f) == NULL) return 1;
    // parse_wire(line, wire2);
    //
    // for (p = (struct wire_segment*)utarray_front(wire1); p != NULL; p = utarray_next(wire1, p)) {
    //     printf("%c - %ld\n", p->dir, p->len);
    // }
    
    // print_visited(&visited);

    // x = 0;
    // y = 0;
    // while (fscanf(f, "%c%ld", &dir, &len) == 2) {
    //     // printf("%c - %ld\n", dir, len);
    //
    //
    //
    //
    //
    //     // get_deltas(dir, &dx, &dy);
    //     // for (long i = 0; i < len; i++) {
    //     //     x += dx;
    //     //     y += dy;
    //     //     if (is_visited(&visited, x, y)) {
    //     //         dist = labs(x) + labs(y);
    //     //         if (dist < min_dist) {
    //     //             min_dist = dist;
    //     //         }
    //     //     }
    //     // }
    //
    //     delim = fgetc(f);
    //     if (delim != ',')
    //         break;
    // }

    // printf("%ld\n", min_dist);

    return 0;
}
