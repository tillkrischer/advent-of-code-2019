#include <string.h>
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    char *key;
    char *value;
} Orbit;

static void parse_file(FILE *f, Orbit **orbits);
static void solve(Orbit *orbits);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r");
    assert(f != NULL);

    Orbit *orbits = NULL;
    sh_new_strdup(orbits);
    parse_file(f, &orbits);

    assert(!ferror(f));
    assert(fclose(f) != EOF);

    solve(orbits);
    return 0;
}

static void parse_file(FILE *f, Orbit **orbits) {
    char line[100];
    while (fgets(line, sizeof(line), f) != NULL) {
        char *parent, *child;
        parent = strtok(line, ")\r\n");
        child = strtok(NULL, ")\r\n");
        assert(child != NULL);
        char *parent_dup = strdup(parent);
        assert(parent_dup != NULL);
        free(shget(*orbits, child));
        shput(*orbits, child, parent_dup);
    }
}


static void solve(Orbit *orbits) {
    long count = 0;
    for (ptrdiff_t i = 0; i < shlen(orbits); ++i) {
        char *obj = orbits[i].value;
        assert(obj != NULL);
        while (strcmp(obj, "COM")) {
            obj = shget(orbits, obj);
            assert(obj != NULL);
            count++;
        }
        count++;
    }
    printf("%ld\n", count);
}
