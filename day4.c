#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <limits.h>
#include <stdio.h>
#include <stddef.h>

static int parse_file(FILE *f, long *start, long *end);
static void solve(long start, long end);

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

    long start, end;
    int parse_error = parse_file(f, &start, &end);
    if (!parse_error) {
        solve(start, end);
    } else {
        fprintf(stderr, "Failed to parse input\n");
    }

    if (ferror(f)) {
        perror("read error");
    }

    fclose(f);
    return 0;
}

static bool is_valid(long n) {
    // printf("number: %ld\n", n);

    bool has_repeat = false;
    bool is_increasing = true;

    while (n >= 10) {
        long a = n % 10;
        long b = (n/10) % 10;
        if (a == b) {
            has_repeat = true;
        }
        if (a < b) {
            is_increasing = false;
        }
        n /= 10;
    }

    // printf("has_repeat: %d is_increasing: %d\n", has_repeat, is_increasing);

    return has_repeat && is_increasing;
}


static void solve(long start, long end) {
    long count = 0;
    for (long i = start; i <= end; i++) {
        if(is_valid(i)) {
            count++;
        }
    }
    printf("%ld\n", count);
}

static int parse_file(FILE *f, long *start, long *end) {
    int read = fscanf(f, "%ld-%ld", start, end);
    return read != 2;
}
