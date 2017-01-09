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

#include <arch.h>
#include <eth.h>
#include <guest_interrupts.h>
#include <hypercalls.h>
#include <platform.h>
#include <libc.h>
#include <eth.h>
#include <io.h>



#include <pico_defines.h>
#include <pico_stack.h>
#include <pico_ipv4.h>
#include <pico_tcp.h>
#include <pico_socket.h>
#include <pico_https_server.h>
#include <pico_https_util.h>

#include "SSL_keys.h"
#include "web_files.h"

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

//#include <wolfssl/options.h>

#include <wolfssl/wolfcrypt/settings.h>

#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/memory.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/tfm.h>
#include <wolfssl/wolfcrypt/logging.h>


//#include <wolfssl/ssl.h>

#define LISTENING_PORT 80
#define MAX_CONNECTIONS 1
#define ETH_RX_BUF_SIZE 1536

static uint32_t led1_state, led2_state;
static uint8_t mac_str[32];
static char rx_buf[ETH_RX_BUF_SIZE] = {0};

static struct pico_socket *s = NULL;
static struct pico_ip4 my_eth_addr, netmask;
static struct pico_device *pico_dev_eth;

volatile unsigned int pico_ms_tick = 0;
static uint32_t cp0_ms_ticks = CPU_SPEED/2/1000;

static const char toggledString[] = "Led toggled!";
static uint32_t download_progress = 1u;
static uint32_t length_max = 1u;
static uint8_t http_not_found;

int close(int __fildes ){
	return 0;
}

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


/************************** PRIVATE FUNCTIONS *************************/
const struct web_file * find_web_file(char * filename)
{
    uint16_t i;
    for(i = 0; i < num_web_files; i++)
    {
        if(strcmp(web_files[i].filename, filename) == 0)
        {
            return &web_files[i];
        }
    }
    return NULL;
}

/*** START HTTPS server ***/
#define SIZE 1 * 1024
char http_buffer[SIZE];

void serverWakeup(uint16_t ev, uint16_t conn)
{
    char * body;
    uint32_t read = 0;

    if(ev & EV_HTTPS_CON)
    {
        printf("New connection received\n");
        pico_https_server_accept();

    }

    if(ev & EV_HTTPS_REQ) /* new header received */
    {
        char *resource;
        int method;
        printf("Header request received\n");
        resource = pico_https_getResource(conn);
        if(strcmp(resource, "/") == 0)
        {
            resource = "/index.html";
        }
        method = pico_https_getMethod(conn);
        if(strcmp(resource, "/board_info") == 0)
        {

            pico_https_respond(conn, HTTPS_RESOURCE_FOUND);
            strcpy(http_buffer, "{\"uptime\":");
            pico_itoa(PICO_TIME(), http_buffer + strlen(http_buffer));
			
            strcat(http_buffer, ", \"l1\":\"");
            strcat(http_buffer, led1_state? "on" : "off");

            strcat(http_buffer, "\", \"l2\":\"");
            strcat(http_buffer, led2_state? "on" : "off");
	    
	    strcat(http_buffer, "\"}");

            pico_https_submitData(conn, http_buffer, strlen(http_buffer));
        }
   

        else if(strcmp(resource, "/ip") == 0)
        {
            pico_https_respond(conn, HTTPS_RESOURCE_FOUND);

            struct pico_ipv4_link * link;
            link = pico_ipv4_link_by_dev(pico_dev_eth);
            if (link)
                pico_ipv4_to_string(http_buffer, link->address.addr);
            else
                strcpy(http_buffer, "0.0.0.0");
            pico_https_submitData(conn, http_buffer, strlen(http_buffer));
        }
        else if(strcmp(resource, "/mac") == 0)
        {
            pico_https_respond(conn, HTTPS_RESOURCE_FOUND);
	    strcpy(http_buffer, mac_str);
            pico_https_submitData(conn, http_buffer, strlen(http_buffer));
        }
	else if(strcmp(resource,"/led1") == 0){TOGGLE_LED1; led1_state = !led1_state; pico_https_respond(conn, HTTPS_RESOURCE_FOUND); pico_https_submitData(conn, toggledString, sizeof(toggledString)); }
	else if(strcmp(resource,"/led2") == 0){TOGGLE_LED2; led2_state = !led2_state; pico_https_respond(conn, HTTPS_RESOURCE_FOUND); pico_https_submitData(conn, toggledString, sizeof(toggledString)); }
        else /* search in flash resources */
        {
            struct web_file * web_file;
            web_file = find_web_file(resource + 1);
            if(web_file != NULL)
            {
                uint16_t flags;
                flags = HTTPS_RESOURCE_FOUND | HTTPS_STATIC_RESOURCE;
                if(web_file->cacheable)
                {
                    flags = flags | HTTPS_CACHEABLE_RESOURCE;
                }
                pico_https_respond(conn, flags);
                pico_https_submitData(conn, web_file->content, (int) *web_file->filesize);
            } else { /* not found */
                /* reject */
		printf("Rejected connection...\n");
                pico_https_respond(conn, HTTPS_RESOURCE_NOT_FOUND);
            }

        }
    }


    if(ev & EV_HTTPS_PROGRESS) /* submitted data was sent */
    {
        uint16_t sent, total;
        pico_https_getProgress(conn, &sent, &total);
        printf("Chunk statistics : %d/%d sent\n", sent, total);
    }

    if(ev & EV_HTTPS_SENT) /* submitted data was fully sent */
    {
        printf("Last chunk post !\n");
        pico_https_submitData(conn, NULL, 0); /* send the final chunk */
    }

    if(ev & EV_HTTPS_CLOSE)
    {
        printf("Close request: %p\n", conn);
        if (conn)
            pico_https_close(conn);
        else
            printf(">>>>>>>> Close request w/ conn=NULL!!\n");
    }

    if(ev & EV_HTTPS_ERROR)
    {
        printf("Error on server: %p\n", conn);
        //TODO: what to do?
        //pico_http_close(conn);
    }
}
/* END HTTP server */



int main()
{
	uint8_t mac[6];
	uint32_t timer = 0;
    
	/* Obtain the ethernet MAC address */
	eth_mac(mac);
    
	const char *ipaddr="192.168.0.150";
	uint16_t port_be = 0;
    
	interrupt_register(irq_timer, GUEST_TIMER_INT);
  
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
	
	wolfSSL_Debugging_ON();
    
	pico_string_to_ipv4(ipaddr, &my_eth_addr.addr);
	pico_string_to_ipv4("255.255.255.0", &netmask.addr);
	pico_ipv4_link_add(eth_dev, my_eth_addr, netmask);

	port_be = short_be(LISTENING_PORT);
	
	/* WolfSSL initialization only, to make sure libwolfssl.a is needed */
	pico_https_setCertificate(cert_pem_2048, sizeof(cert_pem_2048));
	pico_https_setPrivateKey(privkey_pem_2048, sizeof(privkey_pem_2048));
	pico_https_server_start(0, serverWakeup);

	
	while (1){
		eth_watchdog(&timer, 500);
		/* pooling picoTCP stack */
		pico_stack_tick();
        
	}

	return 0;
}
