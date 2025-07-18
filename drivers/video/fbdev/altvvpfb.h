/*
 * Copyright (C) 2017 Intel Corporation.
 *
 * Intel Video and Image Processing(VIP) Frame Buffer II driver.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
  /*
 	2025-06-20 will: modified for altera vvp framebuffer
  */

#ifndef _ALTVVPFB_H
#define _ALTVVPFB_H
#include <linux/io.h>
#include <linux/fb.h>

#define DRIVER_NAME	"altvvpfb"
#define PALETTE_SIZE	256
#define BYTES_PER_PIXEL		4

/* vvp registers */

// Parameterization registers
#define VVP_VFR_VID_PID					0x0000 // 0x6AF7_024A
#define VVP_VFR_VERSION					0x0004
#define VVP_VFR_LITE_MODE				0x0008
#define VVP_VFR_DEBUG_ENABLED			0x000C
#define VVP_VFR_MAX_BUFFER_SETS			0x0010
#define VVP_VFR_MAX_HEIGHT				0x0014
#define VVP_VFR_MAX_WIDTH				0x0018
#define VVP_VFR_BITS_PER_SYMBOL			0x001C
#define VVP_VFR_NUMBER_OF_COLOR_PLANES	0x0020
#define VVP_VFR_PIXELS_IN_PARALLEL		0x0024
#define VVP_VFR_PACKING					0x0028

// Interrupt registers
#define VVP_VFR_IRQ_CONTROL				0x0100
#define VVP_VFR_IRQ_STATUS				0x0104

// Control and Debug Registers
#define VVP_VFR_CSR_STATUS				0x0140
#define VVP_VFR_CSR_LAST_BUFFER_READ	0x0144
#define VVP_VFR_CSR_COMMIT				0x0190
#define VVP_VFR_CSR_NUM_BUFFER_SETS 	0x0194
#define VVP_VFR_CSR_BUFFER_MODE			0x0198
#define VVP_VFR_CSR_STARTING_BUFFER_SET	0x019C
#define VVP_VFR_CSR_RUN					0x01A0
#define VVP_VFR_CSR_FSYNC_PULSE_MODE	0x01A4

#define VVP_VFR_BUFFER_0_BASE					0x01B0
#define VVP_VFR_BUFFER_0_NUM_BUFFERS			0x01B4
#define VVP_VFR_BUFFER_0_INTER_BUFFER_OFFSET	0x01B8
#define VVP_VFR_BUFFER_0_INTER_LINE_OFFSET		0x01BC
#define VVP_VFR_BUFFER_0_WIDTH					0x01C0
#define VVP_VFR_BUFFER_0_HEIGHT					0x01C4
#define VVP_VFR_BUFFER_0_INTERLACE				0X01C8
#define VVP_VFR_BUFFER_0_COLORSPACE				0x01CC
#define VVP_VFR_BUFFER_0_SUBSAMPLING			0x01D0
#define VVP_VFR_BUFFER_0_COSITING				0x01D4
#define VVP_VFR_BUFFER_0_BPS					0x01D8
#define VVP_VFR_BUFFER_0_FIELD_COUNT			0x01DC


int altvvpfb_probe(struct device *dev, void __iomem *base);
void altvvpfb_remove(struct device *dev);

struct altvvpfb_priv {
	struct	fb_info info;
	void	__iomem *base;
	int		irq_base;
	u32	pseudo_palette[PALETTE_SIZE];
};

#endif /* _ALTVVPFB_H */
