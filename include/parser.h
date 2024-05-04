#ifndef __PARSER_H_
#define __PARSER_H_ 0

#include "b_list.h"
#include "lexer.h"
#include <stdint.h>

typedef struct b_parser {
	b_list* tokens;
	uint32_t index;

	b_list* labels;
	uint32_t* addresses;
} b_parser;

b_parser* b_parser_new(b_list*);
void b_parser_free(b_parser*);

#define b_parser_eos(parser) (parser->index < 0 || parser->index >= parser->tokens->len)
#define b_parser_current(parser) (b_parser_eos(parser) ? NULL : b_list_get(parser->tokens, parser->index, b_token))
b_token* b_parser_advance(b_parser*);

unsigned char b_parser_match_type(b_parser*, b_token_type);
b_token* b_parser_expect_type(b_parser*, b_token_type);

void b_parse_labels(b_parser* parser);
uint32_t b_parser_lookup(b_parser*, b_string);

uint32_t b_parse_r_type(b_parser*);
uint32_t b_parse_i_type(b_parser*);
uint32_t b_parse_i_star_type(b_parser*);
uint32_t b_parse_load(b_parser*);
uint32_t b_parse_store(b_parser*);
uint32_t b_parse_branch(b_parser*);
uint32_t b_parse_u_type(b_parser*);
uint32_t b_parse_jal(b_parser*);
uint32_t* b_parse_instructions(b_parser*, uint32_t*);

#endif // __PARSER_H_
