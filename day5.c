#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

static int parse_file(FILE *f, long **program);
static void print_program(const long *program);
static int solve(long **program);

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
        // print_program(program);
        solve(&program);
        // print_program(program);
    }
    arrfree(program);

    return status;
}

static void print_program(const long *program) {
    for (ptrdiff_t i = 0; i < arrlen(program); i++) {
        if (i != 0) {
            putchar(',');
        }
        printf("%ld", program[i]);
    }
    putchar('\n');
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
    return 0;
}

static long get_operand(const long *program, long ip, int position) {
    long value = program[ip];
    for (long i = 0; i < position + 1; i++) {
        value /= 10;
    }
    ParameterMode mode = value % 10;
    // printf("pos %d mode: %d\n", position, mode);
    if (mode == IMMEDIATE) {
        return program[ip+position];
    } else {
        return program[program[ip+position]];
    }
}

static int solve(long **program) {
   long ip = 0;
   while (ip < arrlen(*program)) {
       // printf("========== ip: %ld\n", ip);
       long value = (*program)[ip];
       long opcode = value % 100;
       long op1, op2, addr;

       switch (opcode) {
       case 1:
           op1 = get_operand(*program, ip, 1);
           op2 = get_operand(*program, ip, 2);
           addr = (*program)[ip+3];
           // printf("add %ld + %ld\n", op1, op2);
           (*program)[addr] = op1 + op2;
           ip += 4;
           break;
       case 2:
           op1 = get_operand(*program, ip, 1);
           op2 = get_operand(*program, ip, 2);
           addr = (*program)[ip+3];
           // printf("mul %ld * %ld\n", op1, op2);
           (*program)[addr] = op1 * op2;
           ip += 4;
           break;
       case 3:
           addr = (*program)[ip+1];
           printf("input: 1\n");
           (*program)[addr] = 1;
           ip += 2;
           break;
       case 4:
           op1 = get_operand(*program, ip, 1);
           printf("output: %ld\n", op1);
           ip += 2;
           break;
       case 99:
           printf("halt\n");
           return 0;
       default:
           printf("unknown op %ld\n", opcode);
           ip += 1;
           break;

       }
   }
   return 0;
}
