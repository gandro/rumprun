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

#include <bmk-core/printf.h>

#include "encoding.h"
#include "htif.h"

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

static unsigned long mfromhost[HTIF_MAX_DEV];

void
htif_handle_irq(void)
{
	unsigned dev;
	unsigned long packet = swap_csr(mfromhost, 0);

	dev = ((packet & HTIF_DEV_MASK) >> HTIF_DEV_SHIFT);

	if (mfromhost[dev]) {
		bmk_printf("HTIF: Dropping packet for device %d\n", dev);
	}

	mfromhost[dev] = packet;
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

unsigned long
bmk_htif_sync_tofromhost(unsigned dev, unsigned cmd, unsigned long data)
{
	unsigned long response;

	splhigh();

	/* make sure mfromhost is clean */
	if (read_csr(mfromhost)) {
		htif_handle_irq();
	}

	bmk_htif_tohost(dev, cmd, data);

	/* busy wait for response */
	while (!read_csr(mfromhost));
	response = swap_csr(mfromhost, 0);

	spl0();
	
	return response;
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
