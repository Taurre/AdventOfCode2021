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

	char buf[255];
	long depth = 0;
	long pos = 0;
	long aim = 0;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		long n;
		
		if (sscanf(buf, "%*s %ld", &n) != 1) {
			perror("sscanf");
			exit(EXIT_FAILURE);
		}

		switch (buf[0]) {
		case 'f':
			pos += n;
			depth += aim * n;
			break;
		case 'u':
			aim -= n;
			break;
		case 'd':
			aim += n;
			break;
		}
		
	}

	printf("Answer: %ld\n", pos * depth);
	fclose(fp);
	return 0;
}
