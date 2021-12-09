#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ROWS 100
#define COLS 100

#define OWNED 0x80
#define VALUE(map) ((map) & 0x7F)
#define IS_OWNED(map) (((map) & 0x80) == 0x80)

struct basin {
	size_t *data;
	size_t len;
	size_t cap;
};

static int
size_compare(void const *a, void const *b) {
	size_t c = *(size_t *)a;
	size_t d = *(size_t *)b;

	return c < d ? -1 : (c == d) ? 0 : 1;
}

size_t
flow_downward(uint8_t (*map)[ROWS], size_t i, size_t j) {
	size_t size = 1;

	map[i][j] |= OWNED;

	if (i > 0 && VALUE(map[i - 1][j]) < 9 && !IS_OWNED(map[i - 1][j]))
		size += flow_downward(map, i - 1, j);
	if (j > 0 && VALUE(map[i][j - 1] < 9 && !IS_OWNED(map[i][j - 1])))
		size += flow_downward(map, i, j - 1);
	if (i < COLS - 1 && VALUE(map[i + 1][j]) < 9 && !IS_OWNED(map[i + 1][j]))
		size += flow_downward(map, i + 1, j);
	if (j < ROWS - 1 && VALUE(map[i][j + 1]) < 9 && !IS_OWNED(map[i][j + 1]))
		size += flow_downward(map, i, j + 1);

	return size;
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

	struct basin *basins = &(struct basin) { .data = NULL, .len = 0, .cap = 0 };

	for (size_t i = 0; i < COLS; i++) {
		for (size_t j = 0; j < ROWS; j++) {
			if (i > 0) {
				if (VALUE(map[i - 1][j]) <= map[i][j])
					continue;
			}
			if (j > 0) {
				if (VALUE(map[i][j - 1]) <= map[i][j])
					continue;
			}
			if (i < COLS - 1) {
				if (VALUE(map[i + 1][j]) <= map[i][j])
					continue;
			}
			if (j < ROWS - 1) {
				if (VALUE(map[i][j + 1]) <= map[i][j])
					continue;
			}

			if (basins->cap <= basins->len) {
				void *tmp = realloc(basins->data, sizeof basins->data * (basins->cap + 16U));

				if (tmp == NULL) {
					perror("realloc");
					exit(EXIT_FAILURE);
				}

				basins->data = tmp;
				basins->cap += 16U;
			}

			basins->data[basins->len] = flow_downward(map, i, j);
			basins->len++;
		}
	}

	qsort(basins->data, basins->len, sizeof *basins->data, &size_compare);

	if (basins->len >= 3) {
		size_t res = 1;
		res *= basins->data[basins->len - 1];
		res *= basins->data[basins->len - 2];
		res *= basins->data[basins->len - 3];
		printf("Part 2: %zu\n", res);
	}	
	fclose(fp);
	return 0;
}
