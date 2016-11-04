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
#include <arch.h>
#include <libc.h>
#include <usb.h>
#include <guest_interrupts.h>
#include <hypercalls.h>
#include <network.h>
#include <platform.h>
#include <eth.h>


#include <pico_defines.h>
#include <pico_stack.h>
#include <pico_ipv4.h>
#include <pico_tcp.h>
#include <pico_socket.h>
#include "../iidprpl/include/puf.h"

#define LISTENING_PORT 80
#define MAX_CONNECTIONS 1
#define ETH_RX_BUF_SIZE 1536

#define KEYSIZE 16

#define LABEL_SIZE  6

static char msg3[] = "\nInvalid Key ";
static char msg4[] = "\nValid key! Command relayed to robotic arm controller.";
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

static uint32_t calculate_ms_passed(uint32_t old_ticks, uint32_t new_ticks){
	uint32_t diff_ticks;
	
	if (new_ticks >= old_ticks){
		diff_ticks = new_ticks - old_ticks;
	}else{
		diff_ticks = 0xffffffff - (old_ticks - new_ticks);
	}

	return diff_ticks / cp0_ms_ticks;
}

void irq_timer()
{
	static int prev_init = 0;
	static uint32_t prev_count = 0;
	uint32_t cur_count = mfc0(CP0_COUNT, 0);

	if (!prev_init)	{
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

static void cb_tcp(uint16_t ev, struct pico_socket *sock){
	int r = 0;
	uint16_t port;
	struct pico_ip4 peer;
	int ret = 0;
	int i,j;
	char c;
	static int send_key_once = 0;
	static struct pico_socket *s_client = NULL;
	char str_ip_peer[200];

	if (ev & PICO_SOCK_EV_RD) {
		printf("\nVM#1: Receiving client data.");
        
		r = pico_socket_read(s_client, rx_buf, ETH_RX_BUF_SIZE);
		if (r < 0){
			printf("VM#1: Error while reading from socket!\n");
		}
 
		printf("\nVM#1: Data received(%d bytes):\n", r);
		for (i = 0; i < r; i++){
			if(i%10==0) printf("\n");
				printf("%02x ", rx_buf[i]);
		}
		
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

		printf("VM#1: Verifying received key.\n");
		retVal = PUF_UnwrapKey(key_buf, label, context, &keySize, &keyIndex, key_unwrapped);

		if (IID_PRPL_SUCCESS != retVal) {
			printf(msg3);
		} else {
			printf(msg4);
		}
		
		if (rx_buf[r-3]=='1' || rx_buf[r-3]=='2'){
			printf("\nVM#1: Command %c.", rx_buf[r-3]);
			SendMessage(3, &rx_buf[r-3], 1);
		}
        }

	if (ev & PICO_SOCK_EV_CONN){
		s_client = pico_socket_accept(s, &peer, &port);
		pico_ipv4_to_string(str_ip_peer, peer.addr);
		
		printf("\nVM#1: Accepted connection from %s:%d", str_ip_peer, port);
		
		if (key_wrapped == 0){
			printf("\nVM#1: Key not ready yet");
		}else{

			// ---------------------------------------------------------------------------------------------------
			// This code is only needed to demo the tcp listener via telnet by sending the key in a HH notation
			// Remove if you send key in binary
			// ---------------------------------------------------------------------------------------------------
			printf("\nVM#1: Sending wrapped key to the client %s:%d", str_ip_peer, port);
        
			if (send_key_once == 0){
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
				ret = pico_socket_write(s_client, keyAscii, j);
				if (ret < 0){
					printf("\nVM#1: Failed to send wrapped key");
				}else{
					printf("\nVM#1: Waiting for client message.");
				}
		
				send_key_once = 1;	
			}
		}

		/* process error event, socket error occured */
		if (ev & PICO_SOCK_EV_ERR){
			printf("VM#1: Socket error!\n");
		}
    
		if (ev & PICO_SOCK_EV_CLOSE){
			pico_socket_close(s_client);
			printf("\nVM#1: Client connection close.");
		}
	}
}

int main(){
	uint8_t mac[6];
	const char *ipaddr="192.168.0.2";
	uint16_t port_be = 0;
	int i = 0;
	
	/* Get the Ethernet MAC address. */
	eth_mac(mac);
	
	/* Select output serial 2 = UART2, 6 = UART6 */
	serial_select(UART2);

	/* Configure the virtual ethernet driver */
	struct pico_device* eth_dev = PICO_ZALLOC(sizeof(struct pico_device));
	if(!eth_dev) {
		printf("\nVM#1: Error on allocating pico_device structure.");
		return 0;
	}   
    
	eth_dev->send = eth_send;
	eth_dev->poll = eth_poll;
	eth_dev->link_state = eth_link_state;
    
	if( 0 != pico_device_init((struct pico_device *)eth_dev, "virt-eth", mac)) {
		printf ("\nVM#1: Device init failed.");
		PICO_FREE(eth_dev);
		return 0;
	}    
    
	printf("\nVM#1: Initializing pico stack");
	pico_stack_init();
    
	pico_string_to_ipv4(ipaddr, &my_eth_addr.addr);
	pico_string_to_ipv4("255.255.255.0", &netmask.addr);
	pico_ipv4_link_add(eth_dev, my_eth_addr, netmask);

	port_be = short_be(LISTENING_PORT);

	printf("\nVM#1: Opening socket");
	s = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP, &cb_tcp);
	if (!s){
		printf("\nVM#1: Failed to open socket!");
	}

	if (pico_socket_bind(s, &my_eth_addr.addr, &port_be) != 0){
		printf("\nVM#1: Failed to bind socket!");
	}

	if (pico_socket_listen(s, MAX_CONNECTIONS) != 0){
		printf("\nVM#1: Failed to listen on socket!");
	}
    
	printf("\nVM#1: TCP server waiting for incoming connections on %s:%d\n", ipaddr, LISTENING_PORT);

	while (1){
        
		pico_stack_tick();

		/* Only wrap key when VM 2 is up and only do it once */
        
		if (key_wrapped == 0){
			retVal = PUF_WrapKey(key, label, context, keySize, keyProperties, keyIndex, keyCode);

			if (IID_PRPL_SUCCESS != retVal) {
				printf("VM#1: Error PUF_WrapKey: %x\n", retVal);
			} else {
				printf("VM#1: keyCode: ");
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
