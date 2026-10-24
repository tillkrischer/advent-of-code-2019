#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

static int parse_file(FILE *f, long *start, long *end);
static void solve(long start, long end);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    long start, end;
    int parse_error = parse_file(f, &start, &end);
    int status = EXIT_SUCCESS;

    if (ferror(f)) {
        perror("read error");
        status = EXIT_FAILURE;
    } else if (parse_error) {
        fprintf(stderr, "Failed to parse input\n");
        status = EXIT_FAILURE;
    }

    if (fclose(f) == EOF) {
        perror("close error");
        status = EXIT_FAILURE;
    }

    if (status == EXIT_SUCCESS) {
        solve(start, end);
    }

    return status;
}

static void evaluate(long n, bool *has_two_repeat, bool *has_any_repeat, bool *is_increasing) {
    *is_increasing = true;
    *has_any_repeat = false;
    *has_two_repeat = false;
    long curr_repeat_length = 1;
    long a = n % 10;
    n /= 10;
    long b = n % 10;
    while (n > 0) {
        if (a == b) {
            *has_any_repeat = true;
            curr_repeat_length++;
        } else {
            if (curr_repeat_length == 2) {
                *has_two_repeat = true;
            }
            curr_repeat_length = 1;
        }
        if (a < b) {
            *is_increasing = false;
        }
        n /= 10;
        a = b;
        b = n % 10;
    }
    if (curr_repeat_length == 2) {
        *has_two_repeat = true;
    }
}

static void solve(long start, long end) {
    long part1 = 0, part2 = 0;
    for (long i = start; i <= end; i++) {
        bool has_two_repeat, has_any_repeat, is_increasing;
        evaluate(i, &has_two_repeat, &has_any_repeat, &is_increasing);
        if (is_increasing && has_any_repeat) {
            part1++;
        }
        if (is_increasing && has_two_repeat) {
            part2++;
        }
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);
}

static int parse_file(FILE *f, long *start, long *end) {
    int read = fscanf(f, "%ld-%ld", start, end);
    return read != 2;
}
