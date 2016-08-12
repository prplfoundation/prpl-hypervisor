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
#include <puf.h>
#include <eth.h>

#include "pico_defines.h"
#include "pico_stack.h"
#include "pico_ipv4.h"
#include "pico_tcp.h"
#include "pico_socket.h"

#define LISTENING_PORT 80
#define MAX_CONNECTIONS 1
#define ETH_RX_BUF_SIZE 1536

#define KEYSIZE 16

static char tx_buf[ETH_RX_BUF_SIZE] = {0};
static char rx_buf[ETH_RX_BUF_SIZE] = {0};
static struct pico_socket *s = NULL;
static struct pico_ip4 my_eth_addr, netmask;
static struct pico_device *pico_dev_eth;

uint16_t keySize = KEYSIZE;
uint8_t key[KEYSIZE] = { // This is the key that has to be wrapped using PUF
							 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
							 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
						};

uint8_t key_unwrapped[KEYSIZE]; // This will contain the unwrapped key
uint8_t label[LABEL_SIZE] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
uint8_t context[CONTEXT_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
uint16_t keyProperties = KEY_PROP_NONE;
uint8_t keyIndex = 0;
uint8_t keyCode[KEYSIZE + KEYCODE_OVERHEAD]; // This will contain the wrapped key
uint8_t key_wrapped = 0;
return_t retVal;


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
    int i,j;
    char c;

    if (ev & PICO_SOCK_EV_RD) {

        r = pico_socket_read(s, rx_buf, ETH_RX_BUF_SIZE);
        if (r < 0)
            printf("Error while reading from socket!\n");
 
        printf("Data received(%d bytes):\n", r);
        for (i = 0; i < r; i++)
            printf("%02x ", rx_buf[i]);
        printf("\n");

	// ---------------------------------------------------------------------------------------------------
	// This code is only needed to demo the tcp listener via telnet by sending the key in a HH notation
	// and the arm command as a '1' or '2' ascii. Remove if you send key+command in binary. 
	// ---------------------------------------------------------------------------------------------------
	char key_buf[KEYSIZE + KEYCODE_OVERHEAD];
	
	for (i=0,j=0; i < r-1; i+=2,j+=1) {
                char hn = rx_buf[i] >='0' && rx_buf[i] <='9' ? rx_buf[i] - '0' : rx_buf[i] - 'a' +10;
                char ln = rx_buf[i+1] >='0' && rx_buf[i+1] <='9' ? rx_buf[i+1] - '0' : rx_buf[i+1] - 'a' +10;
                key_buf[j] = (hn << 4 ) | ln;
        }

	// ---------------------------------------------------------------------------------------------------	

	//retVal = PUF_UnwrapKey(rx_buf, label, context, &keySize, &keyIndex, key_unwrapped);
	retVal = PUF_UnwrapKey(key_buf, label, context, &keySize, &keyIndex, key_unwrapped);

	if (IID_PRPL_SUCCESS != retVal) {
		printf("Invalid key!\n");
	} else {
		printf("Valid key! Command relayed to robotic arm controller.\n");
		if (rx_buf[r-3]=='1' || rx_buf[r-3]=='2'){
			SendMessage(3, &rx_buf[r-3], 1);
		}
	}

    }

    if (ev == PICO_SOCK_EV_CONN)
    {
        s = pico_socket_accept(s, &peer, &port);
        printf("Accepted connection\n");
	    if (key_wrapped == 0)
	    printf("Key not ready yet\n");
	    else
	    {

	// ---------------------------------------------------------------------------------------------------
	// This code is only needed to demo the tcp listener via telnet by sending the key in a HH notation
	// Remove if you send key in binary
	// ---------------------------------------------------------------------------------------------------
	char keyAscii[ (KEYSIZE+KEYCODE_OVERHEAD)*2];

	for (i=0,j=0; i < KEYSIZE + KEYCODE_OVERHEAD; i++) {

		c = keyCode[i] >>4;
		c = (c >=0 && c <=9 ? c + '0' : c-10 + 'a');
		keyAscii[j++] = c;

		c = keyCode[i] & 0x0F;
		c = (c >=0 && c <=9 ? c + '0' : c-10 + 'a');
		keyAscii[j++] = c;
		
	}

	// ---------------------------------------------------------------------------------------------------

	    //ret = pico_socket_write(s, keyCode, KEYSIZE + KEYCODE_OVERHEAD);
            ret = pico_socket_write(s, keyAscii, j);
	        if (ret < 0)
	            printf("Failed to send wrapped key\n");
            else
                printf("Sent wrapped key\n");
	    }
    }

    /* process error event, socket error occured */
    if (ev == PICO_SOCK_EV_ERR)
        printf("Socket error!\n");
}

int main()
{
    uint8_t mac[6];
    
    eth_get_mac(mac);
    
    const char *ipaddr="192.168.0.2";
    uint16_t port_be = 0;
    int i = 0;
  
    /* Select output serial 2 = UART2, 6 = UART6 */
    serial_select(UART2);

    init_network();
    
    /* Configure the virtual ethernet driver */
    struct pico_device* eth_dev = PICO_ZALLOC(sizeof(struct pico_device));
    if(!eth_dev) {
        return 0;
    }   
    
    eth_dev->send = eth_send;
    eth_dev->poll = eth_poll;
    eth_dev->link_state = eth_link_state;
    
    if( 0 != pico_device_init((struct pico_device *)eth_dev, "virt-eth", mac)) {
        printf ("\nDevice init failed.");
        PICO_FREE(eth_dev);
        return 0;
    }    
    
    printf("\nInitializing pico stack");
    pico_stack_init();
    
    pico_string_to_ipv4(ipaddr, &my_eth_addr.addr);
    pico_string_to_ipv4("255.255.255.0", &netmask.addr);
    pico_ipv4_link_add(eth_dev, my_eth_addr, netmask);

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

	    /* Only wrap key when VM 2 is up and only do it once */
        
	    if (guest_is_up(2) != MESSAGE_VCPU_NOT_INIT && key_wrapped == 0)
	    {
            retVal = PUF_WrapKey(key, label, context, keySize, keyProperties, keyIndex, keyCode);

            if (IID_PRPL_SUCCESS != retVal) {
                printf("Error PUF_WrapKey: %x\n", retVal);
            } else {
                printf("keyCode: ");
                for (i = 0; i < KEYSIZE + KEYCODE_OVERHEAD; ++i) {
                    printf("%02x", keyCode[i]);
                }
                printf("\n");

            }

            key_wrapped = 1;
	    }
    }

    return 0;
}
