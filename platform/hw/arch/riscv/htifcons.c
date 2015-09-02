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

#include "encoding.h"
 
#define HTIF_DEV_SHIFT		(56)
#define HTIF_CMD_SHIFT		(48)

#define HTIF_CMD_READ		(0x00ULL)
#define HTIF_CMD_WRITE		(0x01ULL)
#define HTIF_CMD_IDENTIFY	(0xFFULL)

#define HTIF_DEV_CONSOLE	(1ULL) /* hard-coded, no device discovery yet */

#define HTIF_CONSOLE_PUTC	((HTIF_DEV_CONSOLE << HTIF_DEV_SHIFT) | \
				 (HTIF_CMD_WRITE << HTIF_CMD_SHIFT))

void
bmk_cons_putc(int c)
{
	uint64_t packet = HTIF_CONSOLE_PUTC | c;

	splhigh();
	while (swap_csr(mtohost, packet) != 0);
	while (swap_csr(mfromhost, 0));
	spl0();
}

void
bmk_cons_puts(const char *s)
{
	int c;

	while ((c = *s++) != 0)
		bmk_cons_putc(c);
}
