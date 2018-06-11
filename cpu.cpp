#include "cpu.hpp"

typedef enum mode {
	UNUSED,
	implicit,
	accumulator,
	immediate,
	zeroPage,
	zeroPageX,
	zeroPageY,
	relative,
	absolute,
	absoluteX,
	absoluteY,
	indirect,
	indexedIndirect,
	indirectIndexed,
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
	MODE mode;
} OP_CODE_INFO;

#define FLAG_CARRY 0x01
#define FLAG_ZERO 0x02
#define FLAG_INTERRUPT 0x04
#define FLAG_DECIMAL 0x08
#define FLAG_BREAK 0x10
#define FLAG_CONSTANT 0x20
#define FLAG_OVERFLOW 0x40
#define FLAG_SIGN 0x80
#define BASE_STACK_START 0x100

class cpu
{
private:
	// various registers
	int16_t PC;
	int8_t SP, A, X, Y, Status;
	int8_t reg[8];
	memory *mem;

	// helper variables
	int32_t insturctions;
	int32_t clockTicks6502, clockGoal6502;
public:
	int8_t getStatusRegister(CPU *c) {
		//qwe
	}
	cpu() {
		this->mem = new memory();
		this->PC = this->SP = this->A = this->X = this->Y = this->Status = 0;
		// default value always
		this->reg[reg_constant] = 1;
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

	int8_t getCarryFlag() { return this->reg[reg_carry]; }
	int8_t getZeroFlag() { return this->reg[reg_zero]; }
	int8_t getInterruptFlag() { return this->reg[reg_interrupt]; }
	int8_t getDecimalFlag() { return this->reg[reg_decimal]; }
	int8_t getBreakFlag() { return this->reg[reg_break]; }
	int8_t getConstantFlag() { return this->reg[reg_fifth]; }
	int8_t getOverflowFlag() { return this->reg[reg_overflow]; }
	int8_t getSignFlag() { return this->reg[reg_sign]; }
	int8_t getAccumulator() { return this->A; }
	int8_t getX() { return this->X; }
	int8_t getY() { return this->Y; }


	void setZeroFlag(int8_t val) {
		if (val == 0) this->reg[reg_zero] = 1;
		else this->reg[reg_zero] = 0;
	}

	void setCarryFlag(int16_t val) {
		(val > 0xFF) ? this->reg[reg_carry] = 1, this->reg[reg_carry] = 0;
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
		(val > 0) ? this->reg[reg_overflow] = 1, this->reg[reg_overflow] = 0;
	}

	void setBreakFlag(int8_t val) {
		(val > 0) ? this->reg[reg_break] = 1, this->reg[reg_break] = 0;
	}

	void setInterruptFlag(int8_t val) {
		(val > 0) ? this->reg[reg_interrupt] = 1, this->reg[reg_interrupt] = 0;
	}

	void setDecimalFlag(int8_t val) {
		(val > 0) ? this->reg[reg_decimal] = 1, this->reg[reg_decimal] = 0;
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
		int8_t accumulator = this->getAccumulator();
		int8_t operand = o->operand;
		int16_t sum = (0x00FF & carry) + (0x00FF & accumulator) + (0x00FF & operand);
		int8_t sumInByte = (0xFF & sum);
		this->setZeroFlag(sumInByte);
		this->setCarryFlag(sum);
		this->setOverflowFlagADC(accumulator, sumInByte, o->operand);
		this->setNegativeFlag(sumInByte);
		this->setAccumulator(sumInByte);
	}

	void and(OP_CODE_INFO *o) {
		int8_t accumulator = this->getAccumulator();
		int8_t operand = o->operand;
		int8_t res = accumulator & operand;
		this->setNegativeFlag(res);
		this->setZeroFlag(res);
		this->setAccumulator(res);
	}

	void asl(OP_CODE_INFO *o) {
		int16_t res = (0x00FF & o->operand) << 1;
		int8_t resInByte = res & 0xFF;
		this->setCarryFlag(res);
		this->setZeroFlag(resInByte);
		this->setNegativeFlag(resInByte);
		if (o->mode == accumulator) {
			this->setAccumulator(resInByte);
		} else {
			// TODO : implement the function
			this->writeToMemory(resInByte, o->address);
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
		this->setZeroFlag(o->operand & this->getAccumulator());
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
		if (!this->getOverflowFlag()) {
			this->PC = o->address;
		}
	}

	void clc(OP_CODE_INFO *o) {this->setCarryFlag(0);}
	void cld(OP_CODE_INFO *o) {this->setDecimalFlag(0);}
	void cli(OP_CODE_INFO *o) {this->setInterruptFlag(0);}
	void clv(OP_CODE_INFO *o) {this->setOverflowFlag(0);}

	void cmp(OP_CODE_INFO *o) {
		int8_t src = this->getAccumulator() - o->operand;
		int16_t longSrc = (int16_t)this->getAccumulator() - (int16_t)o->operand;
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

	// check if uint8_t is the correct datatype to use.
	// it probably is because of the overflow which might occur
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
		this->PC = o->address;
	}

	void jsr(OP_CODE_INFO *o) {
		this->PC--;
		int8_t returnAddressHighBits = ((this->PC >> 8) & 0xFF);
		int8_t returnAddressLowerBits = (this->PC & 0xFF);
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
		this->setZeroFlag(o->operand):
		this->setY(o->operand);
	}

	void lsr(OP_CODE_INFO *o) {
		this->setCarryFlag((o->operand & 0x01) > 0 ? 1 : 0);
		int8_t src = o->operand;
		src >>= 1;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		if (o->mode == accumulator) {
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
		this->setSignFlag(src);
		this->setZeroFlag(src);
	}

	// TODO : implement setStatusRegister function
	void plp(OP_CODE_INFO *o) {
		int8_t src = this->pullFromStack();
		this->setStatusRegister(src);
	}

	void rol(OP_CODE_INFO *o) {
		int16_t longSrc = o->operand;
		longSrc <<= 1;
		if (this->getCarryFlag()) {
			longSrc |= 0x1;
		}
		int8_t src = longSrc & 0xFF;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		if (o->mode == accumulator) {
			this->setAccumulator(src);
		} else {
			this->writeToMemory(o->address, src);
		}
	}

	void ror(OP_CODE_INFO *o) {
		int16_t longSrc = o->operand;
		if (this->getCarryFlag()) longSrc |= 0x100;
		this->setCarryFlag((src & 0x01) > 0 ? 1 : 0);
		longSrc >>= 1;
		int8_t src = longSrc & 0xFF;
		this->setSignFlag(src);
		this->setZeroFlag(src);
		if (o->mode == accumulator) {
			this->setAccumulator(src);
		} else {
			this->writeToMemory(o->address, src);
		}
	}

	void rti(OP_CODE_INFO *o) {
		int8_t src = this->pullFromStack();
		this->setStatusRegister(src);
		uint8_t lowerBits = this->pullFromStack();
		uint8_t upperBits = this->pullFromStack();
		uint16_t address = ((upperBits << 8) | lowerBits);
		this->PC = address;
	}

	void rts(OP_CODE_INFO *o) {
		uint16_t src = 0x00FF & this->pullFromStack();
		src += (this->pullFromStack() << 8) + 1;
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
};

void (*opcodeToFunction[256])(CPU *c, OP_CODE_INFO *o) = {
    brk, ora, fut, fut, fut, ora, asl, fut,
    php, ora, asl, fut, fut, ora, asl, fut,
    bpl, ora, fut, fut, fut, ora, asl, fut,
    clc, ora, fut, fut, fut, ora, asl, fut,
    jsr, and, fut, fut, bit, and, rol, fut,
    plp, and, rol, fut, bit, and, rol, fut,
    bmi, and, fut, fut, fut, and, rol, fut,
    sec, and, fut, fut, fut, and, rol, fut,
    rti, eor, fut, fut, fut, eor, lsr, fut,
    pha, eor, lsr, fut, jmp, eor, lsr, fut,
    bvc, eor, fut, fut, fut, eor, lsr, fut,
    cli, eor, fut, fut, fut, eor, lsr, fut,
    rts, adc, fut, fut, fut, adc, ror, fut,
    pla, adc, ror, fut, jmp, adc, ror, fut,
    bvs, adc, fut, fut, fut, adc, ror, fut,
    sei, adc, fut, fut, fut, adc, ror, fut,
    fut, sta, fut, fut, sty, sta, stx, fut,
    dey, fut, txa, fut, sty, sta, stx, fut,
    bcc, sta, fut, fut, sty, sta, stx, fut,
    tya, sta, txs, fut, fut, sta, fut, fut,
    ldy, lda, ldx, fut, ldy, lda, ldx, fut,
    tay, lda, tax, fut, ldy, lda, ldx, fut,
    bcs, lda, fut, fut, ldy, lda, ldx, fut,
    clv, lda, tsx, fut, ldy, lda, ldx, fut,
    cpy, cmp, fut, fut, cpy, cmp, dec, fut,
    iny, cmp, dex, fut, cpy, cmp, dec, fut,
    bne, cmp, fut, fut, fut, cmp, dec, fut,
    cld, cmp, fut, fut, fut, cmp, dec, fut,
    cpx, sbc, fut, fut, cpx, sbc, inc, fut,
    inx, sbc, nop, fut, cpx, sbc, inc, fut,
    beq, sbc, fut, fut, fut, sbc, inc, fut,
    sed, sbc, fut, fut, fut, sbc, inc, fut
};

uint8_t instructionSizes[256] = {
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

uint8_t instructionModes[256] = {
    6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    1, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
    5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
    5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2
};