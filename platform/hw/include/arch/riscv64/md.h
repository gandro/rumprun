#ifndef _BMK_ARCH_RISCV_MD_H_
#define _BMK_ARCH_RISCV_MD_H_

#include <hw/kernel.h>

#define ENTRY(x)        .text; .globl x; .type x,@function; x:
#define END(x)          .size x, . - x

#define PAGE_SHIFT 12
#define PAGE_SIZE (1<<PAGE_SHIFT)

#define BMK_THREAD_STACK_PAGE_ORDER 1
#define BMK_THREAD_STACKSIZE ((1<<BMK_THREAD_STACK_PAGE_ORDER) * PAGE_SIZE)

#ifndef _LOCORE
#include <bmk-core/platform.h>

void splhigh(void);
void spl0(void);

static inline void
hlt(void)
{

	__asm__ __volatile__("wfi");
}

void riscv_boot(void);
#endif /* !_LOCORE */

#endif /* _BMK_ARCH_RISCV_MD_H_ */
