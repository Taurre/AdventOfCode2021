#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CRABS 1000

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

	intmax_t crabs[CRABS] = { 0 };
	intmax_t n;
	intmax_t min = INTMAX_MAX, max = INTMAX_MIN;

	for (size_t i = 0; fscanf(fp, "%jd", &n) == 1; i++) {
		crabs[i] = n;

		if (n < min)
			min = n;
		if (n > max)
			max = n;

		int c = getc(fp);

		if (c == EOF || c == '\n')
			break;
        }

	intmax_t least_fuel = INTMAX_MAX;

	for (size_t i = min; i < max; i++) {
		intmax_t fuel = 0;

		for (size_t j = 0; j < CRABS; j++) {
			if (crabs[j] > i)
				fuel += crabs[j] - i;
			else
				fuel += i - crabs[j];
		}

		if (fuel < least_fuel)
			least_fuel = fuel;
	}

	printf("%jd\n", least_fuel);
	fclose(fp);
	return 0;
}
