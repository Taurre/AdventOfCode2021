#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct reader {
	FILE *fp;
	uint32_t bits;
	size_t len;
};

static bool
read_bits(struct reader *reader, uint32_t *bits, size_t n) {
	assert(reader != NULL);
	assert(n > 0);

	*bits = 0;

	while (reader->len < n) {
		uint32_t hd;

		if (fscanf(reader->fp, "%02x", &hd) != 1) {
			if (ferror(reader->fp)) {
				perror("fscanf");
				exit(EXIT_FAILURE);
			}

			return false;
		}

		reader->bits <<= 8;
		reader->bits |= hd & 0xFF;
		reader->len += 8;
	}

	*bits |= reader->bits >> (reader->len - n);
	reader->len -= n;
	uint32_t mask = UINT32_MAX << reader->len;
	reader->bits &= ~mask;
	return true;
}

void
literal_value(struct reader *reader) {
	for (;;) {
		uint32_t bits = 0;

		if (!read_bits(reader, &bits, 5)) {
			fprintf(stderr, "Bad litteral value\n");
			exit(EXIT_FAILURE);
		}

		if ((bits & 0x10) == 0x10)
			continue;
		else
			break;
	}
}

void
unlabeled(struct reader *reader) {
	uint32_t bits = 0;

	if (!read_bits(reader, &bits, 5)) {
		fprintf(stderr, "Bad unlabeled bits\n");
		exit(EXIT_FAILURE);
	}
}

void
read_packet(struct reader *reader) {
	uint32_t bits = 0;

	if (!read_bits(reader, &bits, 3)) {
		fprintf(stderr, "No packets left\n");
		return;
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

	uint32_t bits = 0;
	struct reader *reader = &(struct reader) { .fp = fp };
	uintmax_t count = 0;

	while (read_bits(reader, &bits, 3)) {
		count += bits;

		if (!read_bits(reader, &bits, 3))
			break;

		if (bits == 4) {
			literal_value(reader);
			continue;
		}

		if (!read_bits(reader, &bits, 1)) {
			fprintf(stderr, "Cannot find length ID\n");
			exit(EXIT_FAILURE);
		}

		if (bits) {
			if (!read_bits(reader, &bits, 11)) {
				fprintf(stderr, "Cannot find number of sub-packets\n");
				exit(EXIT_FAILURE);
			}
		}
		else {
			if (!read_bits(reader, &bits, 15)) {
				fprintf(stderr, "Cannot find total length\n");
				exit(EXIT_FAILURE);
			}
		}
	} 

	printf("Part 1: %ju\n", count);
	fclose(fp);
	return 0;
}
