// SPDX-License-Identifier: GPL-2.0-only
/* Copyright Intel Corporation (C) 2023. All rights reserved.
 *
 * Author: Mahesh R Vaidya <mahesh.r.vaidya@intel.com>
 */

#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_net.h>
#include <linux/phy.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/stmmac.h>

#include "stmmac.h"
#include "stmmac_platform.h"
#include "mrphy.h"

#define MRPHY_CONTROL_AN_EN_MASK			BIT(12)
#define MRPHY_CONTROL_REG				0x00
#define MRPHY_CONTROL_RESTART_AN_MASK			BIT(9)
#define MRPHY_CTRL_AUTONEG_SGMII			0x1140
#define MRPHY_IF_MODE_REG				0x28
#define MRPHY_LINK_TIMER_0_REG			0x24
#define MRPHY_LINK_TIMER_1_REG			0x26
#define MRPHY_SIZE					0x40
#define MRPHY_STATUS_AN_COMPLETED_MASK		BIT(5)
#define MRPHY_STATUS_LINK_MASK			0x0004
#define MRPHY_STATUS_REG				0x02
#define MRPHY_SGMII_SPEED_1000			BIT(3)
#define MRPHY_SGMII_SPEED_100				BIT(2)
#define MRPHY_SGMII_SPEED_10				0x0
#define MRPHY_SW_RST_MASK				0x8000
#define MRPHY_PARTNER_ABILITY_REG			0x0A
#define MRPHY_PARTNER_DUPLEX_FULL			0x1000
#define MRPHY_PARTNER_DUPLEX_HALF			0x0000
#define MRPHY_PARTNER_DUPLEX_MASK			0x1000
#define MRPHY_PARTNER_SPEED_MASK			GENMASK(11, 10)
#define MRPHY_PARTNER_SPEED_1000			BIT(11)
#define MRPHY_PARTNER_SPEED_100			BIT(10)
#define MRPHY_PARTNER_SPEED_10			0x0000
#define MRPHY_PARTNER_SPEED_1000			BIT(11)
#define MRPHY_PARTNER_SPEED_100			BIT(10)
#define MRPHY_PARTNER_SPEED_10			0x0000
#define MRPHY_SGMII_SPEED_MASK			GENMASK(3, 2)
#define MRPHY_SGMII_LINK_TIMER_0			0x0D40
#define MRPHY_SGMII_LINK_TIMER_1			0x0003
#define MRPHY_SW_RESET_TIMEOUT			100
#define MRPHY_USE_SGMII_AN_MASK			BIT(1)
#define MRPHY_USE_SGMII_ENA				BIT(0)
#define MRPHY_IF_USE_SGMII				0x03

#define AUTONEGO_LINK_TIMER				20

static int mrphy_reset(void __iomem *base, struct mr_phy *mrphy)
{
	int counter = 0;
	u16 val;

	val = readw(base + MRPHY_CONTROL_REG);
	val |= MRPHY_SW_RST_MASK;
	writew(val, base + MRPHY_CONTROL_REG);

	while (counter < MRPHY_SW_RESET_TIMEOUT) {
		val = readw(base + MRPHY_CONTROL_REG);
		val &= MRPHY_SW_RST_MASK;
		if (val == 0)
			break;
		counter++;
		udelay(1);
	}
	if (counter >= MRPHY_SW_RESET_TIMEOUT) {
		dev_err(mrphy->dev, "MRPHY could not get out of sw reset\n");
		return -ETIMEDOUT;
	}

	return 0;
}

int mrphy_init(void __iomem *base, struct mr_phy *mrphy)
{
	int ret = 0;
	u16 val;
	printk(KERN_INFO "before read MRPHY_GMII8_ADAPTER_REG = %x\n",val);
	val = readw(base + MRPHY_GMII8_ADAPTER_REG);
	printk(KERN_INFO "read MRPHY_GMII8_ADAPTER_REG = %x\n",val);
	writew(0, base + MRPHY_GMII8_ADAPTER_REG);

/*	writew(MRPHY_IF_USE_SGMII, base + MRPHY_IF_MODE_REG);

	writew(MRPHY_CTRL_AUTONEG_SGMII, base + MRPHY_CONTROL_REG);
	writew(MRPHY_SGMII_LINK_TIMER_0, base + MRPHY_LINK_TIMER_0_REG);
	writew(MRPHY_SGMII_LINK_TIMER_1, base + MRPHY_LINK_TIMER_1_REG);
	ret = mrphy_reset(base, mrphy); */

	return ret;
}

/*static void mrphy_link_timer_callback(struct mr_phy *mrphy)
{
	u16 val = 0;
	void __iomem *mrphy_base = mrphy->mrphy_base;

	val = readw(mrphy_base + MRPHY_STATUS_REG);
	val &= MRPHY_STATUS_LINK_MASK;

	if (val != 0) {
		dev_dbg(mrphy->dev, "Adapter: Link is established\n");
	} else {
		mod_timer(&mrphy->aneg_link_timer, jiffies +
			  msecs_to_jiffies(AUTONEGO_LINK_TIMER));
	}
}

static void auto_nego_timer_callback(struct mr_phy *mrphy)
{
	u16 val = 0;
	u16 speed = 0;
	u16 duplex = 0;
	void __iomem *mrphy_base = mrphy->mrphy_base;

	val = readw(mrphy_base + MRPHY_STATUS_REG);
	val &= MRPHY_STATUS_AN_COMPLETED_MASK;

	if (val != 0) {
		dev_dbg(mrphy->dev, "Adapter: Auto Negotiation is completed\n");
		val = readw(mrphy_base + MRPHY_PARTNER_ABILITY_REG);
		speed = val & MRPHY_PARTNER_SPEED_MASK;
		duplex = val & MRPHY_PARTNER_DUPLEX_MASK;

		if (speed == MRPHY_PARTNER_SPEED_10 &&
		    duplex == MRPHY_PARTNER_DUPLEX_FULL)
			dev_dbg(mrphy->dev,
				"Adapter: Link Partner is Up - 10/Full\n");
		else if (speed == MRPHY_PARTNER_SPEED_100 &&
			 duplex == MRPHY_PARTNER_DUPLEX_FULL)
			dev_dbg(mrphy->dev,
				"Adapter: Link Partner is Up - 100/Full\n");
		else if (speed == MRPHY_PARTNER_SPEED_1000 &&
			 duplex == MRPHY_PARTNER_DUPLEX_FULL)
			dev_dbg(mrphy->dev,
				"Adapter: Link Partner is Up - 1000/Full\n");
		else if (speed == MRPHY_PARTNER_SPEED_10 &&
			 duplex == MRPHY_PARTNER_DUPLEX_HALF)
			dev_err(mrphy->dev,
				"Adapter does not support Half Duplex\n");
		else if (speed == MRPHY_PARTNER_SPEED_100 &&
			 duplex == MRPHY_PARTNER_DUPLEX_HALF)
			dev_err(mrphy->dev,
				"Adapter does not support Half Duplex\n");
		else if (speed == MRPHY_PARTNER_SPEED_1000 &&
			 duplex == MRPHY_PARTNER_DUPLEX_HALF)
			dev_err(mrphy->dev,
				"Adapter does not support Half Duplex\n");
		else
			dev_err(mrphy->dev,
				"Adapter: Invalid Partner Speed and Duplex\n");
	} else {
		val = readw(mrphy_base + MRPHY_CONTROL_REG);
		val |= MRPHY_CONTROL_RESTART_AN_MASK;
		writew(val, mrphy_base + MRPHY_CONTROL_REG);

		mrphy_reset(mrphy_base, mrphy);
		mod_timer(&mrphy->aneg_link_timer, jiffies +
			  msecs_to_jiffies(AUTONEGO_LINK_TIMER));
	}
}

static void aneg_link_timer_callback(struct timer_list *t)
{
	struct mr_phy *mrphy = from_timer(mrphy, t, aneg_link_timer);

	if (mrphy->autoneg == AUTONEG_ENABLE)
		auto_nego_timer_callback(mrphy);
	else if (mrphy->autoneg == AUTONEG_DISABLE)
		mrphy_link_timer_callback(mrphy);
}*/

void mrphy_fix_mac_speed(struct mr_phy *mrphy, struct phy_device *phy_dev,
			   unsigned int speed)
{
	void __iomem *mrphy_base = mrphy->mrphy_base;
	/*u32 val;

	mrphy->autoneg = phy_dev->autoneg;

	if (phy_dev->autoneg == AUTONEG_ENABLE) {
		val = readw(mrphy_base + MRPHY_CONTROL_REG);
		val |= MRPHY_CONTROL_AN_EN_MASK;
		writew(val, mrphy_base + MRPHY_CONTROL_REG);

		val = readw(mrphy_base + MRPHY_IF_MODE_REG);
		val |= MRPHY_USE_SGMII_AN_MASK;
		writew(val, mrphy_base + MRPHY_IF_MODE_REG);

		val = readw(mrphy_base + MRPHY_CONTROL_REG);
		val |= MRPHY_CONTROL_RESTART_AN_MASK;

		mrphy_reset(mrphy_base, mrphy);

		timer_setup(&mrphy->aneg_link_timer, aneg_link_timer_callback,
			    0);
		mod_timer(&mrphy->aneg_link_timer, jiffies +
			  msecs_to_jiffies(AUTONEGO_LINK_TIMER));
	} else if (phy_dev->autoneg == AUTONEG_DISABLE) {
		val = readw(mrphy_base + MRPHY_CONTROL_REG);
		val &= ~MRPHY_CONTROL_AN_EN_MASK;
		writew(val, mrphy_base + MRPHY_CONTROL_REG);

		val = readw(mrphy_base + MRPHY_IF_MODE_REG);
		val &= ~MRPHY_USE_SGMII_AN_MASK;
		writew(val, mrphy_base + MRPHY_IF_MODE_REG);

		val = readw(mrphy_base + MRPHY_IF_MODE_REG);
		val &= ~MRPHY_SGMII_SPEED_MASK;

		switch (speed) {
		case 1000:
			val |= MRPHY_SGMII_SPEED_1000;
			break;
		case 100:
			val |= MRPHY_SGMII_SPEED_100;
			break;
		case 10:
			val |= MRPHY_SGMII_SPEED_10;
			break;
		default:
			return;
		}
		writew(val, mrphy_base + MRPHY_IF_MODE_REG);

		mrphy_reset(mrphy_base, mrphy);

		timer_setup(&mrphy->aneg_link_timer, aneg_link_timer_callback,
			    0);
		mod_timer(&mrphy->aneg_link_timer, jiffies +
			  msecs_to_jiffies(AUTONEGO_LINK_TIMER));
	}*/
	mrphy_reset(mrphy_base, mrphy);
	mrphy_init(mrphy_base, mrphy);
}
