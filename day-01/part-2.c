#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned window_t[3];

static unsigned
window_sum(window_t window) {
	assert(window != NULL);
	unsigned sum = 0;

	for (size_t i = 0; i < 3; i++)
		sum += window[i];

	return sum;
}

void
window_shift(window_t window) {
	assert(window != NULL);

	for (size_t i = 1; i < 3; i++) 
		window[i - 1] = window[i];
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

	window_t window;

	for (size_t i = 0; i < 3; i++)
		if (fscanf(fp, "%u", window + i) != 1) {
			perror("fscanf");
			exit(EXIT_FAILURE);
		}

	unsigned sum = window_sum(window);
	unsigned prev = sum;
	unsigned count = 0;
	window_shift(window);

	while (fscanf(fp, "%u", window + 2) == 1) {
		sum = window_sum(window);

		if (sum > prev) 
			count++;

		window_shift(window);
		prev = sum;
	}

	if (ferror(fp)) {
		perror("fscanf");
		exit(EXIT_FAILURE);
	}

	printf("Part 2: %u\n", count);
	fclose(fp);
	return 0;
}
