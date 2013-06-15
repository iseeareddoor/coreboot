/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* RAM driver for the SMSC KBC1100 Super I/O chip */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <device/smbus.h>
#include <string.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"
#include "sch4037.h"

/* Forward declarations */
static void enable_dev(device_t dev);
static void sch4037_pnp_set_resources(device_t dev);
static void sch4037_pnp_enable_resources(device_t dev);
static void sch4037_pnp_enable(device_t dev);
static void sch4037_init(device_t dev);

static void pnp_enter_conf_state(device_t dev);
static void pnp_exit_conf_state(device_t dev);

struct chip_operations superio_smsc_sch4037_ops = {
	CHIP_NAME("SMSC SCH4037 Super I/O")
		.enable_dev = enable_dev,
};

static const struct pnp_mode_ops pnp_conf_mode_ops = {
	.enter_conf_mode  = pnp_enter_conf_state,
	.exit_conf_mode   = pnp_exit_conf_state,
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = sch4037_pnp_set_resources,
	.enable_resources = sch4037_pnp_enable_resources,
	.enable           = sch4037_pnp_enable,
	.init             = sch4037_init,
	.ops_pnp_mode     = &pnp_conf_mode_ops,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, SCH4037_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, { 0x7ff, 0 }, { 0x7ff, 0x4}, },
};

static void enable_dev(device_t dev)
{
	printk(BIOS_SPEW, "file '%s',line %d, %s()\n", __FILE__, __LINE__, __func__);
	pnp_enable_devices(dev, &pnp_ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

static void sch4037_pnp_set_resources(device_t dev)
{
	pnp_set_resources(dev);
}

static void sch4037_pnp_enable_resources(device_t dev)
{
	pnp_enable_resources(dev);
}

static void sch4037_pnp_enable(device_t dev)
{
	pnp_alt_enable(dev);
}

static void sch4037_init(device_t dev)
{
	struct superio_smsc_sch4037_config *conf = dev->chip_info;
	struct resource *res0, *res1;

	if (!dev->enabled) {
		return;
	}

	switch(dev->path.pnp.device) {

		case SCH4037_KBC:
			res0 = find_resource(dev, PNP_IDX_IO0);
			res1 = find_resource(dev, PNP_IDX_IO1);
			pc_keyboard_init(&conf->keyboard);
			break;
	}
}

static void pnp_enter_conf_state(device_t dev)
{
	outb(0x55, dev->path.pnp.port);
}

static void pnp_exit_conf_state(device_t dev)
{
	outb(0xaa, dev->path.pnp.port);
}
