#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    parse_file(f);

    if (ferror(f)) {
        perror("read error");
    }

    fclose(f);
    return 0;
}

int parse_program(char *s, long *program, long *program_size) {
    char *token = strtok(s, ",");
    long val;
    while (token != NULL) {
        val = strtol(token, NULL, 10);
        program[*program_size] = val;
        *program_size += 1;
        token = strtok(NULL, ",");
    }

    // printf("%lu\n", *program_size);

    return 0;
}

void print_program(long *program, long program_size) {
    for (long i = 0; i < program_size; i++) {
        if (i != 0) putchar(',');
        printf("%lu", program[i]);
    }
    putchar('\n');
}

long evaluate(long *program, long program_size) {
    long i = 0;
    while (i < program_size && program[i] != 99) {
        if (program[i] == 1) {
            long op1 = program[i + 1];
            long op2 = program[i + 2];
            long dest = program[i + 3];
            program[dest] = program[op1] + program[op2];
            i += 4;
        } else if (program[i] == 2) {
            long op1 = program[i + 1];
            long op2 = program[i + 2];
            long dest = program[i + 3];
            program[dest] = program[op1] * program[op2];
            i += 4;
        } else {
            i += 1;
        }
    }

    // print_program(program, program_size);

    return program[0];
}

void restore_program(long *program) {
    program[1] = 12;
    program[2] = 2;
}

int parse_file(FILE *f) {
    char line[4000];
    long program[200];
    long orig_program[200];
    long program_size = 0;
    long res;

    while (fgets(line, sizeof(line), f) != NULL) {
        parse_program(line, orig_program, &program_size);
        memcpy(program, orig_program, sizeof(long) * program_size);
        restore_program(program);
        res = evaluate(program, program_size);
        printf("%lu\n", res);
        for(long i = 0; i <= 100; i++) {
            for(long j = 0; j <= 100; j++) {
                memcpy(program, orig_program, sizeof(long) * program_size);
                program[1] = i;
                program[2] = j;
                res = evaluate(program, program_size);
                if (res == 19690720) {
                    // printf("%lu %lu: %lu\n", i, j, res);
                    long part2 = 100 * i + j;
                    printf("%lu\n", part2);
                }
            }
        }

        program_size = 0;
    }


    return 0;
}
