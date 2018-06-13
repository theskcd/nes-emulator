// #include "memory.cpp"
#include "string.h"
// just get everything here
#include <bits/stdc++.h>
#include "console.cpp"

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

class CPU
{
private:
	// various registers
	uint16_t PC;
	uint8_t SP, A, X, Y, Status;
	int8_t reg[8];
	memory *mem;
	console *nes;

	// helper variables
	int32_t insturctions;
	int32_t clockTicks6502, clockGoal6502;

	OP_CODE_INFO *getOpcodeInfo(int8_t operand, uint16_t address, uint8_t mode) {
		OP_CODE_INFO *o = (OP_CODE_INFO*)malloc(sizeof(OP_CODE_INFO));
		o->operand = operand;
		o->address = address;
		o->mode = mode;
		return o;
	}

	uint8_t pullFromStack() {
		this->SP++;
		return this->console->cpuReadMemory((BASE_STACK_START | this->SP));
	}

	void pushToStack(uint8_t val) {
		this->console->cpuWriteMemory((BASE_STACK_START | this->SP), val);
		this->SP++;
		return ;
	}

	void writeToMemory(uint16_t address, int8_t val) {
		this->console->cpuWriteMemory(address, val);
		return ;
	}

	uint8_t readMemory(uint16_t address) {
		return this->console->cpuReadMemory(address);
	}
public:
	CPU(console *_nes) {
		this->PC = this->SP = this->A = this->X = this->Y = this->Status = 0;
		// default value always
		this->reg[reg_interrupt] = 1;
		this->nes = _nes;
	}

	void setMemory(Memory *mem) {
		this->memory = mem;
	}

	int8_t getStatusRegister() {
		int8_t statusRegister = 0;
		statusRegister |= (this->reg[reg_carry] >> FLAG_CARRY);
		statusRegister |= (this->reg[reg_zero] >> FLAG_ZERO);
		statusRegister |= (this->reg[reg_interrupt] >> FLAG_INTERRUPT);
		statusRegister |= (this->reg[reg_decimal] >> FLAG_DECIMAL);
		statusRegister |= (this->reg[reg_break] >> FLAG_DECIMAL);
		statusRegister |= (this->reg[reg_constant] >> FLAG_CONSTANT);
		statusRegister |= (this->reg[reg_overflow] >> FLAG_OVERFLOW);
		statusRegister |= (this->reg[reg_sign] >> FLAG_SIGN);
		this->Status = statusRegister;
		return statusRegister;	
	}

	void setStatusRegister(uint8_t statusRegister) {
		(statusRegister & FLAG_CARRY) ? this->reg[reg_carry] = 1 : this->reg[reg_carry] = 0;
		(statusRegister & FLAG_ZERO) ? this->reg[reg_zero] = 1 : this->reg[reg_zero] = 0;
		(statusRegister & FLAG_INTERRUPT) ? this->reg[reg_interrupt] = 1 : this->reg[reg_interrupt] = 0;
		(statusRegister & FLAG_DECIMAL) ? this->reg[reg_decimal] = 1 : this->reg[reg_decimal] = 0;
		(statusRegister & FLAG_BREAK) ? this->reg[reg_break] = 1 : this->reg[reg_break] = 0;
		(statusRegister & FLAG_CONSTANT) ? this->reg[reg_constant] = 1 : this->reg[reg_constant] = 0;
		(statusRegister & FLAG_OVERFLOW) ? this->reg[reg_overflow] = 1 : this->reg[reg_overflow] = 0;
		(statusRegister & FLAG_SIGN) ? this->reg[reg_sign] = 1 : this->reg[reg_sign] = 0;
	}

	int8_t getCarryFlag() { return this->reg[reg_carry]; }
	int8_t getZeroFlag() { return this->reg[reg_zero]; }
	int8_t getInterruptFlag() { return this->reg[reg_interrupt]; }
	int8_t getDecimalFlag() { return this->reg[reg_decimal]; }
	int8_t getBreakFlag() { return this->reg[reg_break]; }
	int8_t getOverflowFlag() { return this->reg[reg_overflow]; }
	int8_t getSignFlag() { return this->reg[reg_sign]; }
	uint8_t getAccumulator() { return this->A; }
	uint8_t getX() { return this->X; }
	uint8_t getY() { return this->Y; }


	void setZeroFlag(uint8_t val) {
		if (val == 0) this->reg[reg_zero] = 1;
		else this->reg[reg_zero] = 0;
	}

	void setCarryFlag(int16_t val) {
		(val > 0xFF) ? this->reg[reg_carry] = 1 : this->reg[reg_carry] = 0;
	}

	void setSignFlag(int8_t val) {
		if (val & 0x80) this->reg[reg_sign] = 1;
		else this->reg[reg_sign] = 0;
	}

	void setOverflowFlagADC(int8_t accumulator, int16_t sum, int8_t operand) {
		if (!((accumulator ^ operand) & 0x80) && ((accumulator ^ sum) & 0x80)) {
			this->reg[reg_overflow] = 1;
		} else {
			this->reg[reg_overflow] = 0;
		}
	}

	void setOverflowFlagSBC(int8_t accumulator, int16_t sub, int8_t operand) {
		if (!((accumulator ^ operand) & 0x80) && ((accumulator ^ sub) & 0x80)) {
			this->reg[reg_overflow] = 1;
		} else {
			this->reg[reg_overflow] = 0;
		}
	}

	void setOverflowFlag(int8_t val) {
		(val > 0) ? this->reg[reg_overflow] = 1 : this->reg[reg_overflow] = 0;
	}

	void setBreakFlag(int8_t val) {
		(val > 0) ? this->reg[reg_break] = 1 : this->reg[reg_break] = 0;
	}

	void setInterruptFlag(int8_t val) {
		(val > 0) ? this->reg[reg_interrupt] =  1 : this->reg[reg_interrupt] = 0;
	}

	void setDecimalFlag(int8_t val) {
		(val > 0) ? this->reg[reg_decimal] = 1 : this->reg[reg_decimal] = 0;
	}

	void setAccumulator(int8_t val) {
		this->A = val;
	}

	void setX(int8_t val) {
		this->X = val;
	}

	void setY(int8_t val) {
		this->Y = val;
	}


	// reference : http://www.obelisk.me.uk/6502/reference.html
	// reference : http://nesdev.com/6502.txt
	void adc(OP_CODE_INFO *o) {
		int8_t carry = this->getCarryFlag();
		uint8_t accumulator = this->getAccumulator();
		uint8_t operand = o->operand;
		int16_t sum = (0x00FF & carry) + (0x00FF & accumulator) + (0x00FF & operand);
		uint8_t sumInByte = (0xFF & sum);
		this->setZeroFlag(sumInByte);
		this->setCarryFlag(sum);
		this->setOverflowFlagADC(accumulator, sumInByte, o->operand);
		this->setSignFlag(sumInByte);
		this->setAccumulator(sumInByte);
	}

	void _and(OP_CODE_INFO *o) {
		int8_t accumulator = this->getAccumulator();
		int8_t operand = o->operand;
		int8_t res = accumulator & operand;
		this->setSignFlag(res);
		this->setZeroFlag(res);
		this->setAccumulator(res);
	}

	void asl(OP_CODE_INFO *o) {
		int16_t res = (0x00FF & o->operand) << 1;
		int8_t resInByte = res & 0xFF;
		this->setCarryFlag(res);
		this->setZeroFlag(resInByte);
		this->setSignFlag(resInByte);
		if (o->mode == acc) {
			this->setAccumulator(resInByte);
		} else {
			// TODO : implement the function
			this->writeToMemory(o->address, resInByte);
		}
	}

	// TODO : missing clock
	void bcc(OP_CODE_INFO *o) {
		if (!this->getCarryFlag()) {
			this->PC = o->address;
		}
	}

	void bcs(OP_CODE_INFO *o) {
		if (this->getCarryFlag()) {
			this->PC = o->address;
		}
	}

	void beq(OP_CODE_INFO *o) {
		if (this->getZeroFlag()) {
			this->PC = o->address;
		}
	}

	void bit(OP_CODE_INFO *o) {
		this->setSignFlag(o->operand);
		this->setOverflowFlag((o->operand & 0x40) ? 1 : 0);
		this->setZeroFlag((o->operand & this->getAccumulator()));
	}

	void bmi(OP_CODE_INFO *o) {
		if (this->getSignFlag()) {
			this->PC = o->address;
		}
	}

	void bne(OP_CODE_INFO *o) {
		if (!this->getZeroFlag()) {
			this->PC = o->address;
		}
	}

	void bpl(OP_CODE_INFO *o) {
		if (!this->getSignFlag()) {
			this->PC = o->address;
		}
	}

	void brk(OP_CODE_INFO *o) {
		this->PC++;
		int8_t returnAddressHighBits = ((this->PC >> 8) & 0xFF);
		this->pushToStack(returnAddressHighBits);
		int8_t returnAddressLowerBits = (this->PC & 0xFF);
		this->pushToStack(returnAddressLowerBits);
		this->setBreakFlag(1);
		this->pushToStack(this->getStatusRegister());
		this->setInterruptFlag(1);
		this->PC = (this->loadFromMemory(0xFFFE) | (this->LoadFromMemory(0xFFFF) << 8));
	}

	void bvc(OP_CODE_INFO *o) {
		if (!this->getOverflowFlag()) {
			this->PC = o->address;
		}
	}

	void bvs(OP_CODE_INFO *o) {
		if (this->getOverflowFlag()) {
			this->PC = o->address;
		}
	}

	void clc(OP_CODE_INFO *o) {this->setCarryFlag(0);}
	void cld(OP_CODE_INFO *o) {this->setDecimalFlag(0);}
	void cli(OP_CODE_INFO *o) {this->setInterruptFlag(0);}
	void clv(OP_CODE_INFO *o) {this->setOverflowFlag(0);}

	void cmp(OP_CODE_INFO *o) {
		int8_t src = this->getAccumulator() - o->operand;
		int16_t longSrc = (0x00FF & this->getAccumulator()) - (0x00FF & o->operand);
		this->setCarryFlag(longSrc < 0x100);
		this->setSignFlag(src);
		this->setZeroFlag(src);
	}

	void cpx(OP_CODE_INFO *o) {
		int8_t src = this->getX() - o->operand;
		int16_t longSrc = (0x00FF & this->getX()) - (0x00FF & o->operand);
		this->setCarryFlag(longSrc < 0x100);
		this->setSignFlag(src);
		this->setZeroFlag(src);
	}

	void cpy(OP_CODE_INFO *o) {
		int8_t src = this->getY() - o->operand;
		int16_t longSrc = (0x00FF & this->getY()) - (0x00FF & o->operand);
		this->setCarryFlag(longSrc < 0x100);
		this->setSignFlag(src);
		this->setZeroFlag(src);
	}

	void dec(OP_CODE_INFO *o) {
		uint8_t src = o->operand - 1;
		src = src & 0xFF;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->writeToMemory(o->address, src);
	}

	void dex(OP_CODE_INFO *o) {
		uint8_t src = this->getX();
		src = (src - 1) & 0xFF;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setX(src);
	}

	void dey(OP_CODE_INFO *o) {
		uint8_t src = this->getY();
		src = (src - 1) & 0xFF;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setY(src);
	}

	void eor(OP_CODE_INFO *o) {
		int8_t src = o->operand ^ this->getAccumulator();
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setAccumulator(src);
	}

	void inc(OP_CODE_INFO *o) {
		uint8_t src = o->operand;
		src = src + 1;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->writeToMemory(o->address, src);
	}

	void inx(OP_CODE_INFO *o) {
		uint8_t src = this->getX();
		src = src + 1;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setX(src);
	}

	void iny(OP_CODE_INFO *o) {
		uint8_t src = this->getY();
		src = src + 1;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setY(src);
	}

	void jmp(OP_CODE_INFO *o) {
		// convert to uint16_t
		this->PC = (0x00FF & o->address);
	}

	void jsr(OP_CODE_INFO *o) {
		this->PC--;
		uint8_t returnAddressHighBits = ((this->PC >> 8) & 0xFF);
		uint8_t returnAddressLowerBits = (this->PC & 0xFF);
		this->pushToStack(returnAddressHighBits);
		this->pushToStack(returnAddressLowerBits);
		this->PC = o->address;
	}

	void lda(OP_CODE_INFO *o) {
		this->setSignFlag(o->operand);
		this->setZeroFlag(o->operand);
		this->setAccumulator(o->operand);
	}

	void ldx(OP_CODE_INFO *o) {
		this->setSignFlag(o->operand);
		this->setZeroFlag(o->operand);
		this->setX(o->operand);
	}

	void ldy(OP_CODE_INFO *o) {
		this->setSignFlag(o->operand);
		this->setZeroFlag(o->operand);
		this->setY(o->operand);
	}

	void lsr(OP_CODE_INFO *o) {
		this->setCarryFlag((o->operand & 0x01) > 0 ? 1 : 0);
		int8_t src = o->operand;
		src >>= 1;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		if (o->mode == acc) {
			this->setAccumulator(src);
		} else {
			this->writeToMemory(o->address, src);
		}
	}

	void nop(OP_CODE_INFO *o) {
		// do nothing
	}

	void ora(OP_CODE_INFO *o) {
		int8_t src = o->operand;
		src |= this->getAccumulator();
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setAccumulator(src);
	}

	void pha(OP_CODE_INFO *o) {
		this->pushToStack(this->getAccumulator());
	}

	void php(OP_CODE_INFO *o) {
		this->pushToStack(this->getStatusRegister());
	}

	// TODO : implement pullFromStack function
	void pla(OP_CODE_INFO *o) {
		int8_t src = this->pullFromStack();
		this->setAccumulator(src);
		this->setSignFlag(src);
		this->setZeroFlag(src);
	}

	// TODO : implement setStatusRegister function
	void plp(OP_CODE_INFO *o) {
		uint8_t src = this->pullFromStack();
		this->setStatusRegister(src);
	}

	void rol(OP_CODE_INFO *o) {
		uint16_t longSrc = o->operand;
		longSrc <<= 1;
		if (this->getCarryFlag()) {
			longSrc |= 0x1;
		}
		uint8_t src = longSrc & 0xFF;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		if (o->mode == acc) {
			this->setAccumulator(src);
		} else {
			this->writeToMemory(o->address, src);
		}
	}

	void ror(OP_CODE_INFO *o) {
		uint16_t longSrc = o->operand;
		if (this->getCarryFlag()) longSrc |= 0x100;
		uint8_t src = longSrc & 0xFF;
		this->setCarryFlag((src & 0x01) > 0 ? 1 : 0);
		longSrc >>= 1;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		if (o->mode == acc) {
			this->setAccumulator(src);
		} else {
			this->writeToMemory(o->address, src);
		}
	}

	void rti(OP_CODE_INFO *o) {
		uint8_t src = this->pullFromStack();
		this->setStatusRegister(src);
		uint8_t lowerBits = this->pullFromStack();
		uint8_t upperBits = this->pullFromStack();
		uint16_t address = ((upperBits << 8) | lowerBits);
		this->PC = address;
	}

	void rts(OP_CODE_INFO *o) {
		uint8_t lowerByte = this->pullFromStack();
		uint8_t upperByte = this->pullFromStack();
		uint16_t src = ((upperByte << 8) | lowerByte);
		src += 1;
		this->PC = src;
	}

	void sbc(OP_CODE_INFO *o) {
		uint16_t longRes = (0x00FF & this->getAccumulator()) - (0x00FF & o->operand) - (this->getCarryFlag() ? 1 : 0);
		uint8_t res = longRes & 0xFF;
		this->setSignFlag(res);
		this->setZeroFlag(res);
		this->setOverflowFlagSBC(this->getAccumulator(), longRes, o->operand);
		this->setCarryFlag((longRes < 0x100) ? 1 : 0);
		this->setAccumulator(res);
	}

	void sec(OP_CODE_INFO *o) {this->setCarryFlag(1);}
	void sed(OP_CODE_INFO *o) {this->setDecimalFlag(1);}
	void sei(OP_CODE_INFO *o) {this->setInterruptFlag(1);}
	void sta(OP_CODE_INFO *o) {this->writeToMemory(o->address, this->getAccumulator());}
	void stx(OP_CODE_INFO *o) {this->writeToMemory(o->address, this->getX());}
	void sty(OP_CODE_INFO *o) {this->writeToMemory(o->address, this->getY());}

	void tax(OP_CODE_INFO *o) {
		uint8_t src = this->getAccumulator();
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setX(src);
	}

	void tay(OP_CODE_INFO *o) {
		uint8_t src = this->getAccumulator();
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setY(src);
	}

	void tsx(OP_CODE_INFO *o) {
		uint8_t src = this->SP;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setX(src);
	}

	void txa(OP_CODE_INFO *o) {
		uint8_t src = this->getX();
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setAccumulator(src);
	}

	void txs(OP_CODE_INFO *o) {
		uint8_t src = this->getX();
		this->SP = src;
	}

	void tya(OP_CODE_INFO *o) {
		uint8_t src = this->getY();
		this->setSignFlag(src);
		this->setZeroFlag(src);
		this->setAccumulator(src);
	}

	void fut(OP_CODE_INFO *o) {
		// do nothing :|
		// these opcodes arent required for NES.
	}

	// This might be the ugliest thing I have ever done :|
	// FML.
	void runFunction(OP_CODE_INFO *o, uint8_t opCode) {
		std::string functionName = opcodeToFunction[opCode];
		// >BULLSHIT.exe
		if (functionName ==  "adc") {this->adc(o); return;}
		if (functionName ==  "and") {this->_and(o); return;}
		if (functionName ==  "asl") {this->asl(o); return;}
		if (functionName ==  "bcc") {this->bcc(o); return;}
		if (functionName ==  "bcs") {this->bcs(o); return;}
		if (functionName ==  "beq") {this->beq(o); return;}
		if (functionName ==  "bit") {this->bit(o); return;}
		if (functionName ==  "bmi") {this->bmi(o); return;}
		if (functionName ==  "bne") {this->bne(o); return;}
		if (functionName ==  "bpl") {this->bpl(o); return;}
		if (functionName ==  "brk") {this->brk(o); return;}
		if (functionName ==  "bvc") {this->bvc(o); return;}
		if (functionName ==  "clc") {this->clc(o); return;}
		if (functionName ==  "cld") {this->cld(o); return;}
		if (functionName ==  "cli") {this->cli(o); return;}
		if (functionName ==  "cmp") {this->cmp(o); return;}
		if (functionName ==  "cpx") {this->cpx(o); return;}
		if (functionName ==  "cpy") {this->cpy(o); return;}
		if (functionName ==  "dec") {this->dec(o); return;}
		if (functionName ==  "dex") {this->dex(o); return;}
		if (functionName ==  "dey") {this->dey(o); return;}
		if (functionName ==  "fut") {this->fut(o); return;}
		if (functionName ==  "inc") {this->inc(o); return;}
		if (functionName ==  "inx") {this->inx(o); return;}
		if (functionName ==  "iny") {this->iny(o); return;}
		if (functionName ==  "jmp") {this->jmp(o); return;}
		if (functionName ==  "jsr") {this->jsr(o); return;}
		if (functionName ==  "lda") {this->lda(o); return;}
		if (functionName ==  "ldx") {this->ldx(o); return;}
		if (functionName ==  "ldy") {this->ldy(o); return;}
		if (functionName ==  "lsr") {this->lsr(o); return;}
		if (functionName ==  "nop") {this->nop(o); return;}
		if (functionName ==  "ora") {this->ora(o); return;}
		if (functionName ==  "pha") {this->pha(o); return;}
		if (functionName ==  "php") {this->php(o); return;}
		if (functionName ==  "pla") {this->pla(o); return;}
		if (functionName ==  "plp") {this->plp(o); return;}
		if (functionName ==  "rts") {this->rts(o); return;}
		if (functionName ==  "sbc") {this->sbc(o); return;}
		if (functionName ==  "sec") {this->sec(o); return;}
		if (functionName ==  "sta") {this->sta(o); return;}
		if (functionName ==  "stx") {this->stx(o); return;}
		if (functionName ==  "sty") {this->sty(o); return;}
		if (functionName ==  "tya") {this->tya(o); return;}
		if (functionName ==  "txa") {this->txa(o); return;}
		if (functionName ==  "tax") {this->tax(o); return;}
		if (functionName ==  "eor") {this->eor(o); return;}
		if (functionName ==  "rti") {this->rti(o); return;}
		if (functionName ==  "rol") {this->rol(o); return;}
		if (functionName ==  "ror") {this->ror(o); return;}
		if (functionName ==  "sei") {this->sei(o); return;}
		if (functionName ==  "txs") {this->txs(o); return;}
		if (functionName ==  "sed") {this->sed(o); return;}
	}

	void run() {
		uint8_t opCode = this->readMemory(this->PC);
		OP_CODE_INFO *o;
		uint8_t mode = instructionModes[opCode];
		switch(mode) {
			case acc: {
				uint8_t operand = this->getAccumulator();
				o = this->getOpcodeInfo(operand, 0, mode);
				break;
			}
			case abso: {
				uint8_t lowerByte = this->readMemory(this->PC + 1);
				uint8_t upperByte = this->readMemory(this->PC + 2);
				uint16_t address = (upperByte << 8) | lowerByte;
				o = this->getOpcodeInfo(0, address, mode);
				break;
			}
			case absx: {
				uint8_t lowerByte = this->readMemory(this->PC + 1);
				uint8_t upperByte = this->readMemory(this->PC + 2);
				uint16_t address = (upperByte << 8) | lowerByte;
				uint16_t xVal = this->getX();
				o = this->getOpcodeInfo(0, address + xVal, mode);
				break;
			}
			case absy: {
				uint8_t lowerByte = this->readMemory(this->PC + 1);
				uint8_t upperByte = this->readMemory(this->PC + 2);
				uint16_t address = (upperByte << 8) | lowerByte;
				uint16_t yVal = this->getY();
				o = this->getOpcodeInfo(0, address + yVal, mode);
				break;
			}
			case imm: {
				uint8_t operand = this->readMemory(this->PC + 1);
				o = this->getOpcodeInfo(operand, this->PC + 1, mode);
				break;	
			}
			case imp: {
				o = this->getOpcodeInfo(0, this->PC, mode);
				break;
			}
			case ind: {
				uint16_t address = (this->readMemory(this->PC + 2) << 8) | this->readMemory(this->PC + 1);
				uint16_t finalAddress = (this->readMemory(address + 1) << 8) | this->readMemory(address);
				o = this->getOpcodeInfo(0, finalAddress, mode);
				break;
			}
			case indx: {
				uint16_t memoryContent = this->readMemory(this->PC + 1);
				uint16_t xVal = this->getX();
				uint8_t lowerByte = this->readMemory(memoryContent + xVal);
				uint8_t upperByte = this->readMemory(memoryContent + xVal + 1);
				uint8_t operand = this->readMemory((upperByte << 8) | lowerByte);
				o = this->getOpcodeInfo(operand, (upperByte << 8) | lowerByte, mode);
				break;
			}
			case indy: {
				uint16_t memoryContent = this->readMemory(this->PC + 1);
				uint8_t lowerByte = this->readMemory(memoryContent);
				uint8_t upperByte = this->readMemory(memoryContent + 1);
				uint16_t yVal = this->getY();
				uint16_t finalAddress = ((upperByte << 8) | lowerByte) + yVal;
				uint8_t operand = this->readMemory(finalAddress);
				o = this->getOpcodeInfo(operand, finalAddress, mode);
				break;
			}
			case rel: {
				int16_t offset = this->readMemory(this->PC + 1);
				uint16_t address = this->PC + 2 + offset;
				o = this->getOpcodeInfo(0, address, mode);
				break;
			}
			case zp: {
				uint16_t address = 0x00FF & this->readMemory(this->PC + 1);
				uint8_t operand = this->readMemory(address);
				o = this->getOpcodeInfo(operand, address, mode);
			}
			case zpx: {
				uint16_t address = 0x00FF & this->readMemory(this->PC + 1);
				uint16_t finalAddress = address + (0x00FF & this->getX());
				uint8_t operand = this->readMemory(finalAddress);
				o = this->getOpcodeInfo(operand, finalAddress, mode);
				break;
			}
			case zpy: {
				uint16_t address = 0x00FF & this->readMemory(this->PC + 1);
				uint16_t finalAddress = address + (0x00FF & this->getY());
				uint8_t operand = this->readMemory(finalAddress);
				o = this->getOpcodeInfo(operand, finalAddress, mode);
				break;
			}
		}
		this->PC += instructionSizes[opCode];
		this->runFunction(o, opCode);
	}
};