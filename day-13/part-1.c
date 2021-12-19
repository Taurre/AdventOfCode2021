#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLS 1311
#define ROWS 1311

static void
page_print(uint8_t (*page)[ROWS], uintmax_t xmax, uintmax_t ymax) {
	for (uintmax_t i = 0; i < ymax; i++) {
		for (uintmax_t j = 0; j < xmax; j++) {
			putchar(page[i][j] ? '#' : '.');
		}

		putchar('\n');
	}
}

static void
fold_up(uint8_t (*page)[ROWS], uintmax_t xmax, uintmax_t ymax, uintmax_t line) {
	uintmax_t ynew = line - 1;

	for (size_t i = line + 1; i < ymax; i++) {
		for (size_t j = 0; j < xmax; j++)
			if (page[i][j])
				page[ynew][j] = 1;

		ynew--;
	}
}

static void
fold_left(uint8_t (*page)[ROWS], uintmax_t xmax, uintmax_t ymax, uintmax_t line) {
	for (size_t i = 0; i < ymax; i++) {
		uintmax_t xnew = line - 1;

		for (size_t j = line + 1; j < xmax; j++) {
			
			if (page[i][j])
				page[i][xnew] = 1;

			xnew--;
		}
	}
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
	uint8_t page[COLS][ROWS] = { { 0 } };
	uintmax_t xmax = ROWS, ymax = COLS;

	while (fgets(line, sizeof line, fp) != NULL) {
		uintmax_t x, y;

		if (strlen(line) == 1)
			break;
		if (sscanf(line, "%ju,%ju", &x, &y) != 2) {
			perror("sscanf");
			exit(EXIT_FAILURE);
		}

		page[y][x] = 1;
	}

	if (ferror(fp)) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}

	while (fgets(line, sizeof line, fp) != NULL) {
		char fold;
		uintmax_t ln;

		if (sscanf(line, "fold along %c=%ju", &fold, &ln) != 2) {
			perror("sscanf");
			exit(EXIT_FAILURE);
		}

		if (fold == 'y') {
			fold_up(page, xmax, ymax, ln);
			ymax = ln;
			break;
		}
		else if (fold == 'x') {
			fold_left(page, xmax, ymax, ln);
			xmax = ln;
			break;
		}
	}

	if (ferror(fp)) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
	uintmax_t count = 0;

	for (size_t i = 0; i < ymax; i++)
		for (size_t j = 0; j < xmax; j++)
			if (page[i][j])
				count++;

	printf("Part 1: %ju\n", count);
	return 0;
}
