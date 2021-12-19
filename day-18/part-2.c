#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINES 100

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

static struct node *
tree_find_first_pair_to_explode(struct node *node, uintmax_t current_level) {
	if (node->left == NULL && node->right == NULL)
		return NULL;
	if (current_level >= 5)
		return node;

	struct node *left = tree_find_first_pair_to_explode(node->left, current_level + 1);

	if (left != NULL)
		return left;

	struct node *right = tree_find_first_pair_to_explode(node->right, current_level + 1);

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
tree_find_first_regular(struct node *node, enum position pos) {
	if (node->left == NULL && node->right == NULL)
		return node;

	struct node *first = NULL;

	if (pos == LEFT)
		first = tree_find_first_regular(node->right, pos);
	else
		first = tree_find_first_regular(node->left, pos);

	if (first != NULL)
		return first;

	if (pos == LEFT)
		first = tree_find_first_regular(node->left, pos);
	else
		first = tree_find_first_regular(node->right, pos);

	if (first != NULL)
		return first;
	
	return NULL;
}

static void
tree_add_to_first_regular(struct node *node, uint8_t num, enum position pos) {
	struct node *prev = node;

	for (struct node *parent = node->parent; parent != NULL; parent = parent->parent) {
		struct node *regular = NULL;

		if (pos == LEFT && parent->left != prev)
			regular = tree_find_first_regular(parent->left, LEFT);
		else if (pos == RIGHT && parent->right != prev)
			regular = tree_find_first_regular(parent->right, RIGHT);

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

static void
tree_delete(struct node *node) {
	if (node->left == NULL && node->right == NULL) {
		free(node);
		return;
	}

	tree_delete(node->left);
	tree_delete(node->right);
	free(node);
}

static struct node *
tree_copy(struct node *node) {
	struct node *copy = malloc(sizeof *copy);

	if (copy == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	copy->num = node->num;

	if (node->left != NULL) {
		copy->left = tree_copy(node->left);
		copy->left->parent = copy;
	}
	else
		copy->left = NULL;

	if (node->right != NULL) {
		copy->right = tree_copy(node->right);
		copy->right->parent = copy;
	}
	else
		copy->right = NULL;

	return copy;
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
	for (;;) {
		struct node *pair = tree_find_first_pair_to_explode(node, 1);

		if (pair != NULL) {
			node_explode(pair);
			continue;
		}

		struct node *regular = tree_find_first_to_split(node);

		if (regular != NULL) {
			node_split(regular);
			continue;
		}

		break;
	}
}

static uintmax_t
tree_magnitude(struct node *node) {
	uintmax_t res = 0;

	if (node->left->left == NULL)
		res += node->left->num * 3;
	else
		res += tree_magnitude(node->left) * 3;

	if (node->right->right == NULL)
		res += node->right->num * 2;
	else
		res += tree_magnitude(node->right) * 2;

	return res;
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
	struct node *numbers[LINES];

	for (size_t i = 0; fgets(buf, sizeof buf, fp) != NULL; i++)
		numbers[i] = tree_create_from_line(buf);

	if (ferror(fp)) {
		perror("fgets");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
	uintmax_t max = 0;

	for (size_t i = 0; i < LINES; i++) {
		for (size_t j = 1; j < LINES; j++) {
			if (i == j)
				continue;

			struct node *sum = tree_add(tree_copy(numbers[i]), tree_copy(numbers[j]));
			tree_reduce(sum);
			uintmax_t magnitude = tree_magnitude(sum);

			if (magnitude > max)
				max = magnitude; 

			tree_delete(sum);
		}
	}

	printf("Biggest magnitude: %ju\n", max);
	return 0;
}
