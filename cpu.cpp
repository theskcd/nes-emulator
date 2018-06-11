#include "cpu.hpp"

typedef struct cpu {
	// various registers
	uint16_t PC;
	uint8_t SP, A, X, Y, Status;
	uint8_t reg[8];

	// helper variables
	uint32_t insturctions;
	uint32_t clockTicks6502, clockGoal6502;
} CPU;

int8_t getCarryFlag(CPU *c) {
	return c->reg[reg_carry];
}

int8_t getZeroFlag(CPU *c) {
	return c->reg[reg_zero];
}

int8_t getInterruptFalg(CPU *c) {
	return c->reg[reg_interrupt];
}

int8_t getDecimalFlag(CPU *c) {
	return c->reg[reg_decimal];
}

int8_t getBreakFlag(CPU *c) {
	return c->reg[reg_break];
}

int8_t getFifthFlag(CPU *c) {
	return c->reg[reg_fifth];
}

int8_t getOverflowFlag(CPU *c) {
	return c->reg[reg_overflow];
}

int8_t getSignFlag(CPU *c) {
	return c->reg[reg_negative];
}

void setZeroFlag(CPU *c, int8_t val) {
	if (val == 0) {
		c->reg[reg_zero] = 1;
	} else {
		c->reg[reg_zero] = 0;
	}
}

void setCarryFlag(CPU *c, int16_t val) {
	(val > 0xFF) ? c->reg[reg_carry] = 1, c->reg[reg_carry] = 0;
}

void setNegativeFlag(CPU *c, int8_t val) {
	if (val & 0x80) {
		c->reg[reg_negative] = 1;
	} else {
		c->reg[reg_negative] = 0;
	}
}

void setOverflowFlag(CPU *c, int8_t accumulator, int16_t sum, int8_t operand) {
	if (!((accumulator ^ operand) & 0x80) && ((accumulator ^ sum) & 0x80)) {
		c->reg[reg_overflow] = 1;
	} else {
		c->reg[reg_overflow] = 0;
	}
}

// for reference http://www.obelisk.me.uk/6502/reference.html
void adc(CPU *c, OP_CODE_INFO *o) {
	int8_t carry = getCarryFlag(c);
	int8_t accumulator = c->A;
	int8_t operand = o->operand;
	int16_t sum = (0x00FF & carry) + (0x00FF & accumulator) + (0x00FF & operand);
	int8_t sumInByte = (0xFF & sum);
	setZeroFlag(c, sumInByte);
	setCarryFlag(c, sum);
	setOverflowFlag(c, accumulator, sumInByte, o->operand);
	setNegativeFlag(c, sumInByte);
	c->A = sumInByte;
}

void and(CPU *c, OP_CODE_INFO *o) {
	int8_t accumulator = c->A;
	int8_t operand = o->operand;
	int8_t res = accumulator & operand;
	setNegativeFlag(c, res);
	setZeroFlag(c, res);
	c->A = res;
}

void asl(CPU *c, OP_CODE_INFO *o) {
	int16_t res = (0x00FF & o->operand) << 1;
	int8_t resInByte = res & 0xFF;
	setCarryFlag(c, res);
	setZeroFlag(c, resInByte);
	setNegativeFlag(c, resInByte);
	if (o->mode == accumulator) {
		c->A = resInByte;
	} else {
		c->writeToMemory(resInByte, o->address);
	}
}

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