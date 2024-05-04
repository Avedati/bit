#include "../include/globals.h"
#include <stdint.h>
#include <string.h>

const char* INSTRUCTIONS[] = {
	"add", "sub", "and", "or", "xor", "sll", "srl", "sra",
	"slt", "sltu", "jalr", "addi", "andi", "ori", "xori", "slti", "sltiu",
	"slli", "srli", "srai", "lb", "lbu", "lh", "lhu", "lw",
	"sb", "sh", "sw", "beq", "bge", "bgeu", "blt", "bltu",
	"bne", "auipc", "lui", "jal"
};
const uint32_t INSTRUCTION_TEMPLATES[] = {
	// R-type: funct7, rs2 (5), rs1 (5), funct3, rd (5), opcode (7)
	0x00000033,
	0x40000033,
	0x00007033,
	0x00006033,
	0x00004033,
	0x00001033,
	0x00005033,
	0x40005033,
	0x00002033,
	0x00003033,

	// I-type: imm (12), rs1 (5), funct3, rd (5), opcode (7)
	0x00000067,
	0x00000013,
	0x00007013,
	0x00006013,
	0x00004013,
	0x00002013,
	0x00003013,

	// I-star-type: funct7, imm (5), rs1 (5), funct3, rd (5), opcode (7)
	0x26001000,
	0x26005000,
	0x26005020,

	// Load: {I-type}
	0x00000003,
	0x00004003,
	0x00001003,
	0x00005003,
	0x00002003,

	// Store: imm[11:5] (7), rs2 (5), rs1 (5), funct3, imm[4:0] (5), opcode (7)
	0x00000023,
	0x00001023,
	0x00002023,

	// Branch: imm[12|10:5] (7), rs2 (5), rs1 (5), funct3, imm[4:1|11] (5), opcode (7)
	0x00000063,
	0x00005063,
	0x00007063,
	0x00004063,
	0x00006063,
	0x00001063,

	// U-type: imm[31:12] (20), rd (5), opcode (7)
	0x00000017,
	0x00000037,
};
const uint32_t JAL_TEMPLATE = 0x0000006F;
const unsigned int N_INSTRUCTIONS = 37;
const unsigned int R_START = 0;
const unsigned int R_END = 10;
const unsigned int I_START = 10;
const unsigned int I_END = 17;
const unsigned int I_STAR_START = 17;
const unsigned int I_STAR_END = 20;
const unsigned int LOAD_START = 20;
const unsigned int LOAD_END = 25;
const unsigned int STORE_START = 25;
const unsigned int STORE_END = 28;
const unsigned int BRANCH_START = 28;
const unsigned int BRANCH_END = 34;
const unsigned int U_START = 34;
const unsigned int U_END = 36;

const char* ARCH_REGS[] = {
	"x0" , "x1" , "x2" , "x3" , "x4" , "x5" , "x6" , "x7" ,
	"x8" , "x9" , "x10", "x11", "x12", "x13", "x14", "x15",
	"x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
	"x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31"
};
const char* PHYS_REGS[] = {
	"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
	"s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
	"a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
	"s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
const unsigned int N_REGS = 32;

// General function
int32_t index_of(const char* haystack[], uint32_t start, uint32_t end, char* needle) {
	for (uint32_t i = start; i < end; i++) {
		if (strcmp(haystack[i], needle) == 0) {
			return i;
		}
	}
	return -1;
}
