#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/conf.h>
#include <sys/device.h>
#include <sys/stat.h>

#include "rump_private.h"
#include "rump_dev_private.h"
#include "rump_vfs_private.h"

RUMP_COMPONENT(RUMP_COMPONENT_DEV)
{
	extern const struct cdevsw htif_char_cdevsw;
	devmajor_t bmaj, cmaj;
	int error;

	bmaj = cmaj = -1;

	if ((error = devsw_attach("htif_char", NULL, &bmaj,
	    &htif_char_cdevsw, &cmaj)) != 0)
		panic("cannot attach htif_char: %d", error);

	if ((error = rump_vfs_makeonedevnode(S_IFCHR, "/dev/htifchar0",
	    cmaj, 0)) != 0)
		panic("cannot create /dev/htifchar0: %d", error);
}
