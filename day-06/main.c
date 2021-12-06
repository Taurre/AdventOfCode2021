#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SCHOOL_MAX 9

typedef uintmax_t school[SCHOOL_MAX];

static uintmax_t
simulate_days(school const begin, uintmax_t days) {
	assert(begin != NULL);
	school lanternfishes;

	memcpy(lanternfishes, begin, sizeof (school));

	for (size_t i = 0; i < days; i++) {
		school next;

		next[0] = lanternfishes[1];
		next[1] = lanternfishes[2];
		next[2] = lanternfishes[3];
		next[3] = lanternfishes[4];
		next[4] = lanternfishes[5];
		next[5] = lanternfishes[6];
		next[6] = lanternfishes[7];
		next[7] = lanternfishes[8];
		next[6] += lanternfishes[0];
		next[8] = lanternfishes[0];
		memcpy(lanternfishes, next, sizeof (school));
	}

	uintmax_t res = 0;

	for (size_t i = 0; i < SCHOOL_MAX; i++)
		res += lanternfishes[i];

	return res;
}

int
main(int argc, char **argv) {
	if (argc <= 1) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE *fp = fopen(argv[1], "r");

	if (fp == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	school lanternfishes = { 0 };
	unsigned n;

	while (fscanf(fp, "%u", &n) == 1) {
		lanternfishes[n]++;
		int c = getc(fp);

		if (c == EOF || c == '\n')
			break;
	}

	if (ferror(fp)) {
		perror("fscanf");
		exit(EXIT_FAILURE);
	}

	printf("Part 1: %ju\n", simulate_days(lanternfishes, 80));
	printf("Part 2: %ju\n", simulate_days(lanternfishes, 256));
	fclose(fp);
	return 0;
}
