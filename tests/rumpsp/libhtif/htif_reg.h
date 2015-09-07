/*-
 * Copyright (c) 2015 Sebastian Wicki. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RISCV_HTIF_HTIF_REG_H_
#define _RISCV_HTIF_HTIF_REG_H_

#define HTIF_MEM_ALIGN			64

#define	HTIF_CMD_READ_MEM		0
#define	HTIF_CMD_WRITE_MEM		1
#define	HTIF_CMD_READ_CONTROL_REG	2
#define	HTIF_CMD_WRITE_CONTROL_REG	3
#define	HTIF_CMD_ACK			4
#define	HTIF_CMD_NACK			5

#define	HTIF_REQ_DEV		__BITS(63,56)
#define	HTIF_REQ_CMD		__BITS(55,48)
#define	HTIF_REQ_PAYLOAD	__BITS(47,0)

#endif /* _RISCV_HTIF_HTIF_REG_H_ */
