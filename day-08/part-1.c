#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

	char line[512];
	uintmax_t count = 0;

	while (fgets(line, sizeof line, fp) != NULL) {
		char *sep = strchr(line, '|');

		if (sep == NULL) {
			fprintf(stderr, "Badly formated line.\n");
			exit(EXIT_FAILURE);
		}

		char digit[8];
		char *pos = sep + 1;
		int n;

		while (sscanf(pos, "%8s%n", digit, &n) == 1) {
			size_t len = strlen(digit);

			switch (len) {
			case 2:
			case 3:
			case 4:
			case 7:
				count++;
			}

			pos += n;
		}
	}

	if (ferror(fp)) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}

	printf("Part 1: %ju\n", count);
	fclose(fp);
	return 0;
}
