#include <stdio.h>
#include <stdlib.h>


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

	unsigned prev = 0;
	unsigned count = 0;
	unsigned depth;

	while (fscanf(fp, "%u", &depth) == 1) {
		if (prev)
			if (depth > prev) 
				count++;

		prev = depth;
	}

	if (ferror(fp)) {
		perror("fscanf");
		exit(EXIT_FAILURE);
	}

	printf("Part 1: %u\n", count);
	fclose(fp);
	return 0;
}
