#include "../include/b_list.h"
#include "../include/globals.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

b_parser* b_parser_new(b_list* tokens) {
	b_parser* parser = (b_parser*)malloc(sizeof(b_parser));
	parser->tokens = tokens;
	parser->index = 0;

	parser->labels = b_list_new();
	parser->addresses = NULL;

	return parser;
}

void b_parser_free(b_parser* parser) {
	b_list_free(parser->labels);
	free(parser->addresses);
	free(parser);
}

#define b_parser_eos(parser) (parser->index < 0 || parser->index >= parser->tokens->len)
#define b_parser_current(parser) (b_parser_eos(parser) ? NULL : b_list_get(parser->tokens, parser->index, b_token))
b_token* b_parser_advance(b_parser* parser) {
	if (b_parser_eos(parser)) {
		return NULL;
	}

	b_token* res = b_list_get(parser->tokens, parser->index, b_token);
	parser->index++;
	return res;
}

unsigned char b_parser_match_type(b_parser* parser, b_token_type type) {
	b_token* tok = b_parser_current(parser);
	return tok != NULL && tok->type == type;
}

b_token* b_parser_expect_type(b_parser* parser, b_token_type type) {
	if (!b_parser_match_type(parser, type)) {
		fprintf(stderr, "Error: expected type `%s`, got `%s`.\n", b_token_type_str(type), b_token_type_str(b_parser_current(parser)->type));
		exit(1);
	}

	return b_parser_advance(parser);
}

void b_parse_labels(b_parser* parser) {
	uint32_t pc = 0;
	parser->index = 0;

	// 1st pass: labels
	while (!b_parser_eos(parser)) {
		b_token* tok = b_parser_advance(parser);
		if (tok->type == T_LABEL && b_parser_match_type(parser, T_COLON)) {
			b_list_append(parser->labels, tok->lexeme);
			parser->addresses = realloc(parser->addresses, sizeof(uint32_t) * (parser->labels->len));
			parser->addresses[parser->labels->len - 1] = pc;
		}
		else if (tok->type == T_INST) {
			pc += 4;
		}
	}
}

uint32_t b_parser_lookup(b_parser* parser, b_string label) {
	for (uint32_t i = 0; i < parser->labels->len; i++) {
		if (strcmp(b_list_get(parser->labels, i, char), label) == 0) {
			return parser->addresses[i];
		}
	}

	return 0xFFFFFFFF;
}

uint32_t b_parse_r_type(b_parser* parser) {
	b_token* inst = b_parser_expect_type(parser, T_INST);
	b_token* rd = b_parser_expect_type(parser, T_REG);
	b_token* rs1 = b_parser_expect_type(parser, T_REG);
	b_token* rs2 = b_parser_expect_type(parser, T_REG);

	uint32_t inst_index = index_of(INSTRUCTIONS, 0, N_INSTRUCTIONS, inst->lexeme);

	uint32_t rd_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rd->lexeme),
	                        index_of(PHYS_REGS, 0, N_REGS, rd->lexeme));

	uint32_t rs1_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs1->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs1->lexeme));

	uint32_t rs2_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs2->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs2->lexeme));

	uint32_t result = INSTRUCTION_TEMPLATES[inst_index];
	result |= (rd_index << 7);
	result |= (rs1_index << 15);
	result |= (rs2_index << 20);
	return result;
}

uint32_t b_parse_i_type(b_parser* parser) {
	b_token* inst = b_parser_expect_type(parser, T_INST);
	b_token* rd = b_parser_expect_type(parser, T_REG);
	b_token* rs1 = b_parser_expect_type(parser, T_REG);
	b_token* imm = b_parser_expect_type(parser, T_NUM);

	uint32_t inst_index = index_of(INSTRUCTIONS, 0, N_INSTRUCTIONS, inst->lexeme);

	uint32_t rd_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rd->lexeme),
	                        index_of(PHYS_REGS, 0, N_REGS, rd->lexeme));

	uint32_t rs1_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs1->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs1->lexeme));

	uint32_t imm_val = imm->imm_val;

	uint32_t result = INSTRUCTION_TEMPLATES[inst_index];
	result |= (rd_index << 7);
	result |= (rs1_index << 15);
	result |= (imm_val << 20);
	return result;
}

uint32_t b_parse_i_star_type(b_parser* parser) {
	b_token* inst = b_parser_expect_type(parser, T_INST);
	b_token* rd = b_parser_expect_type(parser, T_REG);
	b_token* rs1 = b_parser_expect_type(parser, T_REG);
	b_token* imm = b_parser_expect_type(parser, T_NUM);

	uint32_t inst_index = index_of(INSTRUCTIONS, 0, N_INSTRUCTIONS, inst->lexeme);

	uint32_t rd_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rd->lexeme),
	                        index_of(PHYS_REGS, 0, N_REGS, rd->lexeme));

	uint32_t rs1_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs1->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs1->lexeme));

	uint32_t imm_val = imm->imm_val & 0x1F;

	uint32_t result = INSTRUCTION_TEMPLATES[inst_index];
	result |= (rd_index << 7);
	result |= (rs1_index << 15);
	result |= (imm_val << 20);
	return result;
}

uint32_t b_parse_load(b_parser* parser) {
	b_token* inst = b_parser_expect_type(parser, T_INST);
	b_token* rd = b_parser_expect_type(parser, T_REG);
	b_token* imm = b_parser_expect_type(parser, T_NUM);
	b_parser_expect_type(parser, T_OPEN_PAREN);
	b_token* rs1 = b_parser_expect_type(parser, T_REG);
	b_parser_expect_type(parser, T_CLOSE_PAREN);

	uint32_t inst_index = index_of(INSTRUCTIONS, 0, N_INSTRUCTIONS, inst->lexeme);

	uint32_t rd_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rd->lexeme),
	                        index_of(PHYS_REGS, 0, N_REGS, rd->lexeme));

	uint32_t rs1_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs1->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs1->lexeme));

	uint32_t imm_val = imm->imm_val;

	uint32_t result = INSTRUCTION_TEMPLATES[inst_index];
	result |= (rd_index << 7);
	result |= (rs1_index << 15);
	result |= (imm_val << 20);
	return result;
}

uint32_t b_parse_store(b_parser* parser) {
	b_token* inst = b_parser_expect_type(parser, T_INST);
	b_token* rs2 = b_parser_expect_type(parser, T_REG);
	b_token* imm = b_parser_expect_type(parser, T_NUM);
	b_parser_expect_type(parser, T_OPEN_PAREN);
	b_token* rs1 = b_parser_expect_type(parser, T_REG);
	b_parser_expect_type(parser, T_CLOSE_PAREN);

	uint32_t inst_index = index_of(INSTRUCTIONS, 0, N_INSTRUCTIONS, inst->lexeme);

	uint32_t rs2_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs2->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs2->lexeme));

	uint32_t rs1_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs1->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs1->lexeme));

	uint32_t imm_val_4_0 = imm->imm_val & 0x1F;
	uint32_t imm_val_11_5 = (imm->imm_val & 0xFE0) >> 5;

	uint32_t result = INSTRUCTION_TEMPLATES[inst_index];
	result |= (imm_val_4_0 << 7);
	result |= (rs1_index << 15);
	result |= (rs2_index << 20);
	result |= (imm_val_11_5 << 25);
	return result;
}

uint32_t b_parse_branch(b_parser* parser) {
	b_token* inst = b_parser_expect_type(parser, T_INST);
	b_token* rs1 = b_parser_expect_type(parser, T_REG);
	b_token* rs2 = b_parser_expect_type(parser, T_REG);
	uint32_t imm_val; // NOTE: technically the immediate is supposed to be an offset, but here it's a literal PC value.
	if (b_parser_match_type(parser, T_NUM)) {
		b_token* imm = b_parser_expect_type(parser, T_NUM);
		imm_val = imm->imm_val;
	}
	else {
		b_token* imm = b_parser_expect_type(parser, T_LABEL);
		imm_val = b_parser_lookup(parser, imm->lexeme);
	}

	uint32_t inst_index = index_of(INSTRUCTIONS, 0, N_INSTRUCTIONS, inst->lexeme);

	uint32_t rs2_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs2->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs2->lexeme));

	uint32_t rs1_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rs1->lexeme),
	                         index_of(PHYS_REGS, 0, N_REGS, rs1->lexeme));

	uint32_t imm_12_10_5 = ((imm_val & 0x1000) >> 6) | ((imm_val & 0x7E0) >> 5);
	uint32_t imm_4_1_11 = (imm_val & 0x1E) | ((imm_val & 0x800) >> 11);

	uint32_t result = INSTRUCTION_TEMPLATES[inst_index];
	result |= (imm_4_1_11 << 7);
	result |= (rs1_index << 15);
	result |= (rs2_index << 20);
	result |= (imm_12_10_5 << 25);
	return result;
}

uint32_t b_parse_u_type(b_parser* parser) {
	b_token* inst = b_parser_expect_type(parser, T_INST);
	b_token* rd = b_parser_expect_type(parser, T_REG);
	b_token* imm = b_parser_expect_type(parser, T_NUM);
	uint32_t imm_val = imm->imm_val;

	uint32_t inst_index = index_of(INSTRUCTIONS, 0, N_INSTRUCTIONS, inst->lexeme);

	uint32_t rd_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rd->lexeme),
	                        index_of(PHYS_REGS, 0, N_REGS, rd->lexeme));

	uint32_t imm_31_12 = (imm_val & 0xFFFFF000);

	uint32_t result = INSTRUCTION_TEMPLATES[inst_index];
	result |= imm_31_12;
	result |= (rd_index << 7);
	return result;
}

uint32_t b_parse_jal(b_parser* parser) {
	b_parser_expect_type(parser, T_INST);
	b_token* rd = b_parser_expect_type(parser, T_REG);
	uint32_t imm_val; // NOTE: technically the immediate is supposed to be an offset, but here it's a literal PC value.
	if (b_parser_match_type(parser, T_NUM)) {
		b_token* imm = b_parser_expect_type(parser, T_NUM);
		imm_val = imm->imm_val;
	}
	else {
		b_token* imm = b_parser_expect_type(parser, T_LABEL);
		imm_val = b_parser_lookup(parser, imm->lexeme);
	}

	uint32_t rd_index = MAX(index_of(ARCH_REGS, 0, N_REGS, rd->lexeme),
	                        index_of(PHYS_REGS, 0, N_REGS, rd->lexeme));

	uint32_t imm_20 = (imm_val & 0x100000) << 11;
	uint32_t imm_10_1 = (imm_val & 0x7FE) << 20;
	uint32_t imm_11 = (imm_val & 0x800) << 9;
	uint32_t imm_19_12 = (imm_val & 0x000FF000);
	uint32_t imm_packed = (imm_20 | imm_10_1 | imm_11 | imm_19_12);

	uint32_t result = JAL_TEMPLATE;
	result |= imm_packed;
	result |= (rd_index << 7);
	return result;
}

uint32_t* b_parse_instructions(b_parser* parser, uint32_t* n_result) {
	uint32_t* output = NULL;
	uint32_t n_output = 0;
	parser->index = 0;

	// 2nd pass: everything else
	while (!b_parser_eos(parser)) {
		b_token* tok = b_parser_current(parser);
		if (tok->type == T_INST) {
			if (index_of(INSTRUCTIONS, R_START, R_END, tok->lexeme) != -1) {
				output = realloc(output, (n_output + 1) * sizeof(uint32_t));
				output[n_output++] = b_parse_r_type(parser);
			}
			else if (index_of(INSTRUCTIONS, I_START, I_END, tok->lexeme) != -1) {
				output = realloc(output, (n_output + 1) * sizeof(uint32_t));
				output[n_output++] = b_parse_i_type(parser);
			}
			else if (index_of(INSTRUCTIONS, I_STAR_START, I_STAR_END, tok->lexeme) != -1) {
				output = realloc(output, (n_output + 1) * sizeof(uint32_t));
				output[n_output++] = b_parse_i_star_type(parser);
			}
			else if (index_of(INSTRUCTIONS, LOAD_START, LOAD_END, tok->lexeme) != -1) {
				output = realloc(output, (n_output + 1) * sizeof(uint32_t));
				output[n_output++] = b_parse_load(parser);
			}
			else if (index_of(INSTRUCTIONS, STORE_START, STORE_END, tok->lexeme) != -1) {
				output = realloc(output, (n_output + 1) * sizeof(uint32_t));
				output[n_output++] = b_parse_store(parser);
			}
			else if (index_of(INSTRUCTIONS, BRANCH_START, BRANCH_END, tok->lexeme) != -1) {
				output = realloc(output, (n_output + 1) * sizeof(uint32_t));
				output[n_output++] = b_parse_branch(parser);
			}
			else if (index_of(INSTRUCTIONS, U_START, U_END, tok->lexeme) != -1) {
				output = realloc(output, (n_output + 1) * sizeof(uint32_t));
				output[n_output++] = b_parse_u_type(parser);
			}
			else if (strcmp(tok->lexeme, "jal") == 0) {
				output = realloc(output, (n_output + 1) * sizeof(uint32_t));
				output[n_output++] = b_parse_jal(parser);
			}
			else {
				fprintf(stderr, "Error: unsupported instruction `%s`.\n", b_token_type_str(tok->type));
				exit(1);
			}
		}
		else if (tok->type == T_LABEL) {
			// skip over label and colon
			b_parser_expect_type(parser, T_LABEL);
			b_parser_expect_type(parser, T_COLON);
		}
		else {
			fprintf(stderr, "Error: expected INST or LABEL, got `%s`.\n", b_token_type_str(tok->type));
			exit(1);
		}
	}

	*n_result = n_output;
	return output;
}
