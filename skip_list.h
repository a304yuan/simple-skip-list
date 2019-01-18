#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

typedef struct skiplist_node skiplist_node;
typedef struct skiplist skiplist;
typedef int (*skiplist_compare)(const void * a, const void * b);

struct skiplist_node {
	void * key;
	skiplist_node * next[];
};

struct skiplist {
    unsigned seed;
    int levels;
	size_t size;
    size_t elem_size;
	double prob;
	skiplist_compare cmp;
	skiplist_node * head;
};

extern skiplist * skiplist_new_from_array(const void * arr, size_t n, size_t elem_size, double prob, skiplist_compare cmp);
extern skiplist * skiplist_new(size_t elem_size, int max_levels, double prob, skiplist_compare cmp);
extern void skiplist_free(skiplist * list);
extern size_t skiplist_size(const skiplist * list);
extern bool skiplist_exist(const skiplist * list, const void * key);
extern void skiplist_insert(skiplist * list, const void * key);
extern void skiplist_delete(skiplist * list, const void * key);

#endif // SKIP_LIST_H
