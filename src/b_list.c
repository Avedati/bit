#include "../include/b_list.h"
#include <stdlib.h>

b_list* b_list_new(void) {
	b_list* lst = (b_list*)malloc(sizeof(b_list));
	lst->data = NULL;
	lst->len = 0;
	return lst;
}

// WARNING: this function does not free individual elements.
void b_list_free(b_list* lst) {
	free(lst->data);
	free(lst);
}

unsigned char b_list_append(b_list* lst, void* el) {
	lst->data = realloc(lst->data, sizeof(void*) * (lst->len + 1));
	if (lst->data == NULL) {
		return 0;
	}

	lst->data[lst->len++] = el;
	return 1;
}

unsigned char b_list_set(b_list* lst, unsigned int idx, void* el) {
	if (0 <= idx && idx < lst->len) {
		lst->data[idx] = el;
		return 1;
	}

	return 0;
}
