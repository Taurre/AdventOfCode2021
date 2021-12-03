#include <stdio.h>
#include <stdlib.h>

#define NBITS 12

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
	unsigned bin[NBITS] = { 0 };
	size_t len = 0;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		unsigned tmp[NBITS] = { 0 };

		for (size_t i = 0; i < NBITS; i++)
			tmp[i] = buf[i] - '0';
		
		for (size_t i = 0; i < NBITS; i++)
			bin[i] += tmp[i];

		len++;
	}

	unsigned gamma = 0;
	unsigned epsilon = 0;

	for (size_t i = 0; i < NBITS; i++)
		if (len / bin[i] <= 1)
			gamma |= 1 << (NBITS - i - 1);
		else
			epsilon |= 1 << (NBITS - i - 1);

	printf("Answer: %lu\n", (unsigned long)gamma * epsilon);
	fclose(fp);
	return 0;
}
