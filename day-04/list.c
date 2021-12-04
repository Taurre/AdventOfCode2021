#include <assert.h>
#include <stdlib.h>

#include "list.h"

static struct node *
node_create(void *data) {
	struct node *node = malloc(sizeof *node);

	if (node == NULL)
		return NULL;

	node->data = data;
	node->next = NULL;
	return node;
}

bool
list_push(struct list *list, void *data) {
	assert(list != NULL);
	assert(data != NULL);
	struct node *node = node_create(data);

	if (node == NULL)
		return false;

	if (list->first == NULL) {
		list->first = node;
		list->last = node;
	}
	else {
		node->next = list->first;
		list->first = node;
	}

	list->len++;
	return true;
}

bool
list_append(struct list *list, void *data) {
	assert(list != NULL);
	assert(data != NULL);
	struct node *node = node_create(data);

	if (node == NULL)
		return false;

	if (list->last == NULL) {
		list->first = node;
		list->last = node;
	}
	else {
		list->last->next = node;
		list->last = node;
	}

	list->len++;
	return true;
}

bool
list_append_sorted(struct list *list, void *data, int (*cmp)(void *, void *)) {
	assert(list != NULL);
	assert(data != NULL);
	assert(cmp != NULL);

	struct node *node = node_create(data);

	if (node == NULL)
		return false;

	struct node **n;

	for (n = &list->first; *n != NULL; n = &(*n)->next)
		if (cmp(node->data, (*n)->data) < 0)
			break;

	node->next = *n;
	*n = node;
	list->len++;
	return true;
}

void *
list_search(struct list *list, void *data, bool (*filter)(void *, void *)) {
	assert(list != NULL);
	assert(filter != NULL);

	for (struct node *node = list->first; node != NULL; node = node->next)
		if (filter(node->data, data))
			return node->data;

	return NULL;
}

void *
list_pop(struct list *list) {
	assert(list != NULL);

	if (list->first == NULL)
		return NULL;

	struct node *node = list->first;
	list->first = node->next;
	void *data = node->data;
	free(node);
	list->len--;
	return data;
}
