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

/* Simple TCP listener server using picoTCP stack 

	To compile this application, first download the picoTCP sources from:
	https://github.com/tass-belgium/picotcp/releases/tag/prpl-v0.1. Then, compile with:
      
	make CROSS_COMPILE=mips-mti-elf- PLATFORM_CFLAGS="-EL -Os -c -Wa,-mvirt -mips32r5 -mtune=m14k \
	-mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer \
	-G 0" DHCP_SERVER=0 SLAACV4=0 TFTP=0 AODV=0 IPV6=0 NAT=0 PING=1 ICMP4=1 DNS_CLIENT=0 MDNS=0 DNS_SD=0 \
	SNTP_CLIENT=0 PPP=0 MCAST=1 MLD=0 IPFILTER=0 ARCH=pic32

	The compiled picoTCP directory tree must be at the same directory level of the prpl-hypervisor, 
	example:
   
	~/hyper
		/prp-hypervisor
		/picotcp
        
	Once the application is compiled and uploaded to the board, you can use telnet or nc (netcat)
	to interact with this demo connecting to the 192.168.0.2 port 80. 
*/


#include <pico_defines.h>
#include <pico_stack.h>
#include <pico_ipv4.h>
#include <pico_tcp.h>
#include <pico_socket.h>

#include <arch.h>
#include <eth.h>
#include <guest_interrupts.h>
#include <hypercalls.h>
#include <platform.h>
#include <libc.h>
#include <eth.h>

#define LISTENING_PORT 80
#define MAX_CONNECTIONS 1
#define ETH_RX_BUF_SIZE 1536

static char msg1[] = "\nWelcome on the prpl-Hypervisor and picoTCP demo. ";
static char msg2[] = "\nWrite a message and press enter: ";
static char msg3[] = "\nEcho message: ";

static char rx_buf[ETH_RX_BUF_SIZE] = {0};

static struct pico_socket *s = NULL;
static struct pico_ip4 my_eth_addr, netmask;

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

	if (!prev_init){
		prev_count = mfc0(CP0_COUNT, 0);
		prev_init = 1;
	}

	/* pico_ms_tick is not 100% accurate this way but at this point it's not required
	* currently there's a 10% accuracy loss(1000ms only produces 900 pico_ms_ticks) */
	if (cur_count >= prev_count + cp0_ms_ticks){
		pico_ms_tick += calculate_ms_passed(prev_count, cur_count);
		prev_count = cur_count;
	}
}


static void cb_tcp(uint16_t ev, struct pico_socket *sock)
{
	int r = 0;
	uint16_t port;
	uint16_t peer;
	int ret = 0;
	int i;
	static struct pico_socket *s_client = NULL;
    
	/* Read messages from the socket */
	if (ev & PICO_SOCK_EV_RD) {
    
		/* Read client message */
		r = pico_socket_read(s_client, rx_buf, ETH_RX_BUF_SIZE);
		if (r < 0)
			printf("\nError while reading from socket!");
 
		printf("\nData received(%d bytes):\n Message: ", r);
		for (i = 0; i < r; i++)
			printf("%c", rx_buf[i]);
		printf("\n");
        
		rx_buf[i] = '\n';
        
		/* Replies to the client. */
		ret = pico_socket_write(s_client, msg3, strlen(msg3));
		if (ret < 0)
			printf("Failed to send message\n");
        
		ret = pico_socket_write(s_client, rx_buf, r);
		if (ret < 0)
			printf("Failed to send message\n");
        
		ret = pico_socket_write(s_client, msg2, strlen(msg2));
		if (ret < 0)
			printf("Failed to send message\n");
	}

	/* Incomming connection */
	if (ev & PICO_SOCK_EV_CONN) {
		s_client = pico_socket_accept(s, &peer, &port);
		printf("Accepted connection\n");

		/* Send wellcome message to the client. */
		ret = pico_socket_write(s_client, msg1, strlen(msg1));
		if (ret < 0)
			printf("Failed to send message\n");

		ret = pico_socket_write(s_client, msg2, strlen(msg2));
		if (ret < 0)
			printf("Failed to send message\n");
	}

	/* process error event, socket error occured */
	if (ev & PICO_SOCK_EV_ERR){
		printf("Socket error!\n");
	}
    
	/* Client closed the connection. */
	if (ev & PICO_SOCK_EV_CLOSE){
		printf("Connection Close!\n");
		pico_socket_close(s_client);
	}
}

int main()
{
	uint8_t mac[6];
	uint32_t timer = 0;
    
	/* Obtain the ethernet MAC address */
	eth_mac(mac);
    
	const char *ipaddr="192.168.0.2";
	uint16_t port_be = 0;
    
	interrupt_register(irq_timer, GUEST_TIMER_INT);
  
	/* Select output serial 2 = UART2, 6 = UART6 */
	serial_select(UART2);

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

	/* picoTCP initialization */
	printf("\nInitializing pico stack\n");
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

	while (1){
		eth_watchdog(&timer, 500);
		/* pooling picoTCP stack */
		pico_stack_tick();
        
	}

	return 0;
}
