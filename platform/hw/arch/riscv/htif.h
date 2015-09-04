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

#ifndef _RISCV_HTIF_H_
#define _RISCV_HTIF_H_

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

void htif_handle_irq(void);

void 		htif_tohost(unsigned, unsigned, unsigned long);
unsigned long 	htif_fromhost(unsigned);
unsigned long 	htif_sync_tofromhost(unsigned, unsigned, unsigned long);

#endif  /* _RISCV_HTIF_H_ */
