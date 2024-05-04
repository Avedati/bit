#ifndef __GLOBAL_H_
#define __GLOBAL_H_ 0
#include <stdint.h>

// General macro
#define MAX(a, b) ((a < b) ? b : a)

extern const char* INSTRUCTIONS[];
extern const uint32_t INSTRUCTION_TEMPLATES[];
extern const uint32_t JAL_TEMPLATE;
extern const unsigned int N_INSTRUCTIONS;
extern const unsigned int R_START;
extern const unsigned int R_END;
extern const unsigned int I_START;
extern const unsigned int I_END;
extern const unsigned int I_STAR_START;
extern const unsigned int I_STAR_END;
extern const unsigned int LOAD_START;
extern const unsigned int LOAD_END;
extern const unsigned int STORE_START;
extern const unsigned int STORE_END;
extern const unsigned int BRANCH_START;
extern const unsigned int BRANCH_END;
extern const unsigned int U_START;
extern const unsigned int U_END;
extern const char* ARCH_REGS[];
extern const char* PHYS_REGS[];
extern const unsigned int N_REGS;

int32_t index_of(const char* haystack[], uint32_t start, uint32_t end, char* needle);

#endif // __GLOBAL_H_
