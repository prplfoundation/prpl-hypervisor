/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */

#include <libc.h>
#include <pic32mz.h>

#include "enc28j60.h"

#include "pico_device.h"
#include "pico_dev_pic32.h"
#include "pico_stack.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/* Use pin RB3 for CS */
#define SPI_SEL()       LATBCLR = 0x0008;
#define SPI_DESEL()     LATBSET = 0x0008;
#define LOWER_BYTE(x)   ((x) & 0xFF)
#define UPPER_BYTE(x)   ((x) >> 8)

/* Check if SPI1STAT.SPIRBF is set */
#ifndef SPI1_Rx_Buf_Full
#define SPI1_Rx_Buf_Full SPI1STAT & SPISTAT_SPIRBF
#endif

/* Check if SPI1STAT.SPITBF is set */
#ifndef SPI1_Tx_Buf_Full
#define SPI1_Tx_Buf_Full SPI1STAT & SPISTAT_SPITBF
#endif

/* Check if SPI1STAT.SPIBUSY is set */
#ifndef SPI1_Busy
#define SPI1_Busy SPI1STAT & SPISTAT_SPIBUSY
#endif
/******************************************************************************/
/* Function Declaration                                                       */
/******************************************************************************/

static void spiInit(const uint8_t *mac_address);
static void spiSoftReset();
static uint8_t spiReadEthControl(uint8_t address);
static uint8_t spiReadMacMiiControl(uint8_t address);
static uint8_t spiWriteControl(uint8_t address, uint8_t data);
static uint8_t spiBitSet(uint8_t address, uint8_t mask);
static uint8_t spiBitClear(uint8_t address, uint8_t mask);
static void spiSelectBank(uint8_t bank);
static uint8_t spiReadBuffer();
static uint8_t spiReadFullBuffer(uint8_t* pBuffer, uint16_t len);
static uint16_t spiReadPhyControl(uint8_t address);
static void spiWritePhyControl(uint8_t address, uint16_t data);
static uint16_t readBuffer(uint8_t* pBuffer);
static uint16_t writeBuffer(uint8_t* pBuffer, uint16_t size);
static uint8_t spiReadByte(void);
static void spiWriteByte(uint8_t data);

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

uint16_t RXstart_init = 0x0000;
uint16_t RXstop_init = 0x19FE;
uint16_t TXstart_init = 0x19FF;
uint16_t TXstop_init = 0x1FFF;

uint16_t next_packet_pointer = 0x0000;

volatile uint32_t tick_counter = 0;
volatile uint32_t us_counter = 0;

/******************************************************************************/
/* Main Functions                                                               */
/******************************************************************************/

static int pico_pic32_send(struct pico_device *dev, void *buf, int len)
{
  writeBuffer(buf, len);
  return len;
}

static int pico_pic32_poll(struct pico_device *dev, int loop_score)
{
  
  // Check for packet availability
  spiSelectBank(SPI_BANK1);
  
  uint8_t numPackets = 0;
  if (0 < (numPackets = spiReadEthControl(EPKTCNT))) {
    uint8_t rx_buffer[0x05EE];
    uint16_t num_bytes = readBuffer(rx_buffer);
    pico_stack_recv(dev, rx_buffer, num_bytes-4);
    loop_score--;
  }
  return loop_score;
}

// Create function for this device

struct pico_device *pico_eth_create(const char *name, const uint8_t *mac)
{
  struct pico_device *dev = pico_zalloc(sizeof(struct pico_device));
  uint8_t phy_rst;

  if (!dev)
    return NULL;

  if (0 != pico_device_init(dev, name, mac)) {
    // Initialization failed
    return NULL;
  }

  dev->overhead = 0;
  dev->send = pico_pic32_send;
  dev->poll = pico_pic32_poll;

#if 0
  while (1)
  {
  spiBitClear(0xbf, 0x00);
//  udelay(100);
  spiBitClear(0xbf, 0x01);
  udelay(100);
  spiBitClear(0xbf, 0x02);
  udelay(100);
  spiBitClear(0xbf, 0x03);
  udelay(100);
  }

  while(1);
#endif
  SPI_DESEL();
  spiSoftReset();
  udelay(50);
  while(spiReadPhyControl(PHCON1) & PHCON1_PRST);

  // Initialize ENC28J60
  spiInit(mac);
#if 0
  uint16_t phy1 = spiReadPhyControl(PHCON1);
  uint16_t phy2 = spiReadPhyControl(PHCON2);
  uint16_t phyl = spiReadPhyControl(PHLCON);
  spiSelectBank(SPI_BANK2);
  uint8_t macon3 = spiReadMacMiiControl(MACON3);
  uint8_t econ1 = spiReadEthControl(SPI_ECON1);

  printf("phy1 = %04x\n", phy1);
  printf("phy2 = %04x\n", phy2);
  printf("phyl = %04x\n", phyl);
  printf("macon3 = %02x\n", macon3);
  printf("econ1 = %02x\n", econ1);
#endif
  return dev;
}

static void spiInit(const uint8_t * mac_address)
{
  spiSelectBank(SPI_BANK0);

  // Configure the RX start position in memory
  spiWriteControl(ERXSTL, LOWER_BYTE(RXstart_init));
  spiWriteControl(ERXSTH, UPPER_BYTE(RXstart_init));

  // Configure the RX pointer
  spiWriteControl(ERXRDPTL, LOWER_BYTE(RXstart_init));
  spiWriteControl(ERXRDPTH, UPPER_BYTE(RXstart_init));

  spiWriteControl(ERDPTL, LOWER_BYTE(RXstart_init));
  spiWriteControl(ERDPTH, UPPER_BYTE(RXstart_init));

  // Configure the RX stop position in memory
  spiWriteControl(ERXNDL, LOWER_BYTE(RXstop_init));
  spiWriteControl(ERXNDH, UPPER_BYTE(RXstop_init));

  // Configure the TX start position in memory
  spiWriteControl(ETXSTL, LOWER_BYTE(TXstart_init));
  spiWriteControl(ETXSTH, UPPER_BYTE(TXstart_init));

  // Configure the TX stop position in memory
  spiWriteControl(ETXNDL, LOWER_BYTE(TXstop_init));
  spiWriteControl(ETXNDH, UPPER_BYTE(TXstop_init));

  // Filter configuration
  spiSelectBank(SPI_BANK1);
  spiWriteControl(ERXFCON, 0x00); // Set to promiscuous mode

  spiSelectBank(SPI_BANK2);

  // Enable MAC, enable TX and RX pause control
  spiWriteControl(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
  // Enable all hardware
  // Enable 60 byte padding, CRC and frame len calculation.
  //spiBitSet(MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN); Changed to WriteControl
  spiWriteControl(MACON3,  MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
  // Enable deferring transmission indefinitely(802.3 compliance)
  spiWriteControl(MACON4, MACON4_DEFER);
  // Set inter-frame gap (non-back-to-back) ~ Recommended settings by datasheet
  spiWriteControl(MAIPGL, 0x12);
  spiWriteControl(MAIPGH, 0x0C);
  // Set inter-frame gap (back-to-back) ~ Recommended settings by datasheet
  spiWriteControl(MABBIPG, 0x12);
  // Set maximum framelen size (0x5EE = 1518 bytes)
  spiWriteControl(MAMXFLL, 0xEE);
  spiWriteControl(MAMXFLH, 0x05);

  spiSelectBank(SPI_BANK3);

  // Set the mac address (in reverse)
  spiWriteControl(MAADR5, mac_address[0]);
  spiWriteControl(MAADR4, mac_address[1]);
  spiWriteControl(MAADR3, mac_address[2]);
  spiWriteControl(MAADR2, mac_address[3]);
  spiWriteControl(MAADR1, mac_address[4]);
  spiWriteControl(MAADR0, mac_address[5]);

  // Disable loopback
  spiWritePhyControl(PHCON2, PHCON2_HDLDIS);
  //spiWritePhyControl(PHCON1, 0x00);
  // Make LEDs(green/orange) blink when there is Rx/Tx activity respectively
  spiWritePhyControl(PHLCON, 0x3212);

  spiSelectBank(SPI_BANK0);

  // Enable packet reception
  spiBitSet(SPI_ECON1, ECON1_RXEN);
}

static void spiSoftReset()
{
  SPI_SEL();
//  udelay(1);
  spiWriteByte(SPI_OPCODE_RES);
//  SPI1BUF = SPI_OPCODE_RES; // address with opcode mask
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  volatile uint8_t tmp = SPI1BUF;
//  udelay(1);
//  while(SPI1_Busy);
  SPI_DESEL();
}

static uint8_t spiReadEthControl(uint8_t address)
{
  SPI_SEL();
//  udelay(1);
  spiWriteByte((address & SPI_ADDR_MASK) | SPI_OPCODE_RCR);
//  spiWriteByte(0x00);
  address = spiReadByte();
//  SPI1BUF = (address & SPI_ADDR_MASK) | SPI_OPCODE_RCR; // address with opcode mask
//  while (!SPI1_Rx_Buf_Full); // wait till start of transmission
//  address = SPI1BUF;
//  udelay(1);
//  udelay(1);
//  SPI1BUF = 0x00; // Send dummy byte
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  address = SPI1BUF;
//  udelay(1);
//  while(SPI1_Busy);
  SPI_DESEL();
  return address;
}

static uint8_t spiReadMacMiiControl(uint8_t address)
{
  SPI_SEL();
//  udelay(1);
  udelay(1);
  spiWriteByte((address & SPI_ADDR_MASK) | SPI_OPCODE_RCR);
  spiWriteByte(0x00);
//  spiWriteByte(0x00);
  address = spiReadByte();
//  SPI1BUF = (address & SPI_ADDR_MASK) | SPI_OPCODE_RCR; // address with opcode mask
//  while (!SPI1_Rx_Buf_Full); // wait till start of transmission
//  address = SPI1BUF; // Read dummy byte
//  udelay(1);
//  udelay(1);
//  SPI1BUF = 0x00; // Send dummy byte
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  address = SPI1BUF;
//  udelay(1);
//  udelay(1);
//  SPI1BUF = 0x00; // Send dummy byte
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  address = SPI1BUF;
//  udelay(1);
//  while(SPI1_Busy);
  udelay(1);
  SPI_DESEL();
  return address;
}

static uint8_t spiWriteControl(uint8_t address, uint8_t data)
{
  SPI_SEL();
//  udelay(1);
  udelay(1);
  spiWriteByte((address & SPI_ADDR_MASK) | SPI_OPCODE_WCR);
  spiWriteByte(data);
//  SPI1BUF = (address & SPI_ADDR_MASK) | SPI_OPCODE_WCR;
//  while (!SPI1_Rx_Buf_Full); // wait till start of transmission
//  address = SPI1BUF; // Read dummy byte
//  udelay(1);
//  udelay(1);
//  SPI1BUF = data; // Send dummy byte
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  address = SPI1BUF;
//  while(SPI1_Busy);
  udelay(1);
//  udelay(1);
  SPI_DESEL();
  return address;
}

// This function will perform a bitwise OR operation on
// an ETH control register

static uint8_t spiBitSet(uint8_t address, uint8_t mask)
{
  SPI_SEL();
//  udelay(1);
  spiWriteByte((address & SPI_ADDR_MASK) | SPI_OPCODE_BFS);
  spiWriteByte(mask);
//  SPI1BUF = (address & SPI_ADDR_MASK) | SPI_OPCODE_BFS;
//  while (!SPI1_Rx_Buf_Full); // wait till start of transmission
//  address = SPI1BUF; // Read dummy byte
//  udelay(1);
//  udelay(1);
//  SPI1BUF = mask; // Send dummy byte
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  address = SPI1BUF;
//  udelay(1);
//  while(SPI1_Busy);
  SPI_DESEL();
  return address;
}

// This function will perform a bitwise NOTAND operation on
// and ETH control register

static uint8_t spiBitClear(uint8_t address, uint8_t mask)
{
  SPI_SEL();
//  udelay(1)
  spiWriteByte((address & SPI_ADDR_MASK) | SPI_OPCODE_BFC);
  spiWriteByte(mask);
//  SPI1BUF = (address & SPI_ADDR_MASK) | SPI_OPCODE_BFC;
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  address = SPI1BUF; // Read dummy byte
//  udelay(1);
//  udelay(1);
//  SPI1BUF = mask; // Send dummy byte
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  address = SPI1BUF; // read actual address
//  udelay(1);
//  while(SPI1_Busy);
  SPI_DESEL();
  return address;
}

// provide bank number 0, 1, 2 or 3

static void spiSelectBank(uint8_t bank)
{
  spiBitClear(SPI_ECON1, SPI_BANK_MASK); // Clean bank select bits
  spiBitSet(SPI_ECON1, (bank & SPI_BANK_MASK)); // Set bank
}

static uint8_t spiReadBuffer()
{
  uint8_t value;
  SPI_SEL();
//  udelay(1);
  spiWriteByte((0b011010 & SPI_ADDR_MASK) | SPI_OPCODE_RBM);
//  spiWriteByte(0x00);
  value = spiReadByte();
//  SPI1BUF = (0b011010 & SPI_ADDR_MASK) | SPI_OPCODE_RBM; // Read Buffer Mem
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  value = SPI1BUF; // Read dummy byte
//  udelay(1);
//  udelay(1);
//  SPI1BUF = 0x00; // Send dummy byte
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  value = SPI1BUF;
//  udelay(1);
//  while(SPI1_Busy);
  SPI_DESEL();
  return value;
}

static uint8_t spiReadFullBuffer(uint8_t* pBuffer, uint16_t len)
{
  uint16_t cur_pos = 0;
  uint8_t value;
  SPI_SEL();
//  udelay(1);
  spiWriteByte((0b011010 & SPI_ADDR_MASK) | SPI_OPCODE_RBM);
//  SPI1BUF = (0b011010 & SPI_ADDR_MASK) | SPI_OPCODE_RBM; // Read Buffer Mem
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  value = SPI1BUF; // Read dummy byte
//  udelay(1);
//  udelay(1);

  for (cur_pos = 0; cur_pos < len; cur_pos++) {
//  udelay(1);
//    spiWriteByte(0x00);
    pBuffer[cur_pos] = spiReadByte();
//    SPI1BUF = 0x00; // Send dummy byte
//    while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//    pBuffer[cur_pos] = SPI1BUF;
//    udelay(1);
  }
//  udelay(1);
//  while(SPI1_Busy);
  SPI_DESEL();
  return 1;
}

static uint16_t spiReadPhyControl(uint8_t address)
{
  uint16_t lower, upper;
  uint8_t micmd;

  spiSelectBank(SPI_BANK2);

  spiWriteControl(MIREGADR, address);

  spiWriteControl(MICMD, MICMD_MIIRD);
  udelay(15);
  spiSelectBank(SPI_BANK3);
  while(spiReadMacMiiControl(MISTAT) & MISTAT_BUSY);

  spiSelectBank(SPI_BANK2);
  spiWriteControl(MICMD, 0x00);

  lower = spiReadMacMiiControl(MIRDL);
  upper = spiReadMacMiiControl(MIRDH);
#if 0
  printf("PHY lower byte: %02x\n", lower);
  printf("PHY upper byte: %02x\n", upper);
#endif
  return lower + (upper << 8);
}

static void spiWritePhyControl(uint8_t address, uint16_t data)
{
  spiSelectBank(SPI_BANK2);
  // Write the phy address
  spiWriteControl(MIREGADR, address);

  // Write the data
  spiWriteControl(MIWRL, LOWER_BYTE(data));
  spiWriteControl(MIWRH, UPPER_BYTE(data));
  udelay(15);
  spiSelectBank(SPI_BANK3);
  while(spiReadMacMiiControl(MISTAT) & MISTAT_BUSY);
}

static uint16_t readBuffer(uint8_t* pBuffer)
{
  uint16_t total_bytes = 0;
  uint16_t packet_status = 0;

  // Set read pointer
  spiSelectBank(SPI_BANK0);
  spiWriteControl(ERDPTL, LOWER_BYTE(next_packet_pointer));
  spiWriteControl(ERDPTH, UPPER_BYTE(next_packet_pointer));

  // Read next pointer
  next_packet_pointer = spiReadBuffer();
  next_packet_pointer |= (spiReadBuffer() << 8);

  // Read packet size
  total_bytes = spiReadBuffer();
  total_bytes |= (spiReadBuffer() << 8);

  // Read packet status
  packet_status = spiReadBuffer();
  packet_status |= (spiReadBuffer() << 8);

  // Read buffer
  spiReadFullBuffer(pBuffer, total_bytes);

  // Set read buffer pointers for next run
  spiWriteControl(ERXRDPTL, LOWER_BYTE(next_packet_pointer));
  spiWriteControl(ERXRDPTH, UPPER_BYTE(next_packet_pointer));

  // Decrement the on-chip Packet Counter
  spiBitSet(SPI_ECON2, ECON2_PKTDEC);

  return total_bytes;
}

static uint16_t writeBuffer(uint8_t* pBuffer, uint16_t size)
{
  uint16_t cur_pos = 0;
//  uint8_t value = 0;

  spiSelectBank(SPI_BANK0);

  // Set write pointer
  spiWriteControl(EWRPTL, LOWER_BYTE(TXstart_init));
  spiWriteControl(EWRPTH, UPPER_BYTE(TXstart_init));

  // Set TX end pointer(include control byte)
  spiWriteControl(ETXNDL, LOWER_BYTE(TXstart_init + size + 1));
  spiWriteControl(ETXNDH, UPPER_BYTE(TXstart_init + size + 1));

  // Send the control byte and payload
  SPI_SEL();
//  udelay(1);
  spiWriteByte((0b011010 & SPI_ADDR_MASK) | SPI_OPCODE_WBM);
  spiWriteByte(0x00); // Control byte
//  SPI1BUF = (0b011010 & SPI_ADDR_MASK) | SPI_OPCODE_WBM; // Write Buffer Mem
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  value = SPI1BUF;
//  udelay(1);
//  udelay(1);
//  SPI1BUF = 0x00; // Send control byte
//  while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//  value = SPI1BUF;
//  udelay(1);

  for (cur_pos = 0; cur_pos < size; cur_pos++) {
//    udelay(1);
    spiWriteByte(pBuffer[cur_pos]);
//    SPI1BUF = pBuffer[cur_pos];
//    while (!SPI1_Rx_Buf_Full); // wait till completion of transmission
//    value = SPI1BUF;
//    udelay(1);
  }
//  while(SPI1_Busy);
  SPI_DESEL();

  // Send the packet to the WWW
  spiBitSet(SPI_ECON1, ECON1_TXRTS);

  // Wait until the packet is transmitted or aborted
  while(spiReadEthControl(SPI_ECON1) & ECON1_TXRTS);

  return 1;
}

static uint8_t spiReadByte(void)
{
    SPI1BUF = 0x00;
    while(SPI1_Busy);
    return SPI1BUF;
}

static void spiWriteByte(uint8_t data)
{
    uint8_t tmp;
    SPI1BUF = data;
    while(SPI1_Busy);
    tmp = SPI1BUF;
}
