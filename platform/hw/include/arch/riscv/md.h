#ifndef _BMK_ARCH_RISCV_MD_H_
#define _BMK_ARCH_RISCV_MD_H_

#include <bmk/kernel.h>

#define ENTRY(x)        .text; .globl x; .type x,@function; x:
#define END(x)          .size x, . - x

#define PAGE_SHIFT 12
#define PAGE_SIZE (1<<PAGE_SHIFT)

#define BMK_THREAD_STACK_PAGE_ORDER 1
#define BMK_THREAD_STACKSIZE ((1<<BMK_THREAD_STACK_PAGE_ORDER) * PAGE_SIZE)

#ifndef _LOCORE
#include <bmk-core/platform.h>

extern int bmk_spldepth;

static inline void
splhigh(void)
{

	/* clear IE in MSTATUS register */
	__asm__ __volatile__("csrci 0x300, 0x1");
	bmk_spldepth++;
}

static inline void
spl0(void)
{

	if (bmk_spldepth == 0)
		bmk_platform_halt("out of interrupt depth!");
	if (--bmk_spldepth == 0)
			/* set IE in MSTATUS register */
			__asm__ __volatile__("csrsi 0x300, 0x1");
}

static inline void
hlt(void)
{

	__asm__ __volatile__("wfi");
}

void bmk_riscv_boot(void);
#endif /* !_LOCORE */

#endif /* _BMK_ARCH_RISCV_MD_H_ */
