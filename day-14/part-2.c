#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAIR_MAX 128

struct pair {
	char pair[2];
	char to_insert;
	uintmax_t count;
};

struct minmax {
	uintmax_t min;
	uintmax_t max;
};

static struct pair *
pair_find(struct pair *pairs, size_t last, char *pair) {
	for (size_t i = 0; i < last; i++)
		if (memcmp(pairs[i].pair, pair, 2) == 0)
			return pairs + i;

	assert("Cannot find pair" == 0);
}

static void
pair_insertion(struct pair *pairs, size_t last) {
	struct pair *new = malloc(sizeof *new * last);

	if (new == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	memcpy(new, pairs, sizeof *pairs * last);

	for (size_t i = 0; i < last; i++) {
		new[i].count = 0;
	}

	for (size_t i = 0; i < last; i++) {
		struct pair *to_insert;

		to_insert = pair_find(new, last, (char[]) { pairs[i].pair[0], pairs[i].to_insert });
		to_insert->count += pairs[i].count;
		to_insert = pair_find(new, last, (char[]) { pairs[i].to_insert, pairs[i].pair[1] });
		to_insert->count += pairs[i].count;
	}

	memcpy(pairs, new, sizeof *new * last);
	free(new);
}

static void
pair_print(struct pair *pairs, size_t last) {
	for (size_t i = 0; i < last; i++)
		printf("%c%c: %ju\n", pairs[i].pair[0], pairs[i].pair[1], pairs[i].count);
}

static struct minmax
pair_find_minmax(struct pair *pairs, size_t last) {
	struct {
		char name;
		size_t count;
	} stack[PAIR_MAX];
	size_t top = 0;

	stack[top].name = pairs[0].pair[0];
	stack[top].count = pairs[0].count;
	top++;

	for (size_t i = 0; i < last; i++) {
		bool found = false;

		for (size_t j = 0; j < top; j++) {
			if (stack[j].name == pairs[i].pair[1]) {
				found = true;
				stack[j].count += pairs[i].count;
				break;
			}
		}

		if (!found) {
			stack[top].name = pairs[i].pair[1];
			stack[top].count = pairs[i].count;
			top++;
		}
	}

	struct minmax minmax = { .min = UINTMAX_MAX, .max = 0 };

	for (size_t i = 0; i < top; i++) {
		if (stack[i].count < minmax.min)
			minmax.min = stack[i].count;	
		if (minmax.max < stack[i].count)
			minmax.max = stack[i].count;
	}

	return minmax;
}

int
main(int argc, char **argv) {
	if (argc <= 1) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE *fp = fopen(argv[1], "r");

	if (fp == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	char line[255];

	if (fgets(line, sizeof line, fp) == NULL) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}

	struct pair pairs[PAIR_MAX];
	size_t last = 0;
	char pair[3];
	char to_insert[2];

	while (fscanf(fp, "%2s -> %1s", pair, to_insert) == 2) {
		pairs[last].pair[0] = pair[0];
		pairs[last].pair[1] = pair[1];
		pairs[last].to_insert = to_insert[0];
		pairs[last].count = 0;
		last++;
	}

	if (ferror(fp)) {
		perror("fscanf");
		exit(EXIT_FAILURE);
	}
	
	for (size_t i = 0; line[i] != '\n' && line[i] != '\0'; i++) {
		if (line[i + 1] == '\n' || line[i + 1] == '\0')
			break;

		struct pair *pair = pair_find(pairs, last, (char[]) { line[i], line[i + 1] });
		pair->count++;
	}

	for (size_t i = 0; i < 40; i++)
		pair_insertion(pairs, last);

	struct minmax minmax = pair_find_minmax(pairs, last);
	printf("Part 2: %ju\n", minmax.max - minmax.min);
	fclose(fp);
	return 0;
}
