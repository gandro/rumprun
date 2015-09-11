#include <bmk-core/riscv/asm.h>

#include "encoding.h"

#define	ENTRY_EXC(name, bytealign)				\
	.section ".text.entry","ax";				\
	.balign bytealign;					\
	.globl	name;						\
	.type	name, @function;				\
	name:

#define TRAP(name)					 \
ENTRY_EXC(riscv_trap_##name, 0x40)			 \
	j riscv_exception				;\
END(riscv_trap_##name)				 \

/* 0x100 Trap from user-mode */
TRAP(user)
/* 0x140 Trap from supervisor-mode */
TRAP(supervisor)
/* 0x180 Trap from hypervisor-mode */
TRAP(hypervisor)
/* 0x1C0 Trap from machine-mode */
TRAP(machine)

/* 0x1FC Non-maskable interrupt(s) */
ENTRY_EXC(riscv_irq_nmi, 0x20)
.space 28			/* 0x1E0 */
	j riscv_exception	/* 0x1FC */
END(riscv_irq_nmi)

/* 0x200 Reset vector */
ENTRY_EXC(_start, 0x40)
	la gp, _gp
	la sp, bootstack
	/* disable interrupts, will be enabled in riscv_boot */
	csrci mstatus, MSTATUS_IE
	/* ready to go */
	li a0, 0
	jal riscv_boot
	/* exit */
1:
	j 1b
END(_start)

.data
.space 4096
bootstack:

ENTRY(riscv_exception)
	addi sp, sp, -32*REGBYTES
	STORE  x1,  1*REGBYTES(sp)
	STORE  x2,  2*REGBYTES(sp)
	STORE  x3,  3*REGBYTES(sp)
	STORE  x4,  4*REGBYTES(sp)
	STORE  x5,  5*REGBYTES(sp)
	STORE  x6,  6*REGBYTES(sp)
	STORE  x7,  7*REGBYTES(sp)
	STORE  x8,  8*REGBYTES(sp)
	STORE  x9,  9*REGBYTES(sp)
	STORE x10, 10*REGBYTES(sp)
	STORE x11, 11*REGBYTES(sp)
	STORE x12, 12*REGBYTES(sp)
	STORE x13, 13*REGBYTES(sp)
	STORE x14, 14*REGBYTES(sp)
	STORE x15, 15*REGBYTES(sp)
	STORE x16, 16*REGBYTES(sp)
	STORE x17, 17*REGBYTES(sp)
	STORE x18, 18*REGBYTES(sp)
	STORE x19, 19*REGBYTES(sp)
	STORE x20, 20*REGBYTES(sp)
	STORE x21, 21*REGBYTES(sp)
	STORE x22, 22*REGBYTES(sp)
	STORE x23, 23*REGBYTES(sp)
	STORE x24, 24*REGBYTES(sp)
	STORE x25, 25*REGBYTES(sp)
	STORE x26, 26*REGBYTES(sp)
	STORE x27, 27*REGBYTES(sp)
	STORE x28, 28*REGBYTES(sp)
	STORE x29, 29*REGBYTES(sp)
	STORE x30, 30*REGBYTES(sp)
	STORE x31, 31*REGBYTES(sp)

	/* set return address to restore, avoids additional jump */
	la ra, 2f

	csrr a0, mcause
	bltz a0, 1f
	/* exception was a trap */
	csrr a1, mepc
	csrr a2, mbadaddr
	tail riscv_trap
1:
	andi a0, a0, 0xF
	beqz a0, riscv_isr_sw
	li t0, 1
	beq t0, a0, riscv_isr_timer
	li t0, 2
	beq t0, a0, riscv_isr_htif
	li t0, 1
	sllw a0, t0, a0
	tail isr
2:
	LOAD  x1,  1*REGBYTES(sp)
	LOAD  x2,  2*REGBYTES(sp)
	LOAD  x3,  3*REGBYTES(sp)
	LOAD  x4,  4*REGBYTES(sp)
	LOAD  x5,  5*REGBYTES(sp)
	LOAD  x6,  6*REGBYTES(sp)
	LOAD  x7,  7*REGBYTES(sp)
	LOAD  x8,  8*REGBYTES(sp)
	LOAD  x9,  9*REGBYTES(sp)
	LOAD x10, 10*REGBYTES(sp)
	LOAD x11, 11*REGBYTES(sp)
	LOAD x12, 12*REGBYTES(sp)
	LOAD x13, 13*REGBYTES(sp)
	LOAD x14, 14*REGBYTES(sp)
	LOAD x15, 15*REGBYTES(sp)
	LOAD x16, 16*REGBYTES(sp)
	LOAD x17, 17*REGBYTES(sp)
	LOAD x18, 18*REGBYTES(sp)
	LOAD x19, 19*REGBYTES(sp)
	LOAD x20, 20*REGBYTES(sp)
	LOAD x21, 21*REGBYTES(sp)
	LOAD x22, 22*REGBYTES(sp)
	LOAD x23, 23*REGBYTES(sp)
	LOAD x24, 24*REGBYTES(sp)
	LOAD x25, 25*REGBYTES(sp)
	LOAD x26, 26*REGBYTES(sp)
	LOAD x27, 27*REGBYTES(sp)
	LOAD x28, 28*REGBYTES(sp)
	LOAD x29, 29*REGBYTES(sp)
	LOAD x30, 30*REGBYTES(sp)
	LOAD x31, 31*REGBYTES(sp)

	addi sp, sp, 32*REGBYTES
	eret
END(riscv_exception)
