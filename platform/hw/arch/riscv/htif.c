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

#include <bmk-core/memalloc.h>
#include <bmk-core/printf.h>
#include <bmk-core/queue.h>

#include "encoding.h"
#include "htif.h"
#include "isr.h"

#define HTIF_MAX_DEV		(256)

#define HTIF_DEV_SHIFT		(56)
#define HTIF_DEV_MASK		(0xFF00000000000000UL)

#define HTIF_CMD_SHIFT		(48)
#define HTIF_CMD_MASK		(0x00FF000000000000UL)

#define HTIF_DATA_MASK		(0x0000FFFFFFFFFFFFUL)

#define HTIF_CMD_READ		(0x00U)
#define HTIF_CMD_WRITE		(0x01U)
#define HTIF_CMD_IDENTIFY	(0xFFU)

#define HTIF_DEV_CONSOLE	(1ULL) /* hard-coded, no device discovery yet */

struct htif_irq_handler {
	unsigned int			hih_dev;
	unsigned long			hih_data;

	bmk_htif_handler_t		hih_fun;
	void *				hih_arg;
	SLIST_ENTRY(htif_irq_handler)	hih_entries;
};

SLIST_HEAD(htif_ih_head, htif_irq_handler);

static struct htif_ih_head hihandlers;

static int
isr_bouncer(void *arg)
{
	int rv, didwork = 0;
	struct htif_irq_handler *hih;

	SLIST_FOREACH(hih, &hihandlers, hih_entries) {
		if (hih->hih_data == 0) continue;
	
		rv = hih->hih_fun(hih->hih_arg, hih->hih_data);
		if (rv) {
			hih->hih_data = 0;
			didwork = 1;
		}
	}

	return didwork;
}

/* If we don't handle HTIF interrupts by reseting the MFROMHOST register,
 * it will trigger again before any registered interrupt handlers can run,
 * essentially deadlocking the system.
 */
void
riscv_isr_htif(void)
{
	unsigned dev;
	struct htif_irq_handler *hih;
	unsigned long data = swap_csr(mfromhost, 0);

	dev = ((data & HTIF_DEV_MASK) >> HTIF_DEV_SHIFT);

	SLIST_FOREACH(hih, &hihandlers, hih_entries) {
		if (hih->hih_dev == dev) {
			hih->hih_data = data;
		}
	}
	
	if (!SLIST_EMPTY(&hihandlers)) {
		/* schedule isr_bouncer */
		isr(1 << RISCV_IRQ_HTIF);
	}
}

void 
bmk_htif_tohost(unsigned dev, unsigned cmd, unsigned long data)
{
	unsigned long packet = 0;
	packet |= ((unsigned long)dev << HTIF_DEV_SHIFT) & HTIF_DEV_MASK;
	packet |= ((unsigned long)cmd << HTIF_CMD_SHIFT) & HTIF_CMD_MASK;
	packet |= data & HTIF_DATA_MASK;

	while (swap_csr(mtohost, packet) != 0);
}

#if 0
unsigned long
bmk_htif_fromhost(unsigned dev)
{
	unsigned long response = 0;

	if (dev < HTIF_MAX_DEV) {
		response = mfromhost[dev];
		mfromhost[dev] = 0;
	}

	return response;
}
#endif

unsigned long
bmk_htif_sync_tofromhost(unsigned dev, unsigned cmd, unsigned long data)
{
	unsigned long response;

	splhigh();

	/* make sure mfromhost is clean */
	if (read_csr(mfromhost)) {
		riscv_isr_htif();
	}

	bmk_htif_tohost(dev, cmd, data);

	/* busy wait for response */
	while (!read_csr(mfromhost));
	response = swap_csr(mfromhost, 0);

	spl0();
	
	return response;
}

int
bmk_htif_register_irq_handler(unsigned dev,
				bmk_htif_handler_t handler, void *arg)
{
	int first_handler, err;
	struct htif_irq_handler *hih;

	if (dev >= HTIF_MAX_DEV)
		return BMK_EINVAL;

	hih = bmk_xmalloc_bmk(sizeof(*hih));
	if (!hih)
		return BMK_ENOMEM;

	first_handler = SLIST_EMPTY(&hihandlers);

	hih->hih_dev = dev;
	hih->hih_data = 0;
	hih->hih_fun = handler;
	hih->hih_arg = arg;
	SLIST_INSERT_HEAD(&hihandlers, hih, hih_entries);

	if (first_handler) {
		err = bmk_isr_init(isr_bouncer, NULL, RISCV_IRQ_HTIF);
		if (err) {
			SLIST_REMOVE_HEAD(&hihandlers, hih_entries);
			bmk_memfree(hih, BMK_MEMWHO_WIREDBMK);
			return err;
		}
	}
	
	return 0;
}

void *
bmk_htif_dmalloc(size_t size, size_t align)
{
	return bmk_memalloc(size, align, BMK_MEMWHO_WIREDBMK);
}

void
bmk_htif_dmfree(void *addr)
{
	bmk_memfree(addr, BMK_MEMWHO_WIREDBMK);
}

void
cons_putc(int c)
{
	bmk_htif_sync_tofromhost(HTIF_DEV_CONSOLE, HTIF_CMD_WRITE, c);
}

void
cons_puts(const char *s)
{
	int c;

	while ((c = *s++) != 0)
		cons_putc(c);
}
