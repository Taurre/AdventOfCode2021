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
	char stack[512];
	uintmax_t points = 0;

	while (fgets(line, sizeof line, fp) != NULL) {
		size_t top = 0;

		for (size_t i = 0; line[i] != '\n' && line[i] != '\0'; i++) {
			if (!top) {
				stack[top++] = line[i];
				continue;
			}

			char end;

			switch (stack[top - 1]) {
			case '[':
				end = ']';
				break;
			case '(':
				end = ')';
				break;
			case '{':
				end = '}';
				break;
			case '<':
				end = '>';
				break;
			}

			if (line[i] == end)
				top--;
			else if (strchr("[({<", line[i]) != NULL) 
				stack[top++] = line[i];
			else {
				switch (line[i]) {
				case ')':
					points += 3;
					break;
				case ']':
					points += 57;
					break;
				case '}':
					points += 1197;
					break;
				case '>':
					points += 25137;
					break;
				}
				
				break;
			}
		}
	}

	if (ferror(fp)) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}

	printf("Part 1: %ju points.\n", points);
	fclose(fp);
	return 0;
}
