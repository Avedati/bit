#ifndef __LEXER_H_
#define __LEXER_H_ 0

#include "b_list.h"
#include "b_string.h"
#include <stdint.h>

typedef enum b_token_type {
	T_NUM = 100, T_REG, T_LABEL, T_INST, T_COLON, T_OPEN_PAREN, T_CLOSE_PAREN
} b_token_type;

typedef struct b_token {
	b_token_type type;
	b_string lexeme;
	uint32_t imm_val;
} b_token;

const b_string b_token_type_str(b_token_type);
b_token* b_token_new(b_token_type, b_string);
void b_token_free(b_token*);
b_list* tokenize(b_string);

#endif // __LEXER_H_
