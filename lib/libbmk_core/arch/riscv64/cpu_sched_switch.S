/*-
 ****************************************************************************
 * (C) 2005 - Grzegorz Milos - Intel Research Cambridge
 ****************************************************************************
 *
 *        File: sched.c
 *      Author: Grzegorz Milos
 *     Changes: Robert Kaiser
 *              
 *        Date: Aug 2005
 * 
 * Environment: Xen Minimal OS
 * Description: simple scheduler for Mini-Os
 *
 * The scheduler is non-preemptive (cooperative), and schedules according 
 * to Round Robin algorithm.
 *
 ****************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */

#include <bmk-core/riscv/asm.h>

ENTRY(bmk_cpu_sched_bouncer)
	LOAD  a0, (sp)
	LOAD  t0, REGBYTES(sp)
	addi sp, sp, 2*REGBYTES
	li s0, 0	/* fp */
	jalr t0
	jal bmk_sched_exit
END(bmk_cpu_sched_bouncer)

ENTRY(bmk_cpu_sched_switch)
	addi sp, sp, -16*REGBYTES	/* reserve space + 16 byte alignment */
	STORE  s0,  1*REGBYTES(sp)
	STORE  s1,  2*REGBYTES(sp)
	STORE  s2,  3*REGBYTES(sp)
	STORE  s3,  4*REGBYTES(sp)
	STORE  s4,  5*REGBYTES(sp)
	STORE  s5,  6*REGBYTES(sp)
	STORE  s6,  7*REGBYTES(sp)
	STORE  s7,  8*REGBYTES(sp)
	STORE  s8,  9*REGBYTES(sp)
	STORE  s9, 10*REGBYTES(sp)
	STORE s10, 11*REGBYTES(sp)
	STORE s11, 12*REGBYTES(sp)
	STORE ra,  13*REGBYTES(sp)

#ifdef __riscv_hard_float
	// TODO
#endif
	la ra, 1f
	STORE sp, 0*REGBYTES(a0)	/* prev->sp */
	STORE ra, 1*REGBYTES(a0)	/* prev->ip */
	//STORE tp, 2*REGBYTES(a0)	/* prev->tp */	// TODO is this needed?
	
	LOAD sp, 0*REGBYTES(a1)	/* next->sp */
	LOAD ra, 1*REGBYTES(a1)	/* next->ip */
	//LOAD tp, 2*REGBYTES(a1)	/* next->tp */ // TODO is this needed?
	ret
1:
	LOAD  s0,  1*REGBYTES(sp)
	LOAD  s1,  2*REGBYTES(sp)
	LOAD  s2,  3*REGBYTES(sp)
	LOAD  s3,  4*REGBYTES(sp)
	LOAD  s4,  5*REGBYTES(sp)
	LOAD  s5,  6*REGBYTES(sp)
	LOAD  s6,  7*REGBYTES(sp)
	LOAD  s7,  8*REGBYTES(sp)
	LOAD  s8,  9*REGBYTES(sp)
	LOAD  s9, 10*REGBYTES(sp)
	LOAD s10, 11*REGBYTES(sp)
	LOAD s11, 12*REGBYTES(sp)
	LOAD ra,  13*REGBYTES(sp)
	addi sp, sp, 16*REGBYTES
	ret
END(bmk_cpu_sched_switch)
