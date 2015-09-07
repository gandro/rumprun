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
htif_dev_response(unsigned dev, unsigned long *ret)
{
	unsigned long response;

	if (dev > 0xFFU)
		return -1;

	response = bmk_htif_fromhost(dev);
	if (ret)
		*ret = response;
	
	return 0;
}

int
htif_dev_request_sync(unsigned dev, unsigned cmd, unsigned long data, 
			unsigned long * ret)
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
htif_dev_register_irq(unsigned dev, htif_irq_handler_t handler)
{
	return 0;
}
