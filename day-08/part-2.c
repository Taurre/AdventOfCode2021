#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATTERNS 10
#define SEGMENTS 10
#define DIGITS    4

static uint8_t flags[] = {
	[0] = 0x01,
	[1] = 0x02,
	[2] = 0x04,
	[3] = 0x08,
	[4] = 0x10,
	[5] = 0x20,
	[6] = 0x40
};

struct pattern {
	uint8_t segments;
	size_t len;
};

static struct pattern
pattern_match_mask(struct pattern *patterns, uint8_t mask, size_t len) {
	for (size_t i = 0; i < PATTERNS; i++)
		if ((patterns[i].segments & mask) == mask && patterns[i].len == len) {
			struct pattern res = patterns[i];
			patterns[i].segments = patterns[i].len = 0;
			return res;
		}

	printf("Cannot find mask %02x with len %zu\n", mask, len);
	abort();
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

	uintmax_t sum = 0;

	for (;;) {
		struct pattern patterns[PATTERNS] = { [0] = { .len = 0 } };
		struct pattern correct[PATTERNS] = { [0] = { .len = 0 } };

		for (size_t i = 0; i < PATTERNS; i++) {
			char pattern[SEGMENTS];

			if (fscanf(fp, "%9s", pattern) != 1) {
				if (feof(fp))
					goto end;
				if (ferror(fp)) {
					perror("fscanf");
					exit(EXIT_FAILURE);
				}
			}
			for (char *s = pattern; *s != '\0'; s++) {
				patterns[i].segments |= flags[*s - 'a'];
				patterns[i].len++;
			}

			switch (patterns[i].len) {
			case 2:
				correct[1] = patterns[i];
				patterns[i].segments = patterns[i].len = 0;
				break;
			case 4:
				correct[4] = patterns[i];
				patterns[i].segments = patterns[i].len = 0;
				break;
			case 3:
				correct[7] = patterns[i];
				patterns[i].segments = patterns[i].len = 0;
				break;
			case 7:
				correct[8] = patterns[i];
				patterns[i].segments = patterns[i].len = 0;
				break;
			}
		}

		correct[6] = pattern_match_mask(patterns, correct[8].segments ^ correct[7].segments, 6);
		correct[5] = pattern_match_mask(patterns, correct[6].segments ^ (~correct[4].segments & 0x7F), 5);
		correct[9] = pattern_match_mask(patterns, correct[4].segments | correct[5].segments, 6);
		correct[2] = pattern_match_mask(patterns, ~correct[4].segments & 0x7F, 5);
		correct[0] = pattern_match_mask(patterns, ~correct[2].segments & 0x7F, 6);
		correct[3] = pattern_match_mask(patterns, correct[1].segments, 5);

		if (fscanf(fp, " |") < 0) {
			perror("fscanf");
			exit(EXIT_FAILURE);
		}

		struct pattern digits[DIGITS] = { [0] = { .len = 0 } };

		for (size_t i = 0; i < DIGITS; i++) {
			char digit[SEGMENTS];

			if (fscanf(fp, "%s", digit) != 1) {
				if (ferror(fp)) {
					perror("fscanf");
					exit(EXIT_FAILURE);
				}
			}
			for (char *s = digit; *s != '\0'; s++) {
				digits[i].segments |= flags[*s - 'a'];
				digits[i].len++;
			}
		}

		for (size_t i = 0; i < DIGITS; i++) {
			for (size_t j = 0; j < PATTERNS; j++)
				if (digits[i].segments == correct[j].segments) {
					sum += j * (size_t)pow(10., (double)DIGITS - i - 1);
					break;
				}
		}
	}
end:
	if (ferror(fp)) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}

	printf("Part 2: %ju\n", sum);
	fclose(fp);
	return 0;
}
