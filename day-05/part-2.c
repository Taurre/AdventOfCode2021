#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ROWS 991
#define COLS 991

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

	unsigned char map[COLS][ROWS] = { 0 };
	unsigned x1, x2, y1, y2;

	while (fscanf(fp, "%u,%u -> %u,%u", &x1, &y1, &x2, &y2) == 4) {
		unsigned x = x1;
		unsigned y = y1;

		for (;;) {
			map[y][x] += 1;

			if (x == x2 && y == y2)
				break;

			if (x < x2)
				x++;
			else if (x > x2)
				x--;
			if (y < y2)
				y++;
			else if (y > y2)
				y--;
		}
	}

	if (ferror(fp)) {
		perror("fscanf");
		exit(EXIT_FAILURE);
	}

	unsigned count = 0;

	for (size_t i = 0; i < COLS; i++) 
		for (size_t j = 0; j < ROWS; j++)
			if (map[i][j] >= 2)
				count++;

	printf("Part 2: %u\n", count);
	fclose(fp);
	return 0;
}
