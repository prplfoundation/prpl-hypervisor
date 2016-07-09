/*
Copyright (c) 2016, prpl Foundation

Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
fee is hereby granted, provided that the above copyright notice and this permission notice appear 
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

*/

/* Simple UART Bare-metal application sample */

#include <pic32mz.h>
#include <libc.h>
#include <network.h>

#include "pico_defines.h"
#include "pico_stack.h"
#include "pico_ipv4.h"
#include "pico_tcp.h"
#include "pico_socket.h"

#define LISTENING_PORT 80
#define MAX_CONNECTIONS 1
#define RX_BUF_SIZE 1536

static char rx_buf[RX_BUF_SIZE] = {0};
static struct pico_socket *s = NULL;
static struct pico_ip4 my_eth_addr, netmask, inaddr_any = { };
static struct pico_device *pico_dev_eth;


volatile unsigned int pico_ms_tick = 0;
static uint32_t cp0_ms_ticks = CPU_SPEED/2/1000;

static uint32_t calculate_ms_passed(uint32_t old_ticks, uint32_t new_ticks)
{
    uint32_t diff_ticks;
    if (new_ticks >= old_ticks)
        diff_ticks = new_ticks - old_ticks;
    else
        diff_ticks = 0xffffffff - (old_ticks - new_ticks);

    return diff_ticks / cp0_ms_ticks;
}

void irq_timer()
{
    static int prev_init = 0;
    static uint32_t prev_count = 0;
    uint32_t cur_count = mfc0(CP0_COUNT, 0);

    if (!prev_init)
    {
        prev_count = mfc0(CP0_COUNT, 0);
        prev_init = 1;
    }

    /* pico_ms_tick is not 100% accurate this way but at this point it's not required
     * currently there's a 10% accuracy loss(1000ms only produces 900 pico_ms_ticks) */
    if (cur_count >= prev_count + cp0_ms_ticks)
    {
        pico_ms_tick += calculate_ms_passed(prev_count, cur_count);
        prev_count = cur_count;
    }

}


/* SP1 configuration */
static void setupSPI1()
{
#if 0
    /* Pin settings */

    ANSELBCLR = 0x0008; /* pin B3 used as output for CS */
    TRISBCLR = 0x0008;
    TRISDCLR = 0x0002;  /* pin D1 used as output for SCLK */
    TRISFCLR = 0x0020;  /* pin F5 used as output for MOSI */
    TRISFSET = 0x0010;  /* pin F4 used as input for MISO */
    LATFSET = 0x0010;

    SDI1R = 2;  /* pin F4 as SPI1 data input */
    RPF5R = 5;  /* pin F5 as SPI1 data output */

    /* SPI config settings */
#endif
    SPI1BRG = 4; /* Set clock divider to selected_clock/10: selected_clk/(2*(4+1)) */
    SPI1CON = 0x8120; /* enable SPI / master mode / data transition from high to low clk */

}

static void cb_tcp(uint16_t ev, struct pico_socket *sock)
{
    int r = 0;
    uint16_t port;
    uint16_t peer;
    int ret = 0;

    if (ev & PICO_SOCK_EV_RD) {
        r = pico_socket_read(s, rx_buf, RX_BUF_SIZE);
        if (r < 0)
            printf("Error while reading from socket!\n");
        int i;
        printf("Data received(%d bytes):\n", r);
        for (i = 0; i < r; i++)
            printf("%02x ", rx_buf[i]);
        printf("\n");

        ret = SendMessage()
    }

    if (ev == PICO_SOCK_EV_CONN)
    {
        s = pico_socket_accept(s, &peer, &port);
        ret = pico_socket_wri
    }

    /* process error event, socket error occured */
    if (ev == PICO_SOCK_EV_ERR)
        printf("Socket error!\n");
}

int main()
{
    uint8_t mac[6] = {0x00,0x00,0x00,0x12,0x34,0x56};
    const char *ipaddr="192.168.43.42";
    uint16_t port_be = 0;

    /* Select output serial 2 = UART2, 6 = UART6 */
    serial_select(UART2);
#if 0
    printf("Configuring SPI1\n");

    setupSPI1();

    printf("Configured SPI1\n");
#endif
    printf("Initializing pico stack\n");
    pico_stack_init();

    printf("Creating ethernet device\n");
    pico_dev_eth = (struct pico_device *) pico_eth_create("eth", mac);
    if (!pico_dev_eth)
        printf("Failed to create ethernet device!\n");

    pico_string_to_ipv4(ipaddr, &my_eth_addr.addr);
    pico_string_to_ipv4("255.255.255.0", &netmask.addr);
    pico_ipv4_link_add(pico_dev_eth, my_eth_addr, netmask);

    port_be = short_be(LISTENING_PORT);

    printf("Opening socket\n");
    s = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP, &cb_tcp);
    if (!s)
        printf("Failed to open socket!\n");

    if (pico_socket_bind(s, &my_eth_addr.addr, &port_be) != 0)
        printf("Failed to bind socket!\n");

    if (pico_socket_listen(s, MAX_CONNECTIONS) != 0)
        printf("Failed to listen on socket!\n");

    while (1)
    {
	    pico_stack_tick();
    }

    return 0;
}
