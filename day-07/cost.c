#include <stdio.h>
#include <stdint.h>

#define MAX 1788

int
main(void) {
	printf("#include <stdint.h>\n");
	printf("static intmax_t cost[] = {\n");
	intmax_t prev = 0;

	for (size_t i = 0; i < MAX + 1; i++) {
		prev += i;
		printf("\t[%4zu] = %jd,\n", i, prev);
	}

	printf("};\n");
	return 0;
}
