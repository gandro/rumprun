#include <hw/kernel.h>

#include <bmk-core/errno.h>
#include <bmk-core/printf.h>
#include <bmk-core/string.h>

#include <rump/rump.h>
#include <rump/rumperr.h>

#include "nolibc.h"

#define panic(format, ...) \
	do { \
		bmk_printf( format ,  ## __VA_ARGS__ );		\
		bmk_printf("\nerrno: %s\n", 			\
			rump_strerror(*bmk_sched_geterrno()));	\
		rump_sys_reboot(1, NULL);			\
	} while (0)

void
mainthread(void *cmdline)
{
	int rv, fd;

	rv = rump_init();
	bmk_printf("rump kernel init complete, rv %d\n", rv);

	fd = rump_sys_open("/dev/htifchar0", RUMP_O_RDWR);
	if (fd == -1)
		panic("failed to open htifchar0");
	
	ssize_t n = rump_sys_write(fd, "some data", 10);
	if (n < 0)
		panic("write: %zd", n);

	rump_sys_reboot(0, NULL);
}
