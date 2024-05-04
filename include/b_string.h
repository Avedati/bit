#ifndef __B_STRING_H_
#define __B_STRING_H_ 0

typedef char* b_string;
b_string b_string_new(void);
unsigned char b_string_append(b_string, char);
void b_string_free(b_string);

#endif // __B_STRING_H_
