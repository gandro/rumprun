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

#include <bmk/types.h>
#include <bmk/kernel.h>

#include <bmk-core/core.h>
#include <bmk-core/sched.h>
#include <bmk-core/printf.h>
#include <bmk-core/pgalloc.h>

/* Supposedly, the amount of physical memory can be read in address 0x0, but
 * this is not the case for spike. Thus, we use a fixed amount of memory
 */
static void
bmk_riscv_meminit(void)
{
	extern char _end[];
	unsigned long mem_start, mem_end;

	mem_start = bmk_round_page((unsigned long)_end);
	mem_end = (32 << 20);

	bmk_pgalloc_loadmem(mem_start, mem_end);
	bmk_memsize = mem_end - mem_start;
}

void
bmk_riscv_boot(void)
{
	bmk_printf_init(bmk_cons_putc, NULL);
	bmk_core_init(BMK_THREAD_STACK_PAGE_ORDER, PAGE_SHIFT);

	bmk_printf("rump kernel bare metal riscv bootstrap\n\n");

	bmk_cpu_init();
	bmk_sched_init();

	bmk_riscv_meminit();
	spl0();

	bmk_run("");
}
