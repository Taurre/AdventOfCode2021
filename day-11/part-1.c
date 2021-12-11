#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLS 12
#define ROWS 12

#define STEPS 100

#define LEN(a) ((sizeof (a)) / sizeof ((a)[0]))

struct adjacent {
	size_t i;
	size_t j;
};

struct octopus {
	int8_t energy;
	bool flashed;
};

static uintmax_t
flash(struct octopus (*octopus)[ROWS], size_t i, size_t j) {
	assert(octopus != NULL);
	uintmax_t flashes = 1;

	struct adjacent adjacents[] = {
		{ i + 1, j },
		{ i, j + 1 },
		{ i - 1, j },
		{ i, j - 1 },
		{ i + 1, j + 1 },
		{ i - 1, j - 1 },
		{ i + 1, j - 1 },
		{ i - 1, j + 1 },
	};		

	for (size_t k = 0; k < LEN(adjacents); k++) {
		struct octopus *adjacent = &octopus[adjacents[k].i][adjacents[k].j];

		if (adjacent->energy >= 0 && adjacent->flashed == false) {
			adjacent->energy += 1;

			if (adjacent->energy > 9) {
				adjacent->energy = 0;
				adjacent->flashed = true;
				flashes += flash(octopus, adjacents[k].i, adjacents[k].j);
			}
		}
	}

	return flashes;
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

	struct octopus octopus[COLS][ROWS];

	for (size_t i = 0; i < COLS; i++)
		for (size_t j = 0; j < ROWS; j++) {
			octopus[i][j].energy = -1;
			octopus[i][j].flashed = false;
		}

	for (size_t i = 1; i < COLS - 1; i++) {
		for (size_t j = 1; j < ROWS - 1; j++) {
			char s[2];

			if (fscanf(fp, " %1[0123456789]", s) != 1)
				break;

			octopus[i][j].energy = *s - '0';
		}

		if (feof(fp))
			break;
		if (ferror(fp)) {
			perror("fscanf");
			exit(EXIT_FAILURE);
		}
	}

	uintmax_t count = 0;

	for (size_t step = 0; step < STEPS; step++) {
		for (size_t i = 1; i < COLS - 1; i++)
			for (size_t j = 1; j < ROWS - 1; j++)
				octopus[i][j].energy++;

		for (size_t i = 1; i < COLS - 1; i++)
			for (size_t j = 1; j < ROWS - 1; j++)
				if (octopus[i][j].energy > 9) {
					octopus[i][j].energy = 0;
					octopus[i][j].flashed = true;
					count += flash(octopus, i, j);
				}

		for (size_t i = 1; i < COLS - 1; i++)
			for (size_t j = 1; j < ROWS - 1; j++)
				octopus[i][j].flashed = false;
	}
	
	printf("Part 1: %ju\n", count);
	fclose(fp);
	return 0;
}
