#include <limits.h>
#include <stdio.h>
#include "uthash.h"

struct string_entry {
    char key[20];
    UT_hash_handle hh;
};

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

int parse_file(FILE *f) {
    char dir, delim;
    long len, dx, dy, dist;
    long x = 0;
    long y = 0;
    long min_dist = LONG_MAX;
    struct string_entry *visited = NULL; 

    while (fscanf(f, "%c%ld", &dir, &len) == 2) {
        // printf("%c - %ld\n", dir, len);
        get_deltas(dir, &dx, &dy);
        for (long i = 0; i < len; i++) {
            x += dx;
            y += dy;
            visit(&visited, x, y);
        }

        delim = fgetc(f);
        if (delim != ',')
            break;
    }
    
    // print_visited(&visited);

    x = 0;
    y = 0;
    while (fscanf(f, "%c%ld", &dir, &len) == 2) {
        // printf("%c - %ld\n", dir, len);
        get_deltas(dir, &dx, &dy);
        for (long i = 0; i < len; i++) {
            x += dx;
            y += dy;
            if (is_visited(&visited, x, y)) {
                dist = labs(x) + labs(y);
                if (dist < min_dist) {
                    min_dist = dist;
                }
            }
        }

        delim = fgetc(f);
        if (delim != ',')
            break;
    }

    printf("%ld\n", min_dist);

    return 0;
}
