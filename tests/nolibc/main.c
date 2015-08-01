#include <bmk/kernel.h>
#include <bmk-core/printf.h>

#include <rump/rump.h>

void
bmk_mainthread(void *cmdline)
{
	int rv, exit;

	rv = rump_init();
	bmk_printf("rump kernel init complete, rv %d\n", rv);

	rump_init_server("htif:///");

	exit = 1;
	while(1) {
		__asm__ __volatile__ ("csrw mtohost, %0" :: "r"(exit));
	}
}
