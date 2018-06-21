#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

namespace Rework{
class Console;
class Memory;

typedef enum mode {
	UNUSED,
	imp,
	acc,
	imm,
	zp,
	zpx,
	zpy,
	rel,
	abso,
	absx,
	absy,
	ind,
	indx,
	indy,
} MODE;

typedef enum Regs {
	reg_sign,
	reg_overflow,
	reg_ignored,
	reg_break,
	reg_decimal,
	reg_interrupt,
	reg_zero,
	reg_carry,
} REG;

typedef struct op_code_info {
	int8_t operand;
	uint16_t address;
	uint8_t mode;
} OP_CODE_INFO;

#define FLAG_CARRY 0
#define FLAG_ZERO 1
#define FLAG_INTERRUPT 2
#define FLAG_DECIMAL 3
#define FLAG_BREAK 4
#define FLAG_CONSTANT 5
#define FLAG_OVERFLOW 6
#define FLAG_SIGN 7
#define BASE_STACK_START 0x100

const static std::string opcodeToFunction[256] = {
    "brk", "ora", "fut", "fut", "fut", "ora", "asl", "fut",
    "php", "ora", "asl", "fut", "fut", "ora", "asl", "fut",
    "bpl", "ora", "fut", "fut", "fut", "ora", "asl", "fut",
    "clc", "ora", "fut", "fut", "fut", "ora", "asl", "fut",
    "jsr", "_and", "fut", "fut", "bit", "_and", "rol", "fut",
    "plp", "_and", "rol", "fut", "bit", "_and", "rol", "fut",
    "bmi", "_and", "fut", "fut", "fut", "_and", "rol", "fut",
    "sec", "_and", "fut", "fut", "fut", "_and", "rol", "fut",
    "rti", "eor", "fut", "fut", "fut", "eor", "lsr", "fut",
    "pha", "eor", "lsr", "fut", "jmp", "eor", "lsr", "fut",
    "bvc", "eor", "fut", "fut", "fut", "eor", "lsr", "fut",
    "cli", "eor", "fut", "fut", "fut", "eor", "lsr", "fut",
    "rts", "adc", "fut", "fut", "fut", "adc", "ror", "fut",
    "pla", "adc", "ror", "fut", "jmp", "adc", "ror", "fut",
    "bvs", "adc", "fut", "fut", "fut", "adc", "ror", "fut",
    "sei", "adc", "fut", "fut", "fut", "adc", "ror", "fut",
    "fut", "sta", "fut", "fut", "sty", "sta", "stx", "fut",
    "dey", "fut", "txa", "fut", "sty", "sta", "stx", "fut",
    "bcc", "sta", "fut", "fut", "sty", "sta", "stx", "fut",
    "tya", "sta", "txs", "fut", "fut", "sta", "fut", "fut",
    "ldy", "lda", "ldx", "fut", "ldy", "lda", "ldx", "fut",
    "tay", "lda", "tax", "fut", "ldy", "lda", "ldx", "fut",
    "bcs", "lda", "fut", "fut", "ldy", "lda", "ldx", "fut",
    "clv", "lda", "tsx", "fut", "ldy", "lda", "ldx", "fut",
    "cpy", "cmp", "fut", "fut", "cpy", "cmp", "dec", "fut",
    "iny", "cmp", "dex", "fut", "cpy", "cmp", "dec", "fut",
    "bne", "cmp", "fut", "fut", "fut", "cmp", "dec", "fut",
    "cld", "cmp", "fut", "fut", "fut", "cmp", "dec", "fut",
    "cpx", "sbc", "fut", "fut", "cpx", "sbc", "inc", "fut",
    "inx", "sbc", "nop", "fut", "cpx", "sbc", "inc", "fut",
    "beq", "sbc", "fut", "fut", "fut", "sbc", "inc", "fut",
    "sed", "sbc", "fut", "fut", "fut", "sbc", "inc", "fut",
};

const static uint8_t instructionSizes[256] = {
    1, 2, 0, 0, 2, 2, 2, 0, 
    1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0,
    1, 3, 1, 0, 3, 3, 3, 0,
    3, 2, 0, 0, 2, 2, 2, 0,
    1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 3, 1, 0, 3, 3, 3, 0,
    1, 2, 0, 0, 2, 2, 2, 0, 
    1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 3, 1, 0, 3, 3, 3, 0,
    1, 2, 0, 0, 2, 2, 2, 0, 
    1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 3, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 0, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 3, 1, 0, 0, 3, 0, 0,
    2, 2, 2, 0, 2, 2, 2, 0, 
    1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 3, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 3, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 
    1, 3, 1, 0, 3, 3, 3, 0
};

const static uint8_t cyclesPerInstruction[256] = {
	7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

const static uint8_t cyclesWhenPageCrossed[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};

const static uint8_t instructionModes[256] = {
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |     */
/* 0 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imm,  acc,  imm, abso, abso, abso, abso, /* 0 */
/* 1 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 1 */
/* 2 */    abso, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imm,  acc,  imm, abso, abso, abso, abso, /* 2 */
/* 3 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 3 */
/* 4 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imm,  acc,  imm, abso, abso, abso, abso, /* 4 */
/* 5 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 5 */
/* 6 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imm,  acc,  imm,  ind, abso, abso, abso, /* 6 */
/* 7 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 7 */
/* 8 */     imm, indx,  imm, indx,   zp,   zp,   zp,   zp,  imp,  imm,  imp,  imm, abso, abso, abso, abso, /* 8 */
/* 9 */     rel, indy,  imp, indy,  zpx,  zpx,  zpy,  zpy,  imp, absy,  imp, absy, absx, absx, absy, absy, /* 9 */
/* A */     imm, indx,  imm, indx,   zp,   zp,   zp,   zp,  imp,  imm,  imp,  imm, abso, abso, abso, abso, /* A */
/* B */     rel, indy,  imp, indy,  zpx,  zpx,  zpy,  zpy,  imp, absy,  imp, absy, absx, absx, absy, absy, /* B */
/* C */     imm, indx,  imm, indx,   zp,   zp,   zp,   zp,  imp,  imm,  imp,  imm, abso, abso, abso, abso, /* C */
/* D */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* D */
/* E */     imm, indx,  imm, indx,   zp,   zp,   zp,   zp,  imp,  imm,  imp,  imm, abso, abso, abso, abso, /* E */
/* F */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx  /* F */
};


class CPU {
private:
	// various registers
	uint16_t PC;
	uint16_t cycles;
	uint8_t SP, A, X, Y, Status;
	int8_t reg[8];
	// memory *mem;
	Console *nes;

	// helper variables
	int32_t insturctions;
	int32_t clockTicks6502, clockGoal6502;

	OP_CODE_INFO *getOpcodeInfo(int8_t operand, uint16_t address, uint8_t mode);

	uint8_t pullFromStack();
	void pushToStack(uint8_t val);
	void writeToMemory(uint16_t address, int8_t val);
	uint8_t readMemory(uint16_t address);
	bool pagesDiffer(uint16_t a, uint16_t b);
	void setMemory(Memory *mem);
	int8_t getStatusRegister();
	void setStatusRegister(uint8_t statusRegister);
	int8_t getCarryFlag();
	int8_t getZeroFlag();
	int8_t getInterruptFlag();
	int8_t getDecimalFlag();
	int8_t getBreakFlag();
	int8_t getOverflowFlag();
	int8_t getSignFlag();
	uint8_t getAccumulator();
	uint8_t getX();
	uint8_t getY();

	void setZeroFlag(uint8_t val);
	void setCarryFlag(int16_t val);
	void setSignFlag(int8_t val);
	void setOverflowFlagADC(int8_t accumulator, int16_t sum, int8_t operand);
	void setOverflowFlagSBC(int8_t accumulator, int16_t sub, int8_t opernad);
	void setOverflowFlag(int8_t val);
	void setBreakFlag(int8_t val);
	void setInterruptFlag(int8_t val);
	void setDecimalFlag(int8_t val);
	void setAccumulator(int8_t val);
	void setX(int8_t val);
	void setY(int8_t val);

	void adc(OP_CODE_INFO *o);
	void _and(OP_CODE_INFO *o);
	void asl(OP_CODE_INFO *o);
	void bcc(OP_CODE_INFO *o);
	void bcs(OP_CODE_INFO *o);
	void beq(OP_CODE_INFO *o);
	void bit(OP_CODE_INFO *o);
	void bmi(OP_CODE_INFO *o);
	void bne(OP_CODE_INFO *o);
	void bpl(OP_CODE_INFO *o);
	void brk(OP_CODE_INFO *o);
	void bvc(OP_CODE_INFO *o);
	void bvs(OP_CODE_INFO *o);
	void clc(OP_CODE_INFO *o);
	void cld(OP_CODE_INFO *o);
	void cli(OP_CODE_INFO *o);
	void clv(OP_CODE_INFO *o);
	void cmp(OP_CODE_INFO *o);
	void cpx(OP_CODE_INFO *o);
	void cpy(OP_CODE_INFO *o);
	void dec(OP_CODE_INFO *o);
	void dex(OP_CODE_INFO *o);
	void dey(OP_CODE_INFO *o);
	void eor(OP_CODE_INFO *o);
	void inc(OP_CODE_INFO *o);
	void inx(OP_CODE_INFO *o);
	void iny(OP_CODE_INFO *o);
	void jmp(OP_CODE_INFO *o);
	void jsr(OP_CODE_INFO *o);
	void lda(OP_CODE_INFO *o);
	void ldx(OP_CODE_INFO *o);
	void ldy(OP_CODE_INFO *o);
	void lsr(OP_CODE_INFO *o);
	void nop(OP_CODE_INFO *o);
	void ora(OP_CODE_INFO *o);
	void pha(OP_CODE_INFO *o);
	void php(OP_CODE_INFO *o);
	void pla(OP_CODE_INFO *o);
	void plp(OP_CODE_INFO *o);
	void rol(OP_CODE_INFO *o);
	void ror(OP_CODE_INFO *o);
	void rti(OP_CODE_INFO *o);
	void rts(OP_CODE_INFO *o);
	void sbc(OP_CODE_INFO *o);
	void sec(OP_CODE_INFO *o);
	void sed(OP_CODE_INFO *o);
	void sei(OP_CODE_INFO *o);
	void sta(OP_CODE_INFO *o);
	void stx(OP_CODE_INFO *o);
	void sty(OP_CODE_INFO *o);
	void tax(OP_CODE_INFO *o);
	void tay(OP_CODE_INFO *o);
	void tsx(OP_CODE_INFO *o);
	void txa(OP_CODE_INFO *o);
	void txs(OP_CODE_INFO *o);
	void tya(OP_CODE_INFO *o);
	void fut(OP_CODE_INFO *o);

public:
	void runFunction(OP_CODE_INFO *o, uint8_t opCode);
	uint8_t run();
	CPU (Console *_nes);
	~CPU();
	
};

};

#endif