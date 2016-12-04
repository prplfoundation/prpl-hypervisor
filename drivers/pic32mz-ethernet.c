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
 */

/*
 * Network driver for the PIC32MZ internal Ethernet controller. The following
 * PHY variants are supported:
 *      SMSC LAN8720A
 *      SMSC LAN8740A
 *
 * Based on sources of Digilent deIPcK library by Keith Vogel, LiteBSD by
 * Serge Vakulenko and the PIC32 Ethernet interface reference manual.
 */

#include <pic32mz.h>
#include <pic32mz-ethernet.h>
#include <mips_cp0.h>
#include <driver.h>
#include <hypercall_defines.h>
#include <guest_interrupts.h>
#include <tlb.h>
#include <libc.h>
#include <hal.h>
#include <scheduler.h>
#include <malloc.h>
#include <board.h>
#include <hypercall.h>
#include <interrupts.h>
#include <vcpu.h>

static uint32_t receive_index = 0;
static uint32_t read_index = 0;
static uint32_t desc_offset = 0;
static uint32_t frame_size = 0;
static uint32_t read_nbytes = 0;

static vcpu_t* vcpu = NULL;

void ethernet_interrupt_handler(){

	fast_interrupt_delivery(vcpu);

	IFSCLR(IRQ_ETH >> 5) = (1<<25);
	
	ETHIRQCLR = ETHIRQ;
	
}

void en_get_mac(){
	uint8_t * mac = (uint8_t *) tlbCreateEntry((uint32_t) MoveFromPreviousGuestGPR(REG_A0), vm_in_execution->base_addr, sizeof(uint8_t) * 6, 0xf, CACHEABLE);
	memcpy(mac, eth_port.macaddr, sizeof(uint8_t) * 6);
}    


void receive_frame(){
    
	int32_t framesz;
    
	char* frame_ptr = (char*)MoveFromPreviousGuestGPR(REG_A0);
    
	/* Copy the message to the receiver */
	char* frame_ptr_mapped = (char*)tlbCreateEntry((uint32_t)frame_ptr, vm_in_execution->base_addr, MTU, 0xf, NONCACHEABLE);
    
	framesz = en_ll_input((uint8_t*)frame_ptr_mapped);
    
	/* Return the message size to the receiver */
	MoveToPreviousGuestGPR(REG_V0, framesz);
    
}

void send_frame(){
	/* Getting parameters from guest*/
	uint8_t *frame  = (uint8_t *)MoveFromPreviousGuestGPR(REG_A0);
	uint32_t size = MoveFromPreviousGuestGPR(REG_A1);
    
	char* frame_ptr_mapped = (char*)tlbCreateEntry((uint32_t)frame, vm_in_execution->base_addr, size, 0xf, CACHEABLE);
	uint8_t* buf = MACH_PHYS_TO_VIRT(MACH_VIRT_TO_PHYS(tx_buf));
    
	memcpy(buf,frame_ptr_mapped,size);
    
	MoveToPreviousGuestGPR(REG_V0, size);
    
	en_ll_output(buf, size);

}

/*
 * Read PHY register.
 * Return -1 when failed.
 */
static int32_t phy_read(int32_t phy_addr, int32_t reg_num, uint32_t msec)
{
	uint32_t time_start = mfc0(CP0_COUNT, 0);
	uint32_t timeout = msec * (CPU_SPEED / 2000);

	/* Clear any commands. */
	EMAC1MCMD = 0;
	while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
		if (mfc0(CP0_COUNT, 0) - time_start > timeout) {
			return -1;
		}
	}

	EMAC1MADR = PIC32_EMAC1MADR(phy_addr, reg_num);
	EMAC1MCMDSET = PIC32_EMAC1MCMD_READ;
	delay_ms(1);

	/* Wait to finish. */
	time_start = mfc0(CP0_COUNT, 0);
	while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
		if (mfc0(CP0_COUNT, 0) - time_start > timeout) {
			EMAC1MCMD = 0;
			return -1;
		}
	}

	EMAC1MCMD = 0;
	return EMAC1MRDD & 0xffff;
}

/*
 * Scan PHY register for expected value.
 * Return -1 when failed.
 */
static int32_t phy_scan(int32_t phy_addr, int32_t reg_num, int32_t scan_mask, int32_t expected_value, uint32_t msec)
{
	uint32_t time_start = mfc0(CP0_COUNT, 0);
	uint32_t timeout = msec * (CPU_SPEED / 2000);

	/* Clear any commands. */
	EMAC1MCMD = 0;
	while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
		if (mfc0(CP0_COUNT, 0) - time_start > timeout) {
			return -1;
		}
	}

	/* Scan the PHY until it is ready. */
	EMAC1MADR = PIC32_EMAC1MADR(phy_addr, reg_num);
	EMAC1MCMDSET = PIC32_EMAC1MCMD_SCAN;
	delay_ms(1);

	/* Wait for it to become valid. */
	time_start = mfc0(CP0_COUNT, 0);
	while (EMAC1MIND & PIC32_EMAC1MIND_NOTVALID) {
		if (mfc0(CP0_COUNT, 0) - time_start > timeout) {
			return -1;
		}
	}

	/* Wait until we hit our mask. */
	time_start = mfc0(CP0_COUNT, 0);
	while (((EMAC1MRDD & scan_mask) == scan_mask) != expected_value) {
		if (mfc0(CP0_COUNT, 0) - time_start > timeout) {
			return -1;
		}
	}

	/* Kill the scan. */
	EMAC1MCMD = 0;
	delay_ms(1);
	time_start = mfc0(CP0_COUNT, 0);
	while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
		if (mfc0(CP0_COUNT, 0) - time_start > timeout) {
			return -1;
		}
	}
	return 0;
}

/*
 * Write PHY register.
 * Return -1 when failed.
 */
static int32_t phy_write(int32_t phy_addr, int32_t reg_num, int32_t value, uint32_t msec)
{
	uint32_t time_start = mfc0(CP0_COUNT, 0);
	uint32_t timeout = msec * (CPU_SPEED / 2000);

	/* Clear any commands. */
	EMAC1MCMD = 0;
	while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
		if (mfc0(CP0_COUNT, 0) - time_start > timeout) {
			return -1;
		}	
	}

	EMAC1MADR = PIC32_EMAC1MADR(phy_addr, reg_num);
	EMAC1MWTD = value;
	delay_ms(1);

	/* Wait to finish. */
	time_start = mfc0(CP0_COUNT, 0);
	while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
		if (mfc0(CP0_COUNT, 0) - time_start > timeout) {
			return -1;
		}
	}
	return 0;
}

/*
 * Reset the PHY via MIIM interface.
 * Return -1 on failure.
 */
static int32_t phy_reset(int32_t phy_addr)
{
	int32_t advrt;
	int32_t advertise_all = PHY_ADVRT_10_HDX | PHY_ADVRT_10_FDX |
		PHY_ADVRT_100_HDX | PHY_ADVRT_100_FDX;

	/* Check ADVRT register is writable. */
	phy_write(phy_addr, PHY_ADVRT, 0, 100);
	advrt = phy_read(phy_addr, PHY_ADVRT, 1);
	if (advrt & advertise_all)
		return -1;
	phy_write(phy_addr, PHY_ADVRT, PHY_ADVRT_CSMA | advertise_all, 100);
	advrt = phy_read(phy_addr, PHY_ADVRT, 1);
	if ((advrt & advertise_all) != advertise_all)
		return -1;

	/* Send a reset to the PHY. */
	if (phy_write(phy_addr, PHY_CONTROL, PHY_CONTROL_RESET, 100) < 0)
		return -1;

	/* Wait for the reset pin to autoclear. */
	if (phy_scan(phy_addr, PHY_CONTROL, PHY_CONTROL_RESET, 0, 500) < 0)
		return -1;

	/* Advertise both 100Mbps and 10Mbps modes, full or half duplex. */
	phy_write(phy_addr, PHY_ADVRT, PHY_ADVRT_CSMA | advertise_all, 100);

	/* Restart autonegotiation. */
	phy_write(phy_addr, PHY_CONTROL, PHY_CONTROL_ANEG_EN | PHY_CONTROL_ANEG_RESTART, 100);

	return 0;
}

/*
 * Get the speed and duplex mode of LAN87x0A chip.
 */
static void phy_lan87x0a_poll(int32_t phy_addr, int32_t *speed_100, int32_t *full_duplex)
{
	/* Read 87x0A-specific register #31. */
	int32_t special = phy_read(phy_addr, 31, 1);

	if (special & PHY_LAN87x0A_AUTODONE) {
		/* Auto-negotiation is done - get the speed. */
		*speed_100 = (special & PHY_LAN87x0A_100) != 0;
		*full_duplex = (special & PHY_LAN87x0A_FDX) != 0;
	}
}

/*
 * Determine whether the link is up.
 * When up, get the speed and duplex mode.
 */
static int32_t is_phy_linked(int32_t phy_addr, int32_t *speed_100, int32_t *full_duplex)
{
	int32_t status = phy_read(phy_addr, PHY_STATUS, 1);
	if (status < 0)
		return 0;
	if (! (status & PHY_STATUS_LINK))           /* Is link up? */
		return 0;
	if (! (status & PHY_STATUS_ANEG_ACK))       /* Is auto-negotiation done? */
		return 0;

	phy_lan87x0a_poll(phy_addr, speed_100, full_duplex);

	return 1;
}

/*
 * Initialize the Ethernet Controller.
 */
static void en_setup()
{
	int32_t empty_watermark, full_watermark;

	/* Disable the ethernet interrupt. */
	IECCLR(IRQ_ETH >> 5) = 1 << (IRQ_ETH & 31);

	/* Turn the Ethernet cotroller off. */
	ETHCON1CLR = PIC32_ETHCON1_ON | PIC32_ETHCON1_RXEN | PIC32_ETHCON1_TXRTS;

	/* Wait for abort to finish. */
	while (ETHSTAT & PIC32_ETHSTAT_ETHBUSY);

	/* Clear the interrupt flag bit. */
	IFSCLR(IRQ_ETH >> 5) = 1 << (IRQ_ETH & 31);	

	/* Clear interrupts. */
	ETHIEN = 0;
	ETHIRQ = 0;

	/* Clear discriptor pointers; for now. */
	ETHTXST = 0;
	ETHRXST = 0;

	/* High and low watermarks. */
	empty_watermark = MTU / RX_BYTES_PER_DESC;
	full_watermark  = RX_DESCRIPTORS - (MTU * 2) / RX_BYTES_PER_DESC;
	ETHRXWM = PIC32_ETHRXWM_FWM(full_watermark) | PIC32_ETHRXWM_EWM(empty_watermark);

	/* Set RX descriptor buffer size in bytes (aligned to 16 bytes). */
	ETHCON2 = RX_BYTES_PER_DESC;

	/* Set our Rx filters. */
	ETHRXFC = PIC32_ETHRXFC_CRCOKEN |   /* enable checksum filter */
		PIC32_ETHRXFC_RUNTEN |      /* enable short packets */
		PIC32_ETHRXFC_UCEN |        /* enable unicast filter */
		PIC32_ETHRXFC_BCEN;     /* enable broadcast filter */

	/* Hash table, not used. */
	ETHHT0 = 0;
	ETHHT1 = 0;

	/* Pattern match, not used. */
	ETHPMM0 = 0;
	ETHPMM1 = 0;

	/* Byte in TCP like checksum pattern calculation. */
	ETHPMCS = 0;

	/* Turn on the ethernet controller. */
	ETHCON1 = PIC32_ETHCON1_ON;
}

/*
 * Initialize the MAC.
 */
static void en_setup_mac()
{
	/* Reset the MAC. */
	EMAC1CFG1 = PIC32_EMAC1CFG1_SOFTRESET;
	delay_ms(1);

	/* Pull it out of reset. */
	EMAC1CFG1 = 0;
	delay_ms(1);
	EMAC1CFG1 = PIC32_EMAC1CFG1_RXENABLE |          /* Receive enable */
                PIC32_EMAC1CFG1_TXPAUSE |   /* MAC TX flow control */
                PIC32_EMAC1CFG1_RXPAUSE;    /* MAC RX flow control */

	EMAC1CFG2 = PIC32_EMAC1CFG2_PADENABLE |         /* Pad/CRC enable */
                PIC32_EMAC1CFG2_CRCENABLE | /* CRC enable */
                PIC32_EMAC1CFG2_EXCESSDFR |
                PIC32_EMAC1CFG2_AUTOPAD |
                PIC32_EMAC1CFG2_LENGTHCK;

	EMAC1MAXF = 1518;                   /* max frame size in bytes */
	EMAC1IPGR = PIC32_EMAC1IPGR(12, 18);            /* non-back-to-back interpacket gap */
	EMAC1CLRT = PIC32_EMAC1CLRT(55, 15);            /* collision window/retry limit */
}

/*
 * Initialize RMII and MIIM.
 */
static void en_setup_rmii()
{
	EMAC1SUPP = PIC32_EMAC1SUPP_RESETRMII;          /* reset RMII */
	delay_ms(1);
	EMAC1SUPP = 0;

	EMAC1MCFG = PIC32_EMAC1MCFG_RESETMGMT;          /* reset the management fuctions */
	delay_ms(1);
	EMAC1MCFG = 0;

	/* The IEEE 802.3 spec says no faster than 2.5MHz.
	* 80 / 40 = 2MHz */
	EMAC1MCFG = PIC32_EMAC1MCFG_CLKSEL_40;
}

void en_enable_interrupts() {
	uint32_t offset;
	/* Enable interrupts. */
#if 0    
	ETHIENSET = PIC32_ETHIRQ_TXBUSE |           /* Transmit Bus Error */
                PIC32_ETHIRQ_TXDONE |       /* Transmit Done */
                PIC32_ETHIRQ_TXABORT |      /* Transmit Abort */
                PIC32_ETHIRQ_RXBUSE |       /* Receive Bus Error */
                PIC32_ETHIRQ_RXDONE |       /* Receive Done */
                PIC32_ETHIRQ_RXBUFNA |      /* Receive Buffer Not Available */
                PIC32_ETHIRQ_RXOVFLW;       /* Receive FIFO Overflow */
#endif 
	offset = register_interrupt(ethernet_interrupt_handler);
	OFF(IRQ_ETH) = offset;

	INFO("Ethernet interrupt %d registered at 0x%x", IRQ_ETH, offset);
    
	ETHIENSET = PIC32_ETHIRQ_RXDONE;
    
	/* Set IPL , sub-priority 0 */
	IPCSET(38) = 0x1f00;
    
	/* Clear interrupt */
	IFSCLR(IRQ_ETH >> 5) = (1<<25);
	ETHIRQCLR = ETHIRQ;

	IECSET(IRQ_ETH >> 5) = 1 << 25;
}

/*
 * Set DMA descriptors.
 */
void en_setup_dma() {
	struct eth_port *e = &eth_port;
	int32_t i;

	/* Set Rx discriptor list.
	* All owned by the ethernet controller. */
	memset(e->rx_desc, 0, (RX_DESCRIPTORS+1) * sizeof(eth_desc_t));
	for (i=0; i<RX_DESCRIPTORS; i++) {
		DESC_SET_EOWN(&e->rx_desc[i]);
		DESC_CLEAR_NPV(&e->rx_desc[i]);
		e->rx_desc[i].paddr = MACH_VIRT_TO_PHYS(&e->rx_buf[0] + (i * RX_BYTES_PER_DESC));
	}

	/* Loop the list back to the begining.
	* This is a circular array descriptor list. */
	e->rx_desc[RX_DESCRIPTORS].hdr = MACH_VIRT_TO_PHYS(&e->rx_desc[0]);
	DESC_SET_NPV(&e->rx_desc[RX_DESCRIPTORS-1]);

	/* Set RX at the start of the list. */
	receive_index = 0;
	ETHRXST = MACH_VIRT_TO_PHYS(&e->rx_desc[0]);

	/* Set up the transmit descriptors all owned by
	* the software; clear it completely out. */
	memset(e->tx_desc, 0, (TX_DESCRIPTORS+1) * sizeof(eth_desc_t));
	ETHTXST = MACH_VIRT_TO_PHYS(&e->tx_desc[0]);
}

/*
 * ethernet low level input (receive)
 * 
 * a raw ethernet frame is fetched from the reception buffer (controlled by
 * the RX DMA) and copied to the application buffer. Warning: the application
 * buffer should be cache-coherent.
 */
int32_t en_ll_input(uint8_t *frame) {
	struct eth_port *e = &eth_port;
	uint8_t *buf = frame;
	uint16_t size;
	read_index = receive_index;
	desc_offset = 0;
	read_nbytes = 0;

	if (!e->is_up) return 0;

	if (DESC_EOWN(&e->rx_desc[receive_index])) {
		/* There are no receive descriptors to process. */
		return 0;
	}
	frame_size = DESC_FRAMESZ(&e->rx_desc[receive_index]);
	size = frame_size;

	if (frame_size == 0) return 0;

	/* make sure we own the descriptor, bad if we don't! */
	while (frame_size > 0) {
		if (DESC_EOWN(&e->rx_desc[read_index]))
		break;

		int32_t end_of_packet = DESC_EOP(&e->rx_desc[read_index]);
		uint32_t nbytes = DESC_BYTECNT(&e->rx_desc[read_index]);
		uint32_t cb     = min(nbytes - desc_offset, frame_size);

		memcpy(buf, MACH_PHYS_TO_VIRT(e->rx_desc[read_index].paddr + desc_offset), cb);
		buf         += cb;	
		desc_offset += cb;
		read_nbytes += cb;
		frame_size       -= cb;

		/* if we read the whole descriptor page */
		if (desc_offset == nbytes) {
			/* set up for the next page */
			desc_offset = 0;
			read_index = INCR_RX_INDEX(read_index);

			/* if we are done, get out */
			if (end_of_packet || read_nbytes == frame_size)
				break;
		}
	}

	/* Free the receive descriptors. */
	while (!DESC_EOWN(&e->rx_desc[receive_index])){
		int32_t end_of_packet = DESC_EOP(&e->rx_desc[receive_index]);

		DESC_SET_EOWN(&e->rx_desc[receive_index]);  /* give up ownership */
		ETHCON1SET = PIC32_ETHCON1_BUFCDEC;     /* decrement the BUFCNT */
		receive_index = INCR_RX_INDEX(receive_index);   /* check the next one */

		/* hit the end of packet */
		if (end_of_packet)
			break;
	}

	read_index = 0;
	desc_offset = 0;
	frame_size = 0;
	read_nbytes = 0;

	return size;
}

/*
 * ethernet low level output (send)
 * 
 * a raw ethernet frame is copied from the application buffer to the transmission
 * buffer by the TX DMA engine. Warning: the application buffer should be
 * cache-coherent.
 */
void en_ll_output(uint8_t *frame, uint16_t size) {
	struct eth_port *e = &eth_port;
	volatile eth_desc_t *desc = &e->tx_desc[0];
    
	if (size > 0 && size <= MTU) {
		while(ETHCON1 & PIC32_ETHCON1_TXRTS);

		desc->hdr = 0;
		desc->paddr = MACH_VIRT_TO_PHYS(frame);
		DESC_SET_BYTECNT(desc, size);
		DESC_SET_SOP(desc); /* Start of packet */
		DESC_SET_EOWN(desc);    /* Set owner */
		DESC_SET_EOP(desc); /* End of packet */

		/* Set the descriptor table to be transmitted. */
		ETHTXST = MACH_VIRT_TO_PHYS(e->tx_desc);

		/* Start transmitter. */
		ETHCON1SET = PIC32_ETHCON1_TXRTS;
    }
    
}

/*
 * ethernet watchdog
 * 
 * this routine should be called periodically (~500ms) from the application.
 * the link will be stablished and monitored by this routine (link up/down).
 * no transmission / reception will work if the link is down.
 */
void en_watchdog(void)
{
	struct eth_port *e = &eth_port;
	int32_t receiver_enabled = (ETHCON1 & PIC32_ETHCON1_RXEN);
	int32_t speed_100 = 1, full_duplex = 1;

	/* Poll whether the link is active.
	* Get speed and duplex status from the PHY. */
	e->is_up = is_phy_linked(e->phy_addr, &speed_100, &full_duplex);
    
	MoveToPreviousGuestGPR(REG_V0, e->is_up);

	/* Check whether RX is enabled. */
	if (e->is_up && ! receiver_enabled) {
		/* Link activated. */
		INFO("en0: link up, %s, %s duplex",
			speed_100 ? "100Mbps" : "10Mbps",
			full_duplex ? "full" : "half");

		/* Set speed. */
		if (speed_100) {
			EMAC1SUPPSET = PIC32_EMAC1SUPP_SPEEDRMII;
		} else {
			EMAC1SUPPCLR = PIC32_EMAC1SUPP_SPEEDRMII;
		}

		/* Set duplex. */
		if (full_duplex) {
			EMAC1CFG2SET = PIC32_EMAC1CFG2_FULLDPLX;
		} else {
			EMAC1CFG2CLR = PIC32_EMAC1CFG2_FULLDPLX;
		}

		/* Set gap size. */
		EMAC1IPGT = full_duplex ? 21 : 18;

		en_setup_dma();

		ETHCON1SET = PIC32_ETHCON1_RXEN;
	}
	else if (! e->is_up && receiver_enabled) {
		/* Link down. */
		INFO("en0: link down");
		ETHCON1CLR = PIC32_ETHCON1_RXEN;
		while (ETHSTAT & PIC32_ETHSTAT_RXBUSY);
	}
}

/*
 * Different devices can have different pin assignments,
 * depending on pin count and DEVCFG.FETHIO configuration setting.
 */
static void setup_signals()
{
	switch (DEVID & 0x0fffffff) {
		case 0x05104053:            /* MZ2048ECG064 */
		case 0x05109053:            /* MZ2048ECH064 */
		case 0x05131053:            /* MZ2048ECM064 */
		case 0x07203053:            /* MZ1024EFG064 */
		case 0x07204053:            /* MZ2048EFG064 */
		case 0x07208053:            /* MZ1024EFH064 */
		case 0x07209053:            /* MZ2048EFH064 */
		case 0x07230053:            /* MZ1024EFM064 */
		case 0x07231053:            /* MZ2048EFM064 */
		
			if (*(uint32_t*)_DEVCFG3 & _DEVCFG3_FETHIO) {
				/*
				* Default setup for 64-pin device.
				*/
				ANSELECLR = 1 << 4;         /* Disable analog pad on RE4 for ERXERR */
				ANSELECLR = 1 << 6;         /* Disable analog pad on RE6 for ETXD0 */
				ANSELECLR = 1 << 7;         /* Disable analog pad on RE7 for ETXD1 */
				ANSELECLR = 1 << 5;         /* Disable analog pad on RE5 for ETXEN */
				ANSELBCLR = 1 << 15;        /* Disable analog pad on RB15 for EMDC */

				LATECLR = 1 << 6; TRISECLR = 1 << 6;    /* set RE6 as output for ETXD0 */
				LATECLR = 1 << 7; TRISECLR = 1 << 7;    /* set RE7 as output for ETXD1 */
				LATECLR = 1 << 5; TRISECLR = 1 << 5;    /* set RE5 as output for ETXEN */
			} else {
				/*
				* Alternative setup for 64-pin device.
				*/
				ANSELBCLR = 1 << 15;        /* Disable analog pad on RB15 for AEMDC */

				LATFCLR = 1 << 1; TRISFCLR = 1 << 1;    /* set RF1 as output for AETXD0 */
				LATFCLR = 1 << 0; TRISFCLR = 1 << 0;    /* set RF0 as output for AETXD1 */
				LATDCLR = 1 << 2; TRISDCLR = 1 << 2;    /* set RD2 as output for AETXEN */
			}
			break;
			
		case 0x0510E053:            /* MZ2048ECG100 */
		case 0x05113053:            /* MZ2048ECH100 */
		case 0x0513B053:            /* MZ2048ECM100 */
		case 0x0720D053:            /* MZ1024EFG100 */
		case 0x0720E053:            /* MZ2048EFG100 */
		case 0x07212053:            /* MZ1024EFH100 */
		case 0x07213053:            /* MZ2048EFH100 */
		case 0x0723A053:            /* MZ1024EFM100 */
		case 0x0723B053:            /* MZ2048EFM100 */
			if (*(uint32_t*)_DEVCFG3 & _DEVCFG3_FETHIO) {
				/*
				* Default setup for 100-pin devices.
				*/
				ANSELGCLR = 1 << 9;         /* Disable analog pad on RG9 for EREFCLK */
				ANSELBCLR = 1 << 12;        /* Disable analog pad on RB12 for ERXD0 */
				ANSELBCLR = 1 << 13;        /* Disable analog pad on RB13 for ERXD1 */
				ANSELGCLR = 1 << 8;         /* Disable analog pad on RG8 for ECRSDV */
				ANSELBCLR = 1 << 11;        /* Disable analog pad on RB11 for ERXERR */

				LATFCLR = 1 << 1; TRISFCLR = 1 << 1;    /* set RF1 as output for ETXD0 */
				LATFCLR = 1 << 0; TRISFCLR = 1 << 0;    /* set RF0 as output for ETXD1 */
				LATDCLR = 1 << 2; TRISDCLR = 1 << 2;    /* set RD2 as output for ETXEN */
			} else {
				/*
				* Alternative setup for 100-pin devices.
				*/
				ANSELGCLR = 1 << 9;         /* Disable analog pad on RG9 for AEREFCLK */
				ANSELECLR = 1 << 8;         /* Disable analog pad on RE8 for AERXD0 */
				ANSELECLR = 1 << 9;         /* Disable analog pad on RE9 for AERXD1 */
				ANSELGCLR = 1 << 8;         /* Disable analog pad on RG8 for AECRSDV */
				ANSELGCLR = 1 << 15;        /* Disable analog pad on RG15 for AERXERR */
				ANSELDCLR = 1 << 14;        /* Disable analog pad on RD14 for AETXD0 */
				ANSELDCLR = 1 << 15;        /* Disable analog pad on RD15 for AETXD1 */

				LATDCLR = 1 << 14; TRISDCLR = 1 << 14;  /* set RD14 as output for AETXD0 */
				LATDCLR = 1 << 15; TRISDCLR = 1 << 15;  /* set RD15 as output for AETXD1 */
				LATACLR = 1 << 15; TRISACLR = 1 << 15;  /* set RA15 as output for AETXEN */
			}
			break;
		case 0x05118053:            /* MZ2048ECG124 */
		case 0x0511D053:            /* MZ2048ECH124 */
		case 0x05145053:            /* MZ2048ECM124 */
		case 0x07217053:            /* MZ1024EFG124 */
		case 0x07218053:            /* MZ2048EFG124 */
		case 0x0721C053:            /* MZ1024EFH124 */
		case 0x0721D053:            /* MZ2048EFH124 */
		case 0x07244053:            /* MZ1024EFM124 */
		case 0x07245053:            /* MZ2048EFM124 */
			WARNING("124-pin devices not supported yet");
			break;
		case 0x05122053:            /* MZ2048ECG144 */
		case 0x05127053:            /* MZ2048ECH144 */
		case 0x0514F053:            /* MZ2048ECM144 */
		case 0x07221053:            /* MZ1024EFG144 */
		case 0x07222053:            /* MZ2048EFG144 */
		case 0x07226053:            /* MZ1024EFH144 */
		case 0x07227053:            /* MZ2048EFH144 */
		case 0x0724E053:            /* MZ1024EFM144 */
		case 0x0724F053:            /* MZ2048EFM144 */
			/*
			* Setup for 144-pin devices.
			*/
			ANSELJCLR = 1 << 11;        /* Disable analog pad on RJ11 for EREFCLK */
			ANSELHCLR = 1 << 5;         /* Disable analog pad on RH5 for ERXD1 */
			ANSELHCLR = 1 << 4;         /* Disable analog pad on RH4 for ERXERR */
			ANSELJCLR = 1 << 8;         /* Disable analog pad on RJ8 for ETXD0 */
			ANSELJCLR = 1 << 9;         /* Disable analog pad on RJ9 for ETXD1 */

			LATJCLR = 1 << 8; TRISJCLR = 1 << 8;    /* set RJ8 as output for ETXD0 */
			LATJCLR = 1 << 9; TRISJCLR = 1 << 9;    /* set RJ9 as output for ETXD1 */
			LATDCLR = 1 << 6; TRISDCLR = 1 << 6;    /* set RD6 as output for ETXEN */
			break;
		default:
			WARNING("DEVID not recognized\n");
	}
}

static void en_init(){
	struct eth_port *e = &eth_port;

	/* Board-dependent initialization. */
	setup_signals();

	/* Link is down. */
	e->is_up = 0;

	/* As per section 35.4.10 of the Pic32 Family Ref Manual. */
	en_setup();
	en_setup_mac();
	en_setup_rmii();

	/* Auto-detect the PHY address, 0-31. */
	for (e->phy_addr=0; e->phy_addr<32; e->phy_addr++) {
		if (phy_reset(e->phy_addr) >= 0)
		break;
	}
	if (e->phy_addr >= 32) {
		ETHCON1 = 0;
		WARNING("Ethernet PHY not detected\n");
		return;
	}else{
		INFO("Ethernet PHY at %d", e->phy_addr);
	}

	/* Extract our MAC address */
	e->macaddr[0] = EMAC1SA2;
	e->macaddr[1] = EMAC1SA2 >> 8;
	e->macaddr[2] = EMAC1SA1;
	e->macaddr[3] = EMAC1SA1 >> 8;
	e->macaddr[4] = EMAC1SA0;
	e->macaddr[5] = EMAC1SA0 >> 8;

	/*
	* Interface exists: make available by filling in network interface
	* record.  System will initialize the interface when it is ready
	* to accept packets.  We get the ethernet address here.
	*/
	INFO("Ethernet interface en0: interrupt %d, MAC address %x:%x:%x:%x:%x:%x",
		IRQ_ETH, e->macaddr[0], e->macaddr[1], e->macaddr[2], e->macaddr[3], e->macaddr[4], e->macaddr[5]);

	e->phy_id = (phy_read(e->phy_addr, PHY_ID1, 1) << 16 |
		phy_read(e->phy_addr, PHY_ID2, 1)) & 0xfffffff0;
	switch (e->phy_id) {
		case PHY_ID_LAN8720A:
			INFO("Ethernet device is a SMSC LAN8720A");
			break;
		case PHY_ID_LAN8740A:
			INFO("Ethernet device is a SMSC LAN8740A");
			break;
		default:
			INFO("PHY id=%x", e->phy_id);
		break;
	}

	/* allocate buffers and change pointers from kseg0 to kseg1 (non-cachable)
	* this will make DMA buffers coherent because we avoid acessing the L1 cache. */
	e->rx_buf = MACH_PHYS_TO_VIRT(MACH_VIRT_TO_PHYS((int8_t *)malloc(RX_BYTES * sizeof(int8_t))));
	e->rx_desc = MACH_PHYS_TO_VIRT(MACH_VIRT_TO_PHYS((eth_desc_t *)malloc((RX_DESCRIPTORS+1) * sizeof(eth_desc_t))));
	e->tx_desc = MACH_PHYS_TO_VIRT(MACH_VIRT_TO_PHYS((eth_desc_t *)malloc((TX_DESCRIPTORS+1) * sizeof(eth_desc_t))));
	if (!e->rx_buf || !e->rx_desc || !e->tx_desc){
		CRITICAL("\nEthernet Driver: Out of Memory - Hypervisor Halted.");
	}
    
	if (register_hypercall(en_watchdog, HCALL_ETHERNET_WATCHDOG) < 0){
		ERROR("Error registering the HCALL_GET_VM_ID hypercall");
		return;
	}

	if (register_hypercall(en_get_mac, HCALL_ETHERNET_GET_MAC) < 0){
		ERROR("Error registering the HCALL_GET_VM_ID hypercall");
		return;
	}

	if (register_hypercall(send_frame, HCALL_ETHERNET_SEND) < 0){
		ERROR("Error registering the HCALL_GET_VM_ID hypercall");
		return;
	}

	if (register_hypercall(receive_frame, HCALL_ETHERNET_RECV) < 0){
		ERROR("Error registering the HCALL_GET_VM_ID hypercall");
		return;
	}
    
	/* check if there is a VCPU associated to ethernet interrutps. */
	vcpu = get_fast_int_vcpu_node(IRQ_ETH);
	if (vcpu){
		en_enable_interrupts();
	}

	return;
}

driver_init(en_init);