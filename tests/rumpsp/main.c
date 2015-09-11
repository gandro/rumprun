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
	int rv;

	rv = rump_init();
	bmk_printf("rump kernel init complete, rv %d\n", rv);

	rump_init_server("/dev/htifchar0");

	rump_sys_reboot(0, NULL);
}
