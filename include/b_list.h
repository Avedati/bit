#ifndef __B_LIST_H_
#define __B_LIST_H_ 0

typedef struct b_list {
	void** data;
	unsigned int len;
} b_list;

b_list* b_list_new(void);
void b_list_free(b_list*);
unsigned char b_list_append(b_list*, void*);
#define b_list_get(lst, idx, type) ((0 <= idx && idx < lst->len) ? ((type*)(lst->data[idx])) : NULL)
unsigned char b_list_set(b_list*, unsigned int, void*);

#endif // __B_LIST_H_
