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
#include <sys/condvar.h>
#include <sys/conf.h>   /* cdevsw struct */
#include <sys/uio.h>    /* uio struct */
#include <sys/poll.h>

#include "htif_var.h"
#include "htif_reg.h"

extern struct cfdriver htif_char_cd;

static int htif_char_match(device_t, cfdata_t, void *);
static void htif_char_attach(device_t, device_t, void *);

#define HTIF_CHAR_FLAG_OPEN	1

#define HTIF_CHAR_POLLIN	0x1
#define HTIF_CHAR_POLLOUT	0x2
#define HTIF_CHAR_POLLHUP	0x4

struct htif_char_softc {
	device_t	sc_dev;

	int		sc_flags;
	unsigned 	sc_htif_dev;
	kmutex_t	sc_mtx;
	kcondvar_t	sc_cv;
};

dev_type_open(htif_char_open);
dev_type_close(htif_char_close);
dev_type_read(htif_char_read);
dev_type_write(htif_char_write);
dev_type_poll(htif_char_poll);
const struct cdevsw htif_char_cdevsw = {
	.d_open = htif_char_open,
	.d_close = htif_char_close,
	.d_read = htif_char_read,
	.d_write = htif_char_write,
	.d_ioctl = noioctl,
	.d_stop = nostop,
	.d_tty = notty,
	.d_poll = htif_char_poll,
	.d_mmap = nommap,
	.d_kqfilter = nokqfilter,
	.d_discard = nodiscard,
	.d_flag = D_OTHER,
};

CFATTACH_DECL_NEW(htif_char, sizeof(struct htif_char_softc),
    htif_char_match, htif_char_attach, NULL, NULL);

struct htif_char_request {
	uint64_t addr;
	uint64_t size;
	uint64_t tag;
} __packed;

static int
htif_char_isr(void *self, unsigned long data)
{
	struct htif_char_softc * const sc = device_private(self);
	device_printf(sc->sc_dev, "got irq, data: %lx\n", data);
	/* TODO other lwp could have invoked _cmd in the meantime */
	mutex_enter(&sc->sc_mtx);
	cv_signal(&sc->sc_cv);
	mutex_exit(&sc->sc_mtx);
	return 1;
}

/* TODO: should use physio for this */
static size_t
htif_char_cmd(struct htif_char_softc *sc, unsigned cmd, 
				struct uio *uio)
{
	struct htif_char_request *req;
	paddr_t req_pa, data_pa;
	size_t data_len;
	void *data;
	int error = 0;

	data_len = uio->uio_resid;

	error = htif_dmalloc(data_len, HTIF_MEM_ALIGN, 
				(vaddr_t*)&data, &data_pa);
	if (error)
		return error;

	error = htif_dmalloc(sizeof(*req), HTIF_MEM_ALIGN, 
				(vaddr_t*)&req, &req_pa);
	if (error) {
		htif_dmfree((vaddr_t)data);
		return error;
	}

	req->size = data_len;
	req->addr = data_pa;
	req->tag = data_pa;

	mutex_enter(&sc->sc_mtx);
	if (cmd == HTIF_CMD_READ_MEM) {
		htif_dev_request(sc->sc_htif_dev, cmd, req_pa);
		cv_wait(&sc->sc_cv, &sc->sc_mtx);
		uiomove(data, req->size, uio); // TODO this is not safe
	} else if (cmd == HTIF_CMD_WRITE_MEM) {
		uiomove(data, data_len, uio);
		htif_dev_request(sc->sc_htif_dev, cmd, req_pa);
		cv_wait(&sc->sc_cv, &sc->sc_mtx);  // TODO this is not safe
		uio->uio_resid = req->size;
	} else {
		error = EINVAL;
	}

	mutex_exit(&sc->sc_mtx);

	htif_dmfree((vaddr_t)req);
	htif_dmfree((vaddr_t)data);

	return error;
}

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
	int error = 0;
	struct htif_char_softc *sc;
	sc = device_lookup_private(&htif_char_cd, minor(dev));

	unsigned long fromhost = 0;
	htif_dev_request_sync(sc->sc_htif_dev,
		HTIF_CMD_READ_CONTROL_REG, HTIF_CHAR_POLLIN, &fromhost);

	if (!(fromhost & HTIF_CHAR_POLLIN))
		return EAGAIN;

	error = htif_char_cmd(sc, HTIF_CMD_READ_MEM, uio);

	return error;
}

int
htif_char_write(dev_t dev, struct uio *uio, int flag)
{
	int error = 0;
	struct htif_char_softc *sc;
	sc = device_lookup_private(&htif_char_cd, minor(dev));

	unsigned long fromhost = 0;
	htif_dev_request_sync(sc->sc_htif_dev,
		HTIF_CMD_READ_CONTROL_REG, HTIF_CHAR_POLLOUT, &fromhost);

	if (!(fromhost & HTIF_CHAR_POLLOUT))
		return EAGAIN;

	device_printf(sc->sc_dev, "flag: %d\n", flag);
	error = htif_char_cmd(sc, HTIF_CMD_WRITE_MEM, uio);

	return error;
}

int
htif_char_poll(dev_t dev, int events, struct lwp *l)
{
	struct htif_char_softc *sc;
	unsigned long fromhost = 0;
	unsigned long tohost = 0;
	int revents = 0;

	sc = device_lookup_private(&htif_char_cd, minor(dev));

	if (events & POLLIN)
		tohost |= HTIF_CHAR_POLLIN;
	if (events & POLLOUT)
		tohost |= HTIF_CHAR_POLLOUT;

	htif_dev_request_sync(sc->sc_htif_dev,
		HTIF_CMD_READ_CONTROL_REG, tohost, &fromhost);

	if (fromhost & HTIF_CHAR_POLLIN)
		revents |= POLLIN;
	if (fromhost & HTIF_CHAR_POLLOUT)
		revents |= POLLOUT;
	if (fromhost & HTIF_CHAR_POLLHUP)
		revents |= POLLHUP;

	return revents;
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
	int error;
	struct htif_char_softc * const sc = device_private(self);
	struct htif_attach_args * const haa = aux;

	aprint_normal("\n");

	sc->sc_dev = self;
	sc->sc_flags = 0;
	sc->sc_htif_dev = haa->haa_dev;
	mutex_init(&sc->sc_mtx, MUTEX_DEFAULT, IPL_HIGH);
	cv_init(&sc->sc_cv, "htifchr");

	error = htif_dev_register_irq(haa->haa_dev, htif_char_isr, self);
	if (error) {
		aprint_error_dev(self, "couldn't register interrupt\n");
	}
}
