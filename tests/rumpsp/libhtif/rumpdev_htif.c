#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/device.h>

#include "htif_reg.h"
#include "htif_var.h"

#include "htif.h"

int
htif_dev_request(unsigned dev, unsigned cmd, unsigned long data)
{
	if ((dev > 0xFFU) | (cmd > 0xFFU) | (data > 0x0000FFFFFFFFFFFFU))
		return -1;

	bmk_htif_tohost(dev, cmd, data);

	return 0;
}

int
htif_dev_request_sync(unsigned dev, unsigned cmd, unsigned long data, 
			unsigned long *ret)
{
	unsigned long response;

	if ((dev > 0xFFU) | (cmd > 0xFFU) | (data > 0x0000FFFFFFFFFFFFU))
		return -1;

	__sync_synchronize();
	response = bmk_htif_sync_tofromhost(dev, cmd, data);
	__sync_synchronize();

	if (ret)
		*ret = response;

	return 0;
}

int
htif_dev_register_irq(unsigned dev, htif_irq_handler_t handler, void *arg)
{

	return bmk_htif_register_irq_handler(dev, handler, arg);
}

/* This interface will have to change if we ever use it outside rumprun */
int
htif_dmalloc(size_t size, size_t align, vaddr_t *va, paddr_t *pa)
{
	void *addr;

	addr = bmk_htif_dmalloc(size, align);
	if (!addr)
		return -1;

	*va = *pa = (uintptr_t) addr;

	return 0;
}

void
htif_dmfree(vaddr_t va)
{

	bmk_htif_dmfree((void*)(uintptr_t) va);
}
