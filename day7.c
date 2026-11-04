#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_file(FILE *f, long **program);
static int solve(long *program);
// static void print_program(long *program);

typedef enum {
    POSITIONAL,
    IMMEDIATE,
} ParameterMode;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "rb");
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
        // print_program(program);
        status = solve(program);
    }



    arrfree(program);

    return status;
}

// static void print_program(long *program) {
//     for (long i = 0; i < arrlen(program); ++i ) {
//         if (i != 0) printf(",");
//         printf("%ld", program[i]);
//     }
//     printf("\n");
// }
//
static int parse_file(FILE *f, long **program) {
    size_t capacity = 1024 * 1024;
    char *buffer = malloc(capacity);
    fread(buffer, 1, capacity, f);

    char *p = buffer;
    while (true) {
        long n = strtol(p, &p, 10);
        arrput(*program, n);
        if (*p == ',') {
            p++;
        } else {
            goto cleanup;
        }
    }
cleanup:
    free(buffer);
    return 0;
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

static long get_signal(long *program, long phase_settings[5]) {
    long *input = NULL;
    long *output = NULL;
    long signal = 0;

    for (long i = 0; i < 5; ++i) {
        arrput(input, phase_settings[i]);
        arrput(input, signal);
        int status = evaluate(program, input, &output);
        assert(status == EXIT_SUCCESS);
        assert(arrlen(output) == 1);
        signal = output[0];
        arrfree(input);
        arrfree(output);
    }
    return signal;
}

void swap(long *a, long *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void permutations( long *program, long *max_signal, long nums[5], long start)
{
    if (start == 5) {
        long signal = get_signal(program, nums);
        if (signal > *max_signal) {
            *max_signal = signal;
        }
    }

    for (long i = start; i < 5; i++) {
        swap(&nums[start], &nums[i]);
        permutations(program, max_signal, nums, start + 1);
        swap(&nums[start], &nums[i]);
    }
}

static int solve(long *program) {
    long max_signal = 0;
    long phase_setting[5] = {0, 1, 2, 3, 4};
    permutations(program,  &max_signal, phase_setting, 0);
    printf("%ld\n", max_signal);

    return 0;
}
