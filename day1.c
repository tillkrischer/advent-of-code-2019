#include <stdio.h>

void parse_file(FILE *f);

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

long fuel_req(long mass) {
    long sum = 0;
    mass = mass / 3 - 2;
    while (mass > 0) {
        sum += mass;
        mass = mass / 3 - 2;
    }
    return sum;
}

void parse_file(FILE *f) {
    char line[4000];
    long m = 0;
    long sum1 = 0;
    long sum2 = 0;
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%lu", &m);\
        sum1 += m / 3 - 2;
        sum2 += fuel_req(m);
    }
    printf("%lu\n", sum1);
    printf("%lu\n", sum2);
}
