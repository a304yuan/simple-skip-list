#include "skip_list.h"
/*
skiplist *skiplist_new_from_array(void * arr, size_t n, size_t elem_size, double prob, skiplist_compare cmp) {
	skiplist *list = malloc(sizeof(skiplist));

	if (list == NULL) return list;

	list->size = n;
	list->levels = - log(n) / log(prob) + 1;
	list->prob = prob;
	list->head = malloc(sizeof(skiplist_node)+sizeof(skiplist_node *)*list->levels);
	list->head->key = NULL;
	list->cmp = cmp;

	//Form level 0
	skiplist_node *last_node = list->head;
	skiplist_node *cur_node;
	for(size_t i = 0; i < n; i++) {
		cur_node = malloc(sizeof(skiplist_node)+sizeof(skiplist_node *)*list->levels);
		cur_node->data = malloc(elem_size);
		cur_node->key = cur_node->data;
		memcpy(cur_node->key, (char*)a + elem_size * i, elem_size);
		last_node->next[0] = cur_node;
		last_node = cur_node;
	}
	last_node->next[0] = NULL;

	//Form higher levels
	srand((unsigned)time(NULL));
	for(int i = 1; i < list->levels; i++) {
		last_node = list->head;
		cur_node = list->head->next[i-1];
		while (cur_node != NULL) {
			if (rand() % 100 / 100.0 <= prob) {
				last_node->next[i] = cur_node;
				last_node = cur_node;
			}
			else cur_node = cur_node->next[i-1];
		}
		last_node->next[i] = NULL;
	}

	return list;
}
*/
skiplist * skiplist_new_from_array(void * arr, size_t n, size_t elem_size, double prob, skiplist_compare cmp) {
	skiplist * list = malloc(sizeof(skiplist));
	if (list == NULL) return list;

	list->size = n;
	list->levels = - log(n) / log(prob) + 1;
	list->prob = prob;
	list->head = malloc(sizeof(skiplist_node) + sizeof(skiplist_node*) * list->levels);
	list->head->key = NULL;
	list->cmp = cmp;

	//Form level 0
	skiplist_node *last_node = list->head;
	skiplist_node *cur_node;
	for(size_t i = 0; i < n; i++) {
		cur_node = malloc(sizeof(skiplist_node) + sizeof(skiplist_node *) * list->levels);
		cur_node->data = malloc(elem_size);
		cur_node->key = cur_node->data;
		memcpy(cur_node->key, (char*)a + elem_size * i, elem_size);
		last_node->next[0] = cur_node;
		last_node = cur_node;
	}
	last_node->next[0] = NULL;

	//Form higher levels
	int count;
	for(int i = 1; i < list->levels; i++) {
		last_node = list->head;
		cur_node = list->head->next[i-1];
		count = 1;
		while (cur_node != NULL) {
			if (count == (int)(1 / prob)) {
				last_node->next[i] = cur_node;
				last_node = cur_node;
				count = 0;
			}
			else {
				cur_node = cur_node->next[i-1];
				count++;
			}
		}
		last_node->next[i] = NULL;
	}

	return list;
}

static skiplist_node * skiplist_new_node(const void * key, size_t len, int levels) {
    skiplist_node * node = malloc(sizeof(skiplist_node) + sizeof(skiplist_node*) * levels);
    node->key = malloc(len);
    memcpy(node->key, key, len);
    return node;
}

skiplist * skiplist_new(size_t elem_size, int max_levels, double prob, skiplist_compare cmp) {

}

static void skiplist_free_node(skiplist_node * node) {
    if (node->key) free(node->key);
    free(node);
}

void skiplist_free(skiplist * list) {
    skiplist_node * cur_node = list->head;
    skiplist_node * next_node;
    while (cur_node) {
        next_node = cur_node->next[0];
        skiplist_free_node(cur_node);
        cur_node = next_node;
    }
    free(list);
}

size_t skiplist_size(skiplist * list) {
    return list->size;
}

static skiplist_node * skiplist_find_node(skiplist * list, void * key) {
	int lvl = list->levels - 1;
	skiplist_node * cur_node = list->head;
	while (lvl >= 0 && cur_node) {
        skiplist_node * next_node = cur_node->next[lvl];
		if (next_node && list->cmp(key, next_node->key) >= 0) {
            cur_node = next_node;
        }
		else {
            lvl -= 1;
        }
		if (cur_node != list->head)
            if (list->cmp(key, cur_node->key) == 0) return cur_node;
	}

	return NULL;
}

bool skiplist_exist(skiplist *list, void *key) {
    return skiplist_find_node(list, key) ? true : false;
}

void skiplist_insert(skiplist * list, void * key) {
	skiplist_node * update[list->levels];
	int p = 0;
	skiplist_node * cur_node = list->head;
	skiplist_node * new_node = malloc(sizeof(skiplist_node) + list->levels * sizeof(skiplist_node*));
	new_node->data = malloc(key_size + val_size);
	if (key_size == 0)
        new_node->key = key;
	else {
        new_node->key = new_node->data;
        memcpy(new_node->key, key, key_size);
	}

	if (val_size == 0)
        new_node->value = value;
    else {
        new_node->value = (char*)new_node->data + key_size;
        memcpy(new_node->value, value, val_size);
    }

	int lvl = list->levels - 1;
	while(lvl >= 0 && cur_node) {
		if (cur_node->next[lvl] && list->cmp_key(key, cur_node->next[lvl]->key) >= 0)
			cur_node = cur_node->next[lvl];
		else {
			lvl--;
			update[p++] = cur_node;
		}
	}

	lvl = 0;
	srand((unsigned)time(NULL));
	do {
		cur_node = update[--p];
		new_node->next[lvl] = cur_node->next[lvl];
		cur_node->next[lvl] = new_node;
		lvl++;
	}
	while(lvl < list->levels && rand() % 100 / 100.0 <= list->prob);

	list->size++;
}

void skiplist_delete(skiplist *list, void *key) {
	skiplist_node *update[list->levels];
	int p = 0;
	int lvl = list->levels - 1;
	skiplist_node *cur_node = list->head;

	while(lvl >= 0 && cur_node) {
		if (cur_node->next[lvl] && list->cmp_key(key, cur_node->next[lvl]->key) > 0)
			cur_node = cur_node->next[lvl];
		else if (cur_node->next[lvl] && list->cmp_key(key, cur_node->next[lvl]->key) == 0) {
			lvl--;
			update[p++] = cur_node;
		}
		else lvl--;
	}

	lvl = 0;
	skiplist_node *del_node = update[p-1]->next[0];
	while(p) {
		cur_node = update[--p];
		cur_node->next[lvl] = cur_node->next[lvl]->next[lvl];
		lvl++;
	}

	skiplist_free_node(del_node);
	list->size--;
}
