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


/*************************************************************
 * Tester VM for arm control
 * Example:
 *      APP_LIST=  arm-control-tester arm-control
 * */


#include <pic32mz.h>
#include <libc.h>
#include <network.h>

volatile int32_t t2 = 0;

void irq_timer(){
 t2++;
}

int main() {
	uint8_t buffer[1];

    serial_select(UART2);

    init_network();

    printf("starting vm id: %d\n\r", hyp_get_guest_id());

    while (1) {

    	buffer[0] = getchar();

    	printf("arm control tester sending command: %d\n\r", buffer[0]);

		SendMessage(3, buffer, sizeof(buffer));
    }

    return 0;
}

