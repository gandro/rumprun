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
#include <sys/conf.h>   /* cdevsw struct */
#include <sys/uio.h>    /* uio struct */

#include "htif_var.h"
#include "htif_reg.h"

extern struct cfdriver htif_char_cd;

static int htif_char_match(device_t, cfdata_t, void *);
static void htif_char_attach(device_t, device_t, void *);

#define HTIF_CHAR_FLAG_OPEN	1

struct htif_char_softc {
	device_t	sc_dev;

	int		sc_flags;
	unsigned 	sc_htif_dev;
};

struct htif_char_request {
	uint64_t addr;
	uint64_t size;
};

dev_type_open(htif_char_open);
dev_type_close(htif_char_close);
dev_type_read(htif_char_read);
dev_type_write(htif_char_write);
const struct cdevsw htif_char_cdevsw = {
	.d_open = htif_char_open,
	.d_close = htif_char_close,
	.d_read = htif_char_read,
	.d_write = htif_char_write,
	.d_ioctl = noioctl,
	.d_stop = nostop,
	.d_tty = notty,
	.d_poll = nopoll,
	.d_mmap = nommap,
	.d_kqfilter = nokqfilter,
	.d_discard = nodiscard,
	.d_flag = D_OTHER,
};

CFATTACH_DECL_NEW(htif_char, sizeof(struct htif_char_softc),
    htif_char_match, htif_char_attach, NULL, NULL);

int
htif_char_open(dev_t dev, int flags, int mode, struct lwp *l)
{
	struct htif_char_softc *sc;
	sc = device_lookup_private(&htif_char_cd, minor(dev));

	if (sc == NULL)
		return ENXIO;
	if (sc->sc_flags & HTIF_CHAR_FLAG_OPEN)
		return EBUSY;

	sc->sc_flags |= HTIF_CHAR_FLAG_OPEN;

	return 0;
}

int
htif_char_close(dev_t dev, int flags, int fmt, struct lwp *process)
{
	struct htif_char_softc *sc;
	sc = device_lookup_private(&htif_char_cd, minor(dev));

	sc->sc_flags &= ~HTIF_CHAR_FLAG_OPEN;
	return 0;
}


int
htif_char_read(dev_t dev, struct uio *uio, int flag)
{
	return 0;
}

int
htif_char_write(dev_t dev, struct uio *uio, int flag)
{
	struct htif_char_softc *sc;
	sc = device_lookup_private(&htif_char_cd, minor(dev));

	

	return 0;
}

int
htif_char_match(device_t parent, cfdata_t cf, void *aux)
{
	struct htif_attach_args * const haa = aux;

	return !strncmp(haa->haa_name, "char ", 5);
}

static void
htif_char_attach(device_t parent, device_t self, void *aux)
{
	struct htif_char_softc * const sc = device_private(self);
	struct htif_attach_args * const haa = aux;

	sc->sc_dev = self;
	sc->sc_flags = 0;
	sc->sc_htif_dev = haa->haa_dev;

	aprint_normal("\n");
}
