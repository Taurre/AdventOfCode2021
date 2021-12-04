#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "list.h"

static bool
board_is_winner(unsigned char (*grid)[5]) {
	assert(grid != NULL);

	for (size_t i = 0; i < 5; i++) {
		unsigned cc = 0, rc = 0;

		for (size_t j = 0; j < 5; j++) {
			if ((grid[i][j] & 0x80) == 0x80)
				cc++;
			if ((grid[j][i] & 0x80) == 0x80)
				rc++;
		}

		if (cc == 5 || rc == 5)
			return true;
	}

	return false;
}

static uintmax_t
score_compute(unsigned char (*grid)[5], unsigned last_number) {
	assert(grid != NULL);
	uintmax_t score = 0;

	for (size_t i = 0; i < 5; i++)
		for (size_t j = 0; j < 5; j++)
			if (!(grid[i][j] & 0x80))
				score += grid[i][j];

	return score * last_number;
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

	char buf[1024];

	if (fgets(buf, sizeof buf, fp) == NULL) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}

	struct list *numbers = &(struct list) { .first = NULL };
	char *pos = buf;

	for (;;) {
		int offset = 0;
		char c;
		unsigned *number = malloc(sizeof *number);

		if (number == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		if (sscanf(pos, "%u%c%n", number, &c, &offset) != 2) {
			perror("sscanf");
			exit(EXIT_FAILURE);
		}

		if (!list_append(numbers, number)) {
			perror("list_add");
			exit(EXIT_FAILURE);
		}
		if (c == '\n')
			break;

		pos += offset;
	}

	struct list *boards = &(struct list) { .first = NULL };

	for (;;) {
		unsigned char (*grid)[5] = malloc(sizeof *grid * 5);		

		if (grid == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}

		while (fscanf(fp,
		           "%hhu %hhu %hhu %hhu %hhu"
		           "%hhu %hhu %hhu %hhu %hhu"
		           "%hhu %hhu %hhu %hhu %hhu"
		           "%hhu %hhu %hhu %hhu %hhu"
		           "%hhu %hhu %hhu %hhu %hhu",
		           &grid[0][0], &grid[0][1], &grid[0][2], &grid[0][3], &grid[0][4],
		           &grid[1][0], &grid[1][1], &grid[1][2], &grid[1][3], &grid[1][4],
		           &grid[2][0], &grid[2][1], &grid[2][2], &grid[2][3], &grid[2][4],
		           &grid[3][0], &grid[3][1], &grid[3][2], &grid[3][3], &grid[3][4],
		           &grid[4][0], &grid[4][1], &grid[4][2], &grid[4][3], &grid[4][4]
		          ) == 25) {
			if (!list_append(boards, grid)) {
				perror("list_append");
				exit(EXIT_FAILURE);
			}
		}

		if (ferror(fp)) {
			perror("fscanf");
			exit(EXIT_FAILURE);
		}
		else
			break;
	}

	void *data;
	unsigned char (*winner)[5];
	unsigned last_number;

	while ((data = list_pop(numbers)) != NULL) {
		unsigned number = *(unsigned *)data;

		for (struct node *node = boards->first; node != NULL; node = node->next) {
			unsigned char (*grid)[5] = node->data;

			for (size_t i = 0; i < 5; i++)
				for (size_t j = 0; j < 5; j++)
					if (grid[i][j] == number)
						grid[i][j] |= 0x80;

			if (board_is_winner(grid)) {
				winner = grid;
				last_number = number;
				goto end;
			}
		}
	}

	puts("No winner found");
	return EXIT_FAILURE;
end:
	for (size_t i = 0; i < 5; i++) {
		for (size_t j = 0; j < 5; j++)
			printf("%u ", winner[i][j] & 0x7F);

		putchar('\n');
	}

	printf("%ju\n", (uintmax_t)last_number);
	printf("Score: %ju\n", score_compute(winner, last_number));
	fclose(fp);
	return 0;
}
