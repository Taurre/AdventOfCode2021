#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ROWS 10
#define COLS 5

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

	uint8_t map[COLS][ROWS] = { { 0 } };

	for (size_t i = 0; i < COLS; i++) {
		for (size_t j = 0; j < ROWS; j++) {
			char s[2];

			if (fscanf(fp, " %1[0123456789]", s) != 1)
				break;

			map[i][j] = *s - '0';
		}

		if (feof(fp))
			break;
		if (ferror(fp)) {
			perror("getc");
			exit(EXIT_FAILURE);
		}
	}

	uintmax_t sum = 0;

	for (size_t i = 0; i < COLS; i++) {
		for (size_t j = 0; j < ROWS; j++) {
			if (i > 0) {
				if (map[i - 1][j] <= map[i][j])
					continue;
			}
			if (j > 0) {
				if (map[i][j - 1] <= map[i][j])
					continue;
			}
			if (i < COLS - 1) {
				if (map[i + 1][j] <= map[i][j])
					continue;
			}
			if (j < ROWS - 1) {
				if (map[i][j + 1] <= map[i][j])
					continue;
			}

			sum += map[i][j] + 1;
		}
	}

	printf("Part 1: %ju\n", sum);
	fclose(fp);
	return 0;
}
