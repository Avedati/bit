// TODO implement instructions and features outside of UC Berkeley's CS 61C reference card (floating point, strings)
// TODO implement pseudo and extended instructions
// TODO strings
// TODO remove comments in the lexer
#include "../include/b_list.h"
#include "../include/b_string.h"
#include "../include/globals.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

b_string read_file(b_string filename) {
	FILE* fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	long length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	b_string contents = (b_string)malloc(sizeof(char) * (length + 1));
	fread(contents, length, 1, fp);
	contents[length] = 0;
	fclose(fp);

	return contents;
}

int main(int argc, char* argv[]) {
	/* TOKENIZE */
	if (argc == 1) {
		printf("usage: bit file.s\n");
		return 0;
	}

	b_string program = read_file(argv[1]);
	b_list* tokens = tokenize(program);

	/* PARSE */
	b_parser* parser = b_parser_new(tokens);
	b_parse_labels(parser);
	uint32_t n_output;
	uint32_t* output = b_parse_instructions(parser, &n_output);

	/* OUTPUT HEX */
	for (uint32_t i = 0; i < n_output; i++) {
		printf("0x%08x\n", output[i]);
	}

	/* FREE PARSER */
	free(output);
	b_parser_free(parser);

	/* FREE TOKENS */
	for (unsigned int i = 0; i < tokens->len; i++) {
		b_token* tok = b_list_get(tokens, i, b_token);
		b_token_free(tok);
	}
	b_list_free(tokens);

	/* FREE PROGRAM */
	free(program);

	return 0;
}
