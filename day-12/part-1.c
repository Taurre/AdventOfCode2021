#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_MAX 32 
#define NAME_MAX 8
#define LINKS_MAX 8

struct cave {
	char name[NAME_MAX];
	char links[LINKS_MAX][NAME_MAX];
	size_t len;
	bool visited;
};

static struct cave *cave_find(struct cave *, size_t, char const *);

uintmax_t
explore(char const *from, struct cave *caves, size_t len) {
	if (strcmp(from, "end") == 0)
		return 1;

	uintmax_t count = 0;
	struct cave *current = cave_find(caves, len, from);
	current->visited = true;

	for (size_t i = 0; i < current->len; i++) {
		struct cave *next = cave_find(caves, len, current->links[i]);

		if (islower(next->name[0]) && next->visited)
			continue;
		if (strcmp(next->name, "start") == 0)
			continue;

		struct cave *copy = malloc(sizeof *copy * len);

		if (copy == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}

		memcpy(copy, caves, sizeof *copy * len);
		count += explore(next->name, copy, len);		
		free(copy);
	}

	return count;
}

static void
cave_add_link(struct cave *cave, char const *link) {
	assert(cave != NULL);
	assert(link != NULL);

	for (size_t i = 0; i < cave->len; i++)
		if (strcmp(cave->links[i], link) == 0)
			return;

	strcpy(cave->links[cave->len], link);
	cave->len++;
}

static struct cave *
cave_find(struct cave *caves, size_t len, char const *name) {
	for (size_t i = 0; i < len; i++)
		if (strcmp(caves[i].name, name) == 0)
			return caves + i;

	return NULL;
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
	struct cave caves[STACK_MAX] = { { .len = 0 } };
	size_t len = 0;

	while (fgets(line, sizeof line, fp) != NULL) {
		char end[2][NAME_MAX];

		if (sscanf(line, "%7[^-]-%7s", end[0], end[1]) != 2) {
			perror("sscanf");
			exit(EXIT_FAILURE);
		}

		for (size_t i = 0; i < 2; i++) {
			struct cave *cave = cave_find(caves, len, end[i]);

			if (cave == NULL) {
				cave = &caves[len];
				strcpy(cave->name, end[i]);
				len++;
			}

			if (i == 0)
				cave_add_link(cave, end[1]);
			else
				cave_add_link(cave, end[0]);
		}
	}

	if (ferror(fp)) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < len; i++) {
		printf("Cave: %s is linked to: ", caves[i].name);

		for (size_t j = 0; j < caves[i].len; j++)
			printf("%s ", caves[i].links[j]);

		putchar('\n');
	}

	printf("%ju\n", explore("start", caves, len));
	fclose(fp);
	return 0;
}
