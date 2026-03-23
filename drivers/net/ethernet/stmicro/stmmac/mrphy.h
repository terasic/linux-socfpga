/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright Intel Corporation (C) 2023. All rights reserved.
 *
 * Author: Mahesh R Vaidya <mahesh.r.vaidya@intel.com>
 */

#ifndef __MRPHY_H__
#define __MRPHY_H__

#include <linux/phy.h>
#include <linux/timer.h>

#define	MRPHY_DET_LAT_CTRL_REG		0x2E
#define	MRPHY_TX_DL_BLOCK_IN_RESET	0x4
#define MRPHY_RX_DL_BLOCK_IN_RESET	0x8

#define MRPHY_GMII8_ADAPTER_REG		0x16
#define	MRPHY_EFIFO_TX_DELAY_REG	0x30
#define	MRPHY_EFIFO_RX_DELAY_REG	0x34

#define	MRPHY_PCS_SOFT_TX_DELAY_REG	0x38
#define	MRPHY_PCS_SOFT_RX_DELAY_REG	0x3C

struct mr_phy {
	struct device *dev;
	void __iomem *mrphy_base;
	struct timer_list aneg_link_timer;
	int autoneg;
};


int mrphy_init(void __iomem *base, struct mr_phy *mrphy);
void mrphy_fix_mac_speed(struct mr_phy *mrphy, struct phy_device *phy_dev,
			   unsigned int speed);

#endif /* __MRPHY_H__ */
