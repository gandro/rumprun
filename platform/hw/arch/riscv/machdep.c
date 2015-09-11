/*-
 * Copyright (c) 2015 Sebastian Wicki.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <hw/types.h>
#include <hw/kernel.h>

#include <bmk-core/sched.h>
#include <bmk-core/printf.h>

#include "isr.h"
#include "htif.h"
#include "encoding.h"
   
extern int spldepth;

long riscv_mfromhost = 0;

void
splhigh(void)
{

	/* clear IE in MSTATUS register */
	clear_csr(mstatus, MSTATUS_IE);
	spldepth++;
}

void
spl0(void)
{

	if (spldepth == 0)
		bmk_platform_halt("out of interrupt depth!");
	if (--spldepth == 0)
		set_csr(mstatus, MSTATUS_IE);
}

void
riscv_trap(int code, void *pc, void *badaddr)
{
	bmk_printf("Trap %d at pc %p, addr %p", code, pc, badaddr);
	bmk_platform_halt("\n");
}

void riscv_isr_timer(void)
{

}

void riscv_isr_sw(void)
{
	bmk_platform_halt("software interrupt fired!?");
}

/* XXX: RISC-V Thread Local Storage is not properly documented. According to
 *      https://lists.riscv.org/lists/arc/sw-dev/2015-05/msg00032.html
 *      the TP register points directly the the first static TLS block,
 *      at the end of the TCB, which is neither Variant I nor II.
 *      We assume next->btcb_t is Variant I and work around it.
 */
#define TLSOFFSET (2*sizeof(void *))

void
bmk_platform_cpu_sched_settls(struct bmk_tcb *next)
{
	unsigned long tp = next->btcb_tp + TLSOFFSET;
	__asm__ __volatile__("mv tp, %0" : : "r"(tp));
}
