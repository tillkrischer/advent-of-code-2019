#include <iso646.h>
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
} Parent;

typedef struct {
    char *key;
    long value;
} Distance;

static void parse_file(FILE *f, Parent **parents);
static void solve(Parent *parents);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r");
    assert(f != NULL);

    Parent *parents = NULL;
    sh_new_strdup(parents);

    parse_file(f, &parents);

    assert(!ferror(f));
    assert(fclose(f) != EOF);

    solve(parents);

    for (ptrdiff_t i = 0; i < shlen(parents); i++) {
        free(parents[i].value);
    }
    shfree(parents);

    return 0;
}

static char *get_common_parent(Parent *parents, char *a, char *b) {
    Distance *visited = NULL;
    sh_new_strdup(visited);

    while (strcmp(a, "COM")) {
        shput(visited, a, 1);
        a = shget(parents, a);
        assert(a != NULL);
    }

    while (strcmp(b, "COM") != 0 && shgeti(visited, b) == -1) {
        b = shget(parents, b);
        assert(b != NULL);
    }

    shfree(visited);
    return b;
}

static long get_dist(Parent *parents, Distance **distances, char *obj) {
    ptrdiff_t cached_index = shgeti(*distances, obj);
    if (cached_index >= 0) {
        return (*distances)[cached_index].value;
    }
    char *parent = shget(parents, obj);
    assert(parent != NULL);
    long dist = get_dist(parents, distances, parent) + 1;
    shput(*distances, obj, dist);
    return dist;
}

static void solve(Parent *parents) {
    Distance *distances= NULL;
    sh_new_strdup(distances);
    shput(distances, "COM", 0);

    long part1 = 0;
    for (ptrdiff_t i = 0; i < shlen(parents); ++i) {
        part1 += get_dist(parents, &distances, parents[i].key);
    }

    char *common = get_common_parent(parents, "YOU", "SAN");
    long you_dist = shget(distances, "YOU") - 1;
    long santa_dist = shget(distances, "SAN") - 1;
    long common_dist = shget(distances, common);
    long part2 = you_dist + santa_dist - 2 * common_dist;

    printf("%ld\n", part1);
    printf("%ld\n", part2);

    shfree(distances);
}

static void parse_file(FILE *f, Parent **parents) {
    char line[100];
    while (fgets(line, sizeof(line), f) != NULL) {
        char *parent, *child;
        parent = strtok(line, ")\r\n");
        child = strtok(NULL, ")\r\n");
        assert(child != NULL);
        char *parent_dup = strdup(parent);
        assert(parent_dup != NULL);
        shput(*parents, child, parent_dup);
    }
}
