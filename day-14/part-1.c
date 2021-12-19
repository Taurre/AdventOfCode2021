#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAIR_MAX 128

struct element {
	char name;
	struct element *next;
};

struct template {
	struct element *first;
	struct element *last;
};

struct pair {
	char pair[2];
	char to_insert;
};

struct minmax {
	uintmax_t min;
	uintmax_t max;
};

static void
template_append(struct template *template, char name) {
	struct element *element = malloc(sizeof *element);

	if (element == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	element->name = name;
	element->next = NULL;

	if (template->first == NULL) {
		template->first = element;
		template->last = element;
	}
	else {
		template->last->next = element;
		template->last = element;
	}
}

static void
template_print(struct template *template) {
	for (struct element *it = template->first; it != NULL; it = it->next)
		putchar(it->name);

	putchar('\n');
}

static char
pair_find_replacement(struct pair *pairs, size_t last, char *pair) {
	for (size_t i = 0; i < last; i++)
		if (memcmp(pairs[i].pair, pair, 2) == 0)
			return pairs[i].to_insert;

	assert("Cannot find replacement for a pair" == 0);
}

static void
template_pair_insertion(struct template *template, struct pair *pairs, size_t last) {
	struct element *element = template->first;

	while (element != NULL) {
		if (element->next == NULL)
			break;

		struct element *next = element->next;
		char to_insert = pair_find_replacement(pairs, last, (char[]) { element->name, next->name });
		struct element *new = malloc(sizeof *new);

		if (new == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}

		new->name = to_insert;
		new->next = next;
		element->next = new;
		element = next;
	}
}

static struct minmax
template_find_most_and_least_common(struct template *template) {
	struct {
		char name;
		size_t count;
	} stack[PAIR_MAX];
	size_t top = 0;

	for (struct element *it = template->first; it != NULL; it = it->next) {
		bool inc = false;

		for (size_t i = 0; i < top; i++) {
			if (stack[i].name == it->name) {
				stack[i].count++;
				inc = true;
				break;
			}
		}

		if (!inc) {
			stack[top].name = it->name;
			stack[top].count = 1;
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

	int c;
	struct template *template = &(struct template) { .first = NULL};

	while ((c = getc(fp)) != '\n' && c != EOF)
		template_append(template, c);

	if (ferror(fp)) {
		perror("getc");
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
		last++;
	}

	if (ferror(fp)) {
		perror("fscanf");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < 10; i++) {
		template_pair_insertion(template, pairs, last);
	}

	struct minmax minmax = template_find_most_and_least_common(template);
	printf("Part 1: %ju\n", minmax.max - minmax.min);
	fclose(fp);
	return 0;
}
