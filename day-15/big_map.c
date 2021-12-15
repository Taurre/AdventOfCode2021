#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLS 100
#define ROWS 100

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

	int8_t map[COLS][ROWS];

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
			perror("fscanf");
			exit(EXIT_FAILURE);
		}
	}

	for (size_t i = 0; i < COLS * 5; i++) {
		for (size_t j = 0; j < COLS * 5; j++) {
			int8_t value = map[i % COLS][j % ROWS];
			value += i / COLS;
			value += j / ROWS;

			while (value > 9)
				value -= 9;

			printf("%d", value);
		}

		putchar('\n');
	}

	fclose(fp);
	return 0;
}
