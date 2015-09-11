#include <hw/kernel.h>

#include <bmk-core/errno.h>
#include <bmk-core/printf.h>
#include <bmk-core/string.h>

#include <rump/rump.h>
#include <rump/rumperr.h>

#include "nolibc.h"
#include "rumpsp.h"

#define MAXFDS 1

#define errno (*bmk_sched_geterrno())

struct rumpsp_chan {
	void *token;
	int fd;
	struct pollfd pollfd;
};

static int accepted = 0;
static struct rumpsp_chan chanfd;
static struct pollfd pfd;

static struct rumpsp_handlers handlers;

void
rumpsp_cleanup(void)
{
	rump_sys_close(chanfd.fd);
}

int
rumpsp_init_server(const char *url, struct rumpsp_handlers hndlrs)
{
	chanfd.fd = rump_sys_open(url, RUMP_O_RDWR|RUMP_O_NONBLOCK);
	if (chanfd.fd == -1)
		return errno;

	pfd.fd = chanfd.fd;
	pfd.events = POLLIN|POLLOUT;
	pfd.revents = 0;

	handlers = hndlrs;

	return 0;
}

int
rumpsp_read(struct rumpsp_chan *chan, void *data, size_t size, size_t *nbytes)
{
	ssize_t n;
	
	n = rump_sys_read(chan->fd, data, size);
	if (n > 0) {
		*nbytes = (size_t) n;
		return 0;
	}
	
	*nbytes = 0;
	
	if (n == 0) {
		return RUMP_ENOTCONN;
	}

	if (errno == RUMP_EAGAIN) {
		return RUMPSP_EAGAIN;
	}

	return errno;
}

int
rumpsp_write(struct rumpsp_chan *chan, void *data, size_t size, size_t *nbytes)
{
	ssize_t n;

	n = rump_sys_write(chan->fd, data, size);
	if (n > 0) {
		*nbytes = (size_t) n;
		return 0;
	}

	*nbytes = 0;

	if (n == 0) {
		return RUMP_ENOTCONN;
	}

	if (errno == RUMP_EAGAIN) {
		return RUMPSP_EAGAIN;
	}

	return errno;
}

void
rumpsp_close(struct rumpsp_chan *chan)
{

}

int 
rumpsp_enable_events(struct rumpsp_chan *chan, int events)
{
	if (events & RUMPSP_EVENT_WRITABLE) {
		pfd.events |= POLLOUT;
	}
	
	if (events & RUMPSP_EVENT_READABLE) {
		pfd.events |= POLLIN;
	}

	if (pfd.events)
		pfd.fd = chan->fd;

	return 0;
}

int 
rumpsp_disable_events(struct rumpsp_chan *chan, int events)
{
	if (events & RUMPSP_EVENT_WRITABLE) {
		pfd.events &= ~POLLOUT;
	}
	
	if (events & RUMPSP_EVENT_READABLE) {
		pfd.events &= ~POLLIN;
	}
	
	if (!pfd.events)
		pfd.fd = -1;
	
	return 0;
}

int
rumpsp_dispatch(int timeout_ms)
{
	int rv;

	rv = rump_sys_poll(&pfd, 1, timeout_ms);
	if (rv == -1)
		return errno;

	if (rv == 1) {
		if (!accepted && !(pfd.revents & POLLHUP)) {
			bmk_printf("calling accept\n");
			pfd.events = 0;
			handlers.accepted(&chanfd, &chanfd.token);
			accepted = 1;
		} else if (accepted){
			if (pfd.revents & POLLIN) {
				bmk_printf("calling readable\n");
				handlers.readable(&chanfd, chanfd.token);
			}

			if (pfd.revents & POLLOUT) {
				bmk_printf("calling writeable\n");
				handlers.writable(&chanfd, chanfd.token);
			}
		}


	}
	
	return 0;
}
