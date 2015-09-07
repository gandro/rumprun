/*-
 * Copyright (c) 2015 Sebastian Wicki. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software withough specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/device.h>
#include <sys/systm.h>

#include "htif_var.h"

static int htif_eth_match(device_t, cfdata_t, void *);
static void htif_eth_attach(device_t, device_t, void *);

struct htif_eth_softc {
	device_t sc_dev;
};

CFATTACH_DECL_NEW(htif_eth, sizeof(struct htif_eth_softc),
    htif_eth_match, htif_eth_attach, NULL, NULL);

int
htif_eth_match(device_t parent, cfdata_t cf, void *aux)
{
	struct htif_attach_args * const haa = aux;

	return !strcmp(haa->haa_name, cf->cf_name);
}

static void
htif_eth_attach(device_t parent, device_t self, void *aux)
{
	struct htif_eth_softc * const sc = device_private(self);
	sc->sc_dev = self;

	aprint_normal("\n");
}
