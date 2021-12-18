#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum position {
	LEFT = 0,
	RIGHT = 1,
};

struct node {
	uint8_t num;
	struct node *parent;
	struct node *left;
	struct node *right;
};

static struct node *
node_create(struct node *parent) {
	struct node *node = malloc(sizeof *node);

	if (node == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	node->parent = parent;
	node->left = NULL;
	node->right = NULL;
	node->num = 0;
	return node;
}

static uintmax_t
tree_max_pair_level(struct node *node, uintmax_t level) {
	if (node->left == NULL && node->right == NULL)
		return level - 1;

	uintmax_t left = tree_max_pair_level(node->left, level + 1);
	uintmax_t right = tree_max_pair_level(node->right, level + 1);
	return left > right ? left : right;
}

static struct node *
tree_find_first_pair_with_level(struct node *node, uintmax_t target_level, uintmax_t current_level) {
	if (node->left == NULL && node->right == NULL)
		return NULL;
	if (current_level == target_level)
		return node;

	struct node *left = tree_find_first_pair_with_level(node->left, target_level, current_level + 1);

	if (left != NULL)
		return left;

	struct node *right = tree_find_first_pair_with_level(node->right, target_level, current_level + 1);

	if (right != NULL)
		return right;

	return NULL;
}

static struct node *
tree_find_first_to_split(struct node *node) {
	if (node->left == NULL && node->right == NULL) {
		if (node->num > 9)
			return node;
		else
			return NULL;
	}

	struct node *left = tree_find_first_to_split(node->left);

	if (left != NULL)
		return left;

	struct node *right = tree_find_first_to_split(node->right);

	if (right != NULL)
		return right;

	return NULL;
}

static struct node *
tree_find_first_regular(struct node *node) {
	if (node->left == NULL && node->right == NULL)
		return node;

	struct node *left = tree_find_first_regular(node->left);

	if (left != NULL)
		return left;

	struct node *right = tree_find_first_regular(node->right);

	if (right != NULL)
		return right;

	return NULL;
}

static void
tree_add_to_first_regular(struct node *node, uint8_t num, enum position pos) {
	struct node *prev = node;

	for (struct node *parent = node->parent; parent != NULL; parent = parent->parent) {
		struct node *regular = NULL;

		if (pos == LEFT && parent->left != prev)
			regular = tree_find_first_regular(parent->left);
		else if (pos == RIGHT && parent->right != prev)
			regular = tree_find_first_regular(parent->right);

		if (regular != NULL) {
			regular->num += num;
			break;
		}

		prev = parent;
	}
}

static void
node_explode(struct node *node) {
	tree_add_to_first_regular(node, node->left->num, LEFT);
	tree_add_to_first_regular(node, node->right->num, RIGHT);
	free(node->left);
	node->left = NULL;
	free(node->right);
	node->right = NULL;
	node->num = 0;
}

static void
node_split(struct node *node) {
	struct node *left = node_create(node);
	struct node *right = node_create(node);
	left->num = floor(node->num / 2.);
	right->num = ceil(node->num / 2.);
	node->left = left;
	node->right = right;	
}

static void
tree_print(struct node *node) {
	if (node->left == NULL) {
		printf("%d", node->num);
		return;
	}

	assert(node->left != NULL);
	assert(node->right != NULL);
	putchar('[');
	tree_print(node->left);
	putchar(',');
	tree_print(node->right);
	putchar(']');
}

static struct node *
tree_create_from_line(char const *line) {
	assert(line != NULL);
	enum position pos = LEFT;
	struct node *current = NULL;

	for (char const *s = line; *s != '\n' && *s != '\0'; s++) {
		struct node *node;

		switch (*s) {
		case '[':
			node = node_create(current);

			if (current != NULL) {
				if (pos == LEFT)
					current->left = node;
				else
					current->right = node;
			}

			current = node;
			pos = LEFT;
			break;
		case ']':
			if (current->parent != NULL)
				current = current->parent;

			pos = LEFT;
			break;
		case ',':
			pos = RIGHT;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			node = node_create(current);
			node->num = *s - '0';

			if (pos == LEFT)
				current->left = node;
			else
				current->right = node;

			break;
		}
	}

	return current;
}

static struct node *
tree_add(struct node *a, struct node *b) {
	struct node *pair = node_create(NULL);
	pair->left = a;
	pair->right = b;
	a->parent = pair;
	b->parent = pair;
	return pair;
}

static void
tree_reduce(struct node *node) {
	uintmax_t max_level;

	for (;;) {
		if ((max_level = tree_max_pair_level(node, 1)) >= 5) {
			struct node *pair = tree_find_first_pair_with_level(node, max_level, 1);

			if (pair != NULL) {
				node_explode(pair);
				printf("After explosion: ");
				tree_print(node);
				putchar('\n');
				continue;
			}
		}

		struct node *regular = tree_find_first_to_split(node);

		if (regular != NULL) {
			node_split(regular);
			printf("After split: ");
			tree_print(node);
			putchar('\n');
			continue;
		}

		break;
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

	char buf[254];
	struct node *a = NULL;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (a == NULL) {
			a = tree_create_from_line(buf);
			tree_print(a);
			putchar('\n');
			continue;
		}

		struct node *b = tree_create_from_line(buf);
		tree_print(b);
		putchar('\n');
		struct node *sum = tree_add(a, b);
		tree_print(sum);
		putchar('\n');
		tree_reduce(sum);
		tree_print(sum);
		putchar('\n');
		a = sum;
	}

	if (ferror(fp)) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}
	
	tree_print(a);
	putchar('\n');
	fclose(fp);
	return 0;
}
