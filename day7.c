#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef enum { INITIALIZED, RUNNING, HALTED, WAITING_FOR_INPUT } ProgramState;

typedef struct {
    long *instructions;
    long *inputs;
    long *outputs;
    ProgramState state;
    long ip;
} Program;

static int parse_file(FILE *f, long **program);
static void solve(long *program);

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
        solve(program);
    }

    arrfree(program);
    return status;
}

static int parse_file(FILE *f, long **program) {
    size_t capacity = 1024 * 1024;
    char *buffer = malloc(capacity);
    size_t length = fread(buffer, 1, capacity, f);
    int status = 1;
    char *p = buffer;
    while (true) {
        char *end;
        long n = strtol(p, &end, 10);
        if (end == p) {
            goto cleanup;
        }
        arrput(*program, n);
        p = end;
        while (isspace(*p)) {
            p++;
        }
        if (p == buffer + length) {
            status = 0;
            goto cleanup;
        }
        if (*p != ',') {
            goto cleanup;
        }
        p++;
    }
cleanup:
    free(buffer);
    return status;
}

static long get_operand(const long *program, long ip, int position) {
    long value = program[ip];
    for (long i = 0; i < position + 1; i++) {
        value /= 10;
    }
    bool immediate = value % 10;
    if (immediate) {
        return program[ip + position];
    } else {
        return program[program[ip + position]];
    }
}

static int evaluate_program(Program *program) {
    long count = arrlen(program->instructions);
    program->state = RUNNING;
    while (program->ip < count) {
        long value = program->instructions[program->ip];
        long opcode = value % 100;
        long op1, op2, addr;

        switch (opcode) {
        case 1:
            op1 = get_operand(program->instructions, program->ip, 1);
            op2 = get_operand(program->instructions, program->ip, 2);
            addr = program->instructions[program->ip + 3];
            program->instructions[addr] = op1 + op2;
            program->ip += 4;
            break;
        case 2:
            op1 = get_operand(program->instructions, program->ip, 1);
            op2 = get_operand(program->instructions, program->ip, 2);
            addr = program->instructions[program->ip + 3];
            program->instructions[addr] = op1 * op2;
            program->ip += 4;
            break;
        case 3:
            if (arrlen(program->inputs) == 0) {
                program->state = WAITING_FOR_INPUT;
                return EXIT_SUCCESS;
            }
            addr = program->instructions[program->ip + 1];
            program->instructions[addr] = program->inputs[0];
            arrdel(program->inputs, 0);
            program->ip += 2;
            break;
        case 4:
            op1 = get_operand(program->instructions, program->ip, 1);
            arrput(program->outputs, op1);
            program->ip += 2;
            break;
        case 5:
            op1 = get_operand(program->instructions, program->ip, 1);
            op2 = get_operand(program->instructions, program->ip, 2);
            if (op1 != 0) {
                program->ip = op2;
            } else {
                program->ip += 3;
            }
            break;
        case 6:
            op1 = get_operand(program->instructions, program->ip, 1);
            op2 = get_operand(program->instructions, program->ip, 2);
            if (op1 == 0) {
                program->ip = op2;
            } else {
                program->ip += 3;
            }
            break;
        case 7:
            op1 = get_operand(program->instructions, program->ip, 1);
            op2 = get_operand(program->instructions, program->ip, 2);
            addr = program->instructions[program->ip + 3];
            if (op1 < op2) {
                program->instructions[addr] = 1;
            } else {
                program->instructions[addr] = 0;
            }
            program->ip += 4;
            break;
        case 8:
            op1 = get_operand(program->instructions, program->ip, 1);
            op2 = get_operand(program->instructions, program->ip, 2);
            addr = program->instructions[program->ip + 3];
            if (op1 == op2) {
                program->instructions[addr] = 1;
            } else {
                program->instructions[addr] = 0;
            }
            program->ip += 4;
            break;
        case 99:
            program->state = HALTED;
            return EXIT_SUCCESS;
        default:
            fprintf(stderr, "Unknown opcode %ld at position %ld\n", opcode,
                    program->ip);
            return EXIT_FAILURE;
        }
    }
    fprintf(stderr, "Program ended without opcode 99\n");
    return EXIT_FAILURE;
}

static Program init_program(long *original_instructions) {
    Program new_program = {0};
    ptrdiff_t count = arrlen(original_instructions);
    arrsetlen(new_program.instructions, count);
    memcpy(new_program.instructions, original_instructions,
           count * sizeof(*new_program.instructions));

    return new_program;
}

static void free_program(Program program) {
    arrfree(program.inputs);
    arrfree(program.outputs);
    arrfree(program.instructions);
}

static long get_signal(long *program, long phase_settings[5]) {
    Program programs[5];
    for (long i = 0; i < 5; ++i) {
        programs[i] = init_program(program);
        arrput(programs[i].inputs, phase_settings[i]);
    }

    long signal = 0;
    long i = 0;
    while (true) {
        assert(programs[i].state != HALTED);
        arrput(programs[i].inputs, signal);
        int status = evaluate_program(&programs[i]);
        assert(status == EXIT_SUCCESS);
        assert(arrlen(programs[i].outputs) == 1);
        signal = programs[i].outputs[0];
        arrpop(programs[i].outputs);

        if (i == 4 && programs[4].state == HALTED) {
            break;
        }
        i = (i + 1) % 5;
    }

    for (long i = 0; i < 5; ++i) {
        free_program(programs[i]);
    }
    return signal;
}

static void swap(long *a, long *b) {
    long tmp = *a;
    *a = *b;
    *b = tmp;
}

static void permutations(long *program, long *max_signal, long nums[5],
                         long start) {
    if (start == 5) {
        long signal = get_signal(program, nums);
        if (signal > *max_signal) {
            *max_signal = signal;
        }
        return;
    }

    for (long i = start; i < 5; i++) {
        swap(&nums[start], &nums[i]);
        permutations(program, max_signal, nums, start + 1);
        swap(&nums[start], &nums[i]);
    }
}

static void solve(long *program) {
    long part1 = LONG_MIN;
    long part2 = LONG_MIN;
    long phase_setting1[5] = {0, 1, 2, 3, 4};
    long phase_setting2[5] = {5, 6, 7, 8, 9};
    permutations(program, &part1, phase_setting1, 0);
    permutations(program, &part2, phase_setting2, 0);

    printf("%ld\n", part1);
    printf("%ld\n", part2);
}
