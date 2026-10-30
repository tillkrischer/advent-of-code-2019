#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_file(FILE *f, long **program);
static int solve(long *program);

typedef enum {
    POSITIONAL,
    IMMEDIATE,
} ParameterMode;

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

    long *program = NULL;
    int parse_error = parse_file(f, &program);
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
        status = solve(program);
    }
    arrfree(program);

    return status;
}

static int parse_file(FILE *f, long **program) {
    while (true) {
        long n;
        if (fscanf(f, "%ld", &n) != 1) {
            return 1;
        }
        arrput(*program, n);
        int c = fgetc(f);
        if (c == EOF || c == '\n') {
            return 0;
        }
        if (c != ',') {
            return 1;
        }
    }
}

static long get_operand(const long *program, long ip, int position) {
    long value = program[ip];
    for (long i = 0; i < position + 1; i++) {
        value /= 10;
    }
    ParameterMode mode = value % 10;
    if (mode == IMMEDIATE) {
        return program[ip + position];
    } else {
        return program[program[ip + position]];
    }
}

static int evaluate(const long *original_program, const long *input,
                    long **output) {
    ptrdiff_t count = arrlen(original_program);
    long *program = NULL;
    arrsetlen(program, count);
    memcpy(program, original_program, count * sizeof(*program));

    int status = EXIT_FAILURE;
    long ip = 0, input_p = 0;
    while (ip < count) {
        long value = program[ip];
        long opcode = value % 100;
        long op1, op2, addr;

        switch (opcode) {
        case 1:
            op1 = get_operand(program, ip, 1);
            op2 = get_operand(program, ip, 2);
            addr = program[ip + 3];
            program[addr] = op1 + op2;
            ip += 4;
            break;
        case 2:
            op1 = get_operand(program, ip, 1);
            op2 = get_operand(program, ip, 2);
            addr = program[ip + 3];
            program[addr] = op1 * op2;
            ip += 4;
            break;
        case 3:
            addr = program[ip + 1];
            program[addr] = input[input_p++];
            ip += 2;
            break;
        case 4:
            op1 = get_operand(program, ip, 1);
            arrput(*output, op1);
            ip += 2;
            break;
        case 5:
            op1 = get_operand(program, ip, 1);
            op2 = get_operand(program, ip, 2);
            if (op1 != 0) {
                ip = op2;
            } else {
                ip += 3;
            }
            break;
        case 6:
            op1 = get_operand(program, ip, 1);
            op2 = get_operand(program, ip, 2);
            if (op1 == 0) {
                ip = op2;
            } else {
                ip += 3;
            }
            break;
        case 7:
            op1 = get_operand(program, ip, 1);
            op2 = get_operand(program, ip, 2);
            addr = program[ip + 3];
            if (op1 < op2) {
                program[addr] = 1;
            } else {
                program[addr] = 0;
            }
            ip += 4;
            break;
        case 8:
            op1 = get_operand(program, ip, 1);
            op2 = get_operand(program, ip, 2);
            addr = program[ip + 3];
            if (op1 == op2) {
                program[addr] = 1;
            } else {
                program[addr] = 0;
            }
            ip += 4;
            break;
        case 99:
            status = EXIT_SUCCESS;
            goto cleanup;
        default:
            fprintf(stderr, "Unknown opcode %ld at position %ld\n", opcode, ip);
            goto cleanup;
        }
    }
    fprintf(stderr, "Program ended without opcode 99\n");

cleanup:
    arrfree(program);
    return status;
}

static int solve(long *program) {
    long *input = NULL;
    long *output = NULL;

    arrput(input, 1);
    int status = evaluate(program, input, &output);
    if (status != EXIT_SUCCESS) {
        goto cleanup;
    }
    if (arrlen(output) == 0) {
        fprintf(stderr, "Part 1 produced no output\n");
        status = EXIT_FAILURE;
        goto cleanup;
    }
    long part1 = output[arrlen(output) - 1];
    printf("%ld\n", part1);

    arrfree(input);
    arrfree(output);

    arrput(input, 5);
    status = evaluate(program, input, &output);
    if (status != EXIT_SUCCESS) {
        goto cleanup;
    }
    if (arrlen(output) == 0) {
        fprintf(stderr, "Part 2 produced no output\n");
        status = EXIT_FAILURE;
        goto cleanup;
    }
    long part2 = output[arrlen(output) - 1];
    printf("%ld\n", part2);

cleanup:
    arrfree(input);
    arrfree(output);
    return status;
}
