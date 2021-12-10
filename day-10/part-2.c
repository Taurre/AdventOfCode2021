#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct score {
	uintmax_t *values;
	size_t len;
	size_t cap;
};

static char
end_find(char c) {
	switch (c) {
	case '[':
		return ']';
	case '(':
		return ')';
	case '{':
		return '}';
	case '<':
		return '>';
	}

	abort();
}

static int
uintmax_compare(void const *a, void const *b) {
	uintmax_t ca = *(uintmax_t *)a;
	uintmax_t cb = *(uintmax_t *)b;

	return (ca < cb) ? -1 : (ca == cb) ? 0 : 1;
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

	char line[512];
	char stack[512];
	struct score *scores = &(struct score) { .len = 0 };

	while (fgets(line, sizeof line, fp) != NULL) {
		size_t top = 0;

		for (size_t i = 0; line[i] != '\n' && line[i] != '\0'; i++) {
			if (!top) {
				stack[top++] = line[i];
				continue;
			}

			char end = end_find(stack[top - 1]);

			if (line[i] == end)
				top--;
			else if (strchr("[({<", line[i]) != NULL) 
				stack[top++] = line[i];
			else {
				top = 0;
				break;
			}
		}

		if (top > 0) {
			char *completion = malloc(top + 1);

			if (completion == NULL) {
				perror("malloc");
				exit(EXIT_FAILURE);
			}

			char *pos = completion;

			while (top > 0) {
				*pos++ = end_find(stack[top - 1]);
				top--;
			}

			*pos = '\0';
			uintmax_t score = 0;

			for (pos = completion; *pos != '\0'; pos++) {
				score *= 5;

				switch (*pos) {
				case ')':
					score += 1;
					break;
				case ']':
					score += 2;
					break;
				case '}':
					score += 3;
					break;
				case '>':
					score += 4;
					break;
				}
			}

			if (scores->cap <= scores->len) {
				void *tmp = realloc(scores->values, sizeof *scores->values * (scores->cap + 16));

				if (tmp == NULL) {
					perror("realloc");
					exit(EXIT_FAILURE);
				}

				scores->values = tmp;
				scores->cap += 16;
			}

			scores->values[scores->len++] = score;
			free(completion);
		}
	}

	if (ferror(fp)) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}

	qsort(scores->values, scores->len, sizeof *scores->values, &uintmax_compare);
	printf("Part 2: %ju\n", scores->values[scores->len / 2]);
	fclose(fp);
	return 0;
}
