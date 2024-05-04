#include "../include/b_string.h"
#include <stdlib.h>
#include <string.h>

b_string b_string_new(void) {
	b_string str = malloc(sizeof(char));
	str[0] = 0;
	return str;
}

unsigned char b_string_append(b_string str, char c) {
	unsigned int len = strlen(str);
	str = realloc(str, sizeof(char) * (len + 2));
	if (str == NULL) {
		return 0;
	}

	str[len] = c;
	str[len + 1] = 0;
	return 1;
}

void b_string_free(b_string str) {
	free(str);
}
