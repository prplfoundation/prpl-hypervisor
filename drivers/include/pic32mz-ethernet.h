/*
 * Copyright (c) 2016, prpl Foundation
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear 
 * in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.
 * 
 * Network driver for the PIC32MZ internal Ethernet controller. The following
 * PHY variants are supported:
 *      SMSC LAN8720A
 *      SMSC LAN8740A
 *
 * Based on sources of Digilent deIPcK library by Keith Vogel, LiteBSD by
 * Serge Vakulenko and the PIC32 Ethernet interface reference manual.
 */

#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#include <types.h>
#include <globals.h>
#include <pic32mz.h>

#define delay_ms(x) udelay(x*1000)
#define min(a,b)        ((a)<(b)?(a):(b))

#define MTU         1536
#define RX_PACKETS      4
#define RX_BYTES_PER_DESC   128
#define RX_BYTES        (RX_PACKETS * MTU)
#define RX_DESCRIPTORS      (RX_BYTES / RX_BYTES_PER_DESC)
#define TX_DESCRIPTORS      6

/*
 * DMA buffer descriptor (simple list format).
 */
typedef struct {
    uint32_t hdr;       /* Flags */
    uint32_t paddr;     /* Phys address of data buffer */
    uint32_t ctl;       /* TX options / RX filter status */
    uint32_t status;    /* Status */
} eth_desc_t;

/* Start of packet */
#define DESC_SOP(d)     ((d)->hdr & 0x80000000)
#define DESC_SET_SOP(d)     ((d)->hdr |= 0x80000000)

/* End of packet */
#define DESC_EOP(d)     ((d)->hdr & 0x40000000)
#define DESC_SET_EOP(d)     ((d)->hdr |= 0x40000000)

/* Number of data bytes */
#define DESC_BYTECNT(d)     ((d)->hdr >> 16 & 0x7ff)
#define DESC_SET_BYTECNT(d,n)   ((d)->hdr = ((d)->hdr & ~0x7ff0000) | (n) << 16)

/* Next descriptor pointer valid */
#define DESC_SET_NPV(d)     ((d)->hdr |= 0x00000100)
#define DESC_CLEAR_NPV(d)   ((d)->hdr &= ~0x00000100)

/* Eth controller owns this desc */
#define DESC_EOWN(d)        ((d)->hdr & 0x00000080)
#define DESC_SET_EOWN(d)    ((d)->hdr |= 0x00000080)
#define DESC_CLEAR_EOWN(d)  ((d)->hdr &= ~0x00000080)

/* Size of received packet */
#define DESC_FRAMESZ(d)     ((d)->status & 0xffff)

/* Receive filter status */
#define DESC_RXF(d)     ((d)->ctl >> 24)
#define DESC_SET_RXF(d,n)   ((d)->ctl = ((d)->ctl & 0xffffff) | (n) << 24)

#define MACH_VIRT_TO_PHYS(x)    ((uint32_t)(x) & 0x1fffffff)
#define MACH_PHYS_TO_VIRT(x)    ((void *) ((x) | 0xa0000000))

#define INCR_RX_INDEX(_i)   ((_i + 1) % RX_DESCRIPTORS)

#define PHY_ID_LAN8720A     0x0007c0f0  /* SMSC LAN8720A */
#define PHY_ID_LAN8740A     0x0007c110  /* SMSC LAN8740A */

/*-------------------------------------------------------------
 * PHY registers.
 */
#define PHY_CONTROL         0       /* Basic Control Register */
#define PHY_STATUS          1       /* Basic Status Register */
#define PHY_ID1             2       /* PHY identifier 1 */
#define PHY_ID2             3       /* PHY identifier 2 */
#define PHY_ADVRT           4       /* Auto-negotiation advertisement */

#define PHY_CONTROL_DPLX        0x0100  /* Full duplex */
#define PHY_CONTROL_ANEG_RESTART    0x0200  /* Write 1 to restart autoneg */
#define PHY_CONTROL_ISOLATE     0x0400  /* MII interface disable */
#define PHY_CONTROL_PDN         0x0800  /* Powerdown enable */
#define PHY_CONTROL_ANEG_EN     0x1000  /* Auto-negotiation enable */
#define PHY_CONTROL_SPEED_100       0x2000  /* Select 100 Mbps speed */
#define PHY_CONTROL_LPBK        0x4000  /* Loopback enable */
#define PHY_CONTROL_RESET       0x8000  /* Reset, bit self cleared */
#define PHY_CONTROL_BITS "\20"\
"\11dplx\12aneg-rst\13isolate\14pdn\15aneg-en\16speed100\17lpbk\20rst"

#define PHY_STATUS_EXCAP        0x0001  /* Extended capabilities regs present */
#define PHY_STATUS_JAB          0x0002  /* Jabber detected */
#define PHY_STATUS_LINK         0x0004  /* Link valid */
#define PHY_STATUS_CAP_ANEG     0x0008  /* Auto-negotiation available */
#define PHY_STATUS_REM_FLT      0x0010  /* Remote fault detected */
#define PHY_STATUS_ANEG_ACK     0x0020  /* Auto-negotiation acknowledge */
#define PHY_STATUS_EXSTATUS     0x0100  /* Extended status reg present */
#define PHY_STATUS_CAP_100T2_HDX    0x0200  /* Can do 100Base-T2 half duplex */
#define PHY_STATUS_CAP_100T2_FDX    0x0400  /* Can do 100Base-T2 full duplex */
#define PHY_STATUS_CAP_10_HDX       0x0800  /* Can do 10Base-TX half duplex */
#define PHY_STATUS_CAP_10_FDX       0x1000  /* Can do 10Base-TX full duplex */
#define PHY_STATUS_CAP_100_HDX      0x2000  /* Can do 100Base-TX half duplex */
#define PHY_STATUS_CAP_100_FDX      0x4000  /* Can do 100Base-TX full duplex */
#define PHY_STATUS_CAP_100_T4       0x8000  /* Can do 100Base-T4 */
#define PHY_STATUS_BITS "\20"\
"\1exreg\2jab\3link\4cap-aneg\5rem-flt\6aneg-ack"\
"\14hdx10\15fdx10\16hdx100\17fdx100\20t4-100"

#define PHY_ADVRT_CSMA          0x0001  /* Capable of 802.3 CSMA operation */
#define PHY_ADVRT_10_HDX        0x0020  /* Can do 10Base-TX half duplex */
#define PHY_ADVRT_10_FDX        0x0040  /* Can do 10Base-TX full duplex */
#define PHY_ADVRT_100_HDX       0x0080  /* Can do 100Base-TX half duplex */
#define PHY_ADVRT_100_FDX       0x0100  /* Can do 100Base-TX full duplex */
#define PHY_ADVRT_RF            0x2000  /* Remote fault */
#define PHY_ADVRT_BITS "\20"\
"\1csma\6hdx10\7fdx10\10hdx100\11fdx100\16rf"

/*
 * Register #31 for SMSC LAN8720A, LAN8740A.
 */
#define PHY_LAN87x0A_AUTODONE       0x1000  /* Auto-negotiation is done */
#define PHY_LAN87x0A_4B5B       0x0040  /* Enable 4B5B encoding */
#define PHY_LAN87x0A_FDX        0x0010  /* Full duplex */
#define PHY_LAN87x0A_100        0x0008  /* Speed 100 Mbps */
#define PHY_LAN87x0A_10         0x0004  /* Speed 10 Mbps */
#define PHY_LAN87x0A_BITS "\20"\
"\3speed10\4speed100\5fdx\15autodone"

/*
 * Ethernet software interface.
 */
struct eth_port {
    uint8_t     macaddr[6];
    int32_t     is_up;          /* whether the link is up */
    int32_t     phy_addr;       /* 5-bit PHY address on RMII bus */
    int32_t     phy_id;         /* PHY vendor and chip model */
    int8_t      *rx_buf;        /* pointer to the RX buffer */
    eth_desc_t  *rx_desc;       /* pointer to the RX DMA descriptors */
    eth_desc_t  *tx_desc;       /* pointer to the TX DMA descriptors */
} eth_port = {0};

void en_watchdog(void);
void en_ll_output(uint8_t *frame, uint16_t size);
int32_t en_ll_input(uint8_t *frame);
static void en_init();


/* Send buffer used in the hypercalls */
uint8_t tx_buf[MTU];


#endif 