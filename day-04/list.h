#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stddef.h>

struct node {
	void *data;
	struct node *next;
};

struct list {
	struct node *first;
	struct node *last;
	size_t len;
};

extern bool list_append(struct list *, void *);
extern bool list_append_sorted(struct list *, void *, int (*)(void *, void *));
extern bool list_push(struct list *, void *);
extern void *list_search(struct list *, void *, bool (*)(void *, void *));
extern void *list_pop(struct list *);

#endif /* LIST_H */
