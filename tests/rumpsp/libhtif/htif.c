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
#include "htif_reg.h"

static int htif_match(device_t, cfdata_t, void *);
static void htif_attach(device_t, device_t, void *);

struct htif_softc {
	device_t sc_dev;
};

CFATTACH_DECL_NEW(htif, sizeof(struct htif_softc),
    htif_match, htif_attach, NULL, NULL);

static int
htif_print(void *aux, const char *name)
{
	struct htif_attach_args *haa = aux;
	
	if (!name)
		return QUIET;

	aprint_normal("htif dev %d: \"%s\"", haa->haa_dev, haa->haa_name);
	return UNSUPP;
}

int
htif_match(device_t parent, cfdata_t cf, void *aux)
{
	return 1;
}

static void
htif_attach(device_t parent, device_t self, void *aux)
{
	struct htif_attach_args haa;
	struct htif_softc * const sc = device_private(self);
	char *name;
	paddr_t pbuf;
	int error;
	
	sc->sc_dev = self;

	aprint_normal("\n");

	error = htif_dmalloc(128, HTIF_MEM_ALIGN, (vaddr_t*) &name, &pbuf);
	if (error) {
		aprint_error_dev(self, "failed to allocate buffer\n");
		return;
	}

	for (unsigned dev = 0; dev < HTIF_MAX_DEV; dev++) {
		htif_dev_request_sync(dev, 0xFF, pbuf << 8 | 0xFF, NULL);

		if (name[0]) {
			haa.haa_name = name;
			haa.haa_dev = dev;
			config_found_ia(self, "htif", &haa, htif_print);
		}
	}

	htif_dmfree((vaddr_t) name);

	device_printf(self, "finished probing\n");
}
