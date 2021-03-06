/*-
 * Copyright (c) 2015 Antti Kantee.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <mini-os/types.h>
#include <mini-os/hypervisor.h>
#include <mini-os/kernel.h>
#include <mini-os/xenbus.h>
#include <xen/xen.h>

#include <rumprun-base/config.h>
#include <rumprun-base/rumprun.h>

#include <bmk-core/platform.h>
#include <bmk-core/printf.h>

static char hardcoded_jsoncfg[] = "";

static char *
jsonordie(void)
{

	if (hardcoded_jsoncfg[0] == '\0')
		bmk_platform_halt("could not get configuration");

	bmk_printf("using hardcoded_jsoncfg\n");
	return hardcoded_jsoncfg;
}

static char *
get_config(char *cmdline)
{
	xenbus_transaction_t txn;
	char *cfg;
	int retry;

	if (rumprun_config_isonrootfs_p(cmdline))
		return cmdline;

	if (xenbus_transaction_start(&txn))
		return jsonordie();
	if (xenbus_read(txn, "rumprun/cfg", &cfg) != NULL)
		cfg = jsonordie();
	xenbus_transaction_end(txn, 0, &retry);

	return cfg;
}

int
app_main(start_info_t *si)
{
	void *cookie;

	rumprun_boot(get_config((char *)si->cmd_line));

	RUNMAINS();

	while ((cookie = rumprun_get_finished()) != NULL)
		rumprun_wait(cookie);

	rumprun_reboot();
}
