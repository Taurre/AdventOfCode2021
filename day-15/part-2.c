#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLS 502
#define ROWS 502

#define LEN(a) ((sizeof (a)) / sizeof ((a)[0]))

struct node {
	int8_t distance;
	uint16_t weigth;
};

static void
shortest_path(struct node (*map)[ROWS], bool (*visited)[ROWS], int16_t i, int16_t j) {
	struct {
		int16_t x;
		int16_t y;
	} adjacent[] = {
		{ i + 1, j },
		{ i, j + 1 },
	};

	if (visited[i][j])
		return;

	if (map[i - 1][j].distance > 0 && !visited[i - 1][j])
		return;
	if (map[i][j - 1].distance > 0 && !visited[i][j - 1])
		return;
	
	for (size_t k = 0; k < LEN(adjacent); k++) {
		int16_t x = adjacent[k].x, y = adjacent[k].y;

		if (map[x][y].distance > 0) {
			uint16_t weigth = map[i][j].weigth + map[x][y].distance;

			if (weigth < map[x][y].weigth)
				map[x][y].weigth = weigth;
		}
	}

	visited[i][j] = true;

	for (size_t k = 0; k < LEN(adjacent); k++) {
		int16_t x = adjacent[k].x, y = adjacent[k].y;

		if (map[x][y].distance > 0)
			shortest_path(map, visited, x, y);
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

	static struct node map[COLS][ROWS];
	static bool visited[COLS][ROWS];

	for (size_t i = 0; i < COLS; i++)
		for (size_t j = 0; j < ROWS; j++) {
			map[i][j].distance = -1;
			map[i][j].weigth = UINT16_MAX;
			visited[i][j] = false;
		}

	for (size_t i = 1; i < COLS - 1; i++) {
		for (size_t j = 1; j < ROWS - 1; j++) {
			char s[2];

			if (fscanf(fp, " %1[0123456789]", s) != 1)
				break;

			map[i][j].distance = *s - '0';
		}

		if (feof(fp))
			break;
		if (ferror(fp)) {
			perror("fscanf");
			exit(EXIT_FAILURE);
		}
	}

	map[1][1].weigth = 0;
	shortest_path(map, visited, 1, 1);
	printf("Part 1: %ju\n", (uintmax_t)map[COLS - 2][ROWS - 2].weigth);
	fclose(fp);
	return 0;
}
