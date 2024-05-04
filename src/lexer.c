#include "../include/b_list.h"
#include "../include/b_string.h"
#include "../include/globals.h"
#include "../include/lexer.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const b_string b_token_type_str(b_token_type type) {
	switch (type) {
		case T_NUM:
			return "NUM";
		case T_REG:
			return "REG";
		case T_LABEL:
			return "LABEL";
		case T_INST:
			return "INST";
		case T_COLON:
			return "COLON";
		default:
			break;
	}
	return "";
}

b_token* b_token_new(b_token_type type, b_string lexeme) {
	b_token* tok = (b_token*)malloc(sizeof(b_token));
	tok->type = type;
	tok->lexeme = b_string_new();
	for (unsigned int i = 0; i < strlen(lexeme); i++) {
		b_string_append(tok->lexeme, lexeme[i]);
	}
	return tok;
}

void b_token_free(b_token* tok) {
	b_string_free(tok->lexeme);
	free(tok);
}

b_list* tokenize(b_string program) {
	b_list* tokens = b_list_new();
	for (unsigned int i = 0; i < strlen(program); i++) {
		if (isalpha(program[i]) || program[i] == '_') {
			b_string lexeme = b_string_new();
			for (; i < strlen(program) && program[i] != '\n' && (isalnum(program[i]) || program[i] == '_'); i++) {
				b_string_append(lexeme, program[i]);
			}
			i--;

			unsigned char found = 0;
			for (unsigned int j = 0; j < N_INSTRUCTIONS; j++) {
				if (strcmp(lexeme, INSTRUCTIONS[j]) == 0) {
					b_list_append(tokens, b_token_new(T_INST, lexeme));
					b_string_free(lexeme);
					found = 1;
					break;
				}
			}

			if (!found) {
				for (unsigned int j = 0; j < N_REGS; j++) {
					if (strcmp(lexeme, ARCH_REGS[j]) == 0) {
						b_list_append(tokens, b_token_new(T_REG, lexeme));
						b_string_free(lexeme);
						found = 1;
						break;
					}
				}
			}

			if (!found) {
				for (unsigned int j = 0; j < N_REGS; j++) {
					if (strcmp(lexeme, PHYS_REGS[j]) == 0) {
						b_list_append(tokens, b_token_new(T_REG, lexeme));
						b_string_free(lexeme);
						found = 1;
						break;
					}
				}
			}

			if (!found) {
				b_list_append(tokens, b_token_new(T_LABEL, lexeme));
				b_string_free(lexeme);
			}
		}
		else if (program[i] == ':') {
			b_list_append(tokens, b_token_new(T_COLON, ":"));
		}
		else if (program[i] == '(') {
			b_list_append(tokens, b_token_new(T_OPEN_PAREN, "("));
		}
		else if (program[i] == ')') {
			b_list_append(tokens, b_token_new(T_CLOSE_PAREN, ")"));
		}
		else if (isdigit(program[i]) || program[i] == '-') {
			unsigned char negative = (program[i] == '-');
			b_string lexeme = b_string_new();
			if (!negative) {
				b_string_append(lexeme, program[i]);
			}

			uint8_t base = 10; // decimal
			for (i++; i < strlen(program) && program[i] != '\n' && program[i] != '('; i++) {
				if (base == 10 && isdigit(program[i])) {
					b_string_append(lexeme, program[i]);
				}
				else if (base == 2 && (program[i] == '0' || program[i] == '1')) {
					b_string_append(lexeme, program[i]);
				}
				else if (base == 16 && (isdigit(program[i]) || ('a' <= program[i] && program[i] <= 'f') ||
				                                               ('A' <= program[i] && program[i] <= 'F'))) {
					b_string_append(lexeme, program[i]);
				}
				else if (strlen(lexeme) == (1 + negative) && (program[i] == 'b' || program[i] == 'x')) {
					b_string_append(lexeme, program[i]);
					base = (program[i] == 'b') ? 2 : 16;
				}
				else {
					fprintf(stderr, "Error: invalid character for integer literal `%c`.\n", program[i]);
					break;
				}
			}
			i--;

			uint32_t imm_val;
			if (base == 2) {
				imm_val = (uint32_t)strtol(lexeme + 2, NULL, base);
			}
			else {
				imm_val = (uint32_t)strtol(lexeme, NULL, base);
			}
			if (negative) {
				imm_val = (~imm_val) + 1; // two's complement
			}

			b_token* tok = b_token_new(T_NUM, lexeme);
			tok->imm_val = imm_val;
			b_list_append(tokens, tok);
			b_string_free(lexeme);
		}
		else if (!isspace(program[i]) && program[i] != ',') {
			fprintf(stderr, "Error: invalid character `%c`.\n", program[i]);
		}
	}
	return tokens;
}
