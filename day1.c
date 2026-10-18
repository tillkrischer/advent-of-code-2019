#include <stdio.h>
#include <stdlib.h>

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

void parse_file(FILE *f) {
    char line[4000];
    long m = 0;
    long sum = 0;
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%lu", &m);
        sum += m / 3 - 2;
    }
    printf("%lu\n", sum);
}
