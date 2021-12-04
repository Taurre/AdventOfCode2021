#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NBITS 12
#define LINES 1000

static uintmax_t
find_rating(uint16_t numbers[], bool oxygen) {
	uintmax_t min = 0;
	uintmax_t max = (1 << NBITS) - 1;
	uintmax_t treshold = 1 << (NBITS - 1);
	uintmax_t res = 0;

	for (uintmax_t i = 0; i < NBITS; i++) {
		uintmax_t count = 0, higher = 0, last = 0;

		for (uintmax_t j = 0; j < LINES; j++) {
			if (numbers[j] < min || numbers[j] > max)
				continue;
			if ((numbers[j] & treshold))
				higher++;

			last = numbers[j];
			count++;
		}

		if (oxygen) {
			if (higher >= count - higher)
				min += treshold;
			else
				max -= treshold;
		}
		else {
			if (higher < count - higher)
				min += treshold;
			else
				max -= treshold;
		}

		treshold >>= 1;

		if (count == 1) {
			res = last;
			break;
		}
		else if (min == max) {
			res = max;
			break;
		}
	}

	return res;
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

	uint16_t numbers[LINES] = { 0 };
	char buf[255];

	for (size_t i = 0; fgets(buf, sizeof buf, fp) != NULL; i++) {
		for (size_t j = 0; j < NBITS; j++)
			numbers[i] |= (buf[j] - '0') << (NBITS - j - 1);
	}

	uintmax_t oxygen = find_rating(numbers, true);
	uintmax_t co2 = find_rating(numbers, false);
	printf("Life support rating: %ju\n", oxygen * co2);
	fclose(fp);
	return 0;
}
