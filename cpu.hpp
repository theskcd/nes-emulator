#ifdef CPU_H_INCLUDED
#define CPU_H_INCLUDED

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
	reg_negative,
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

#endif