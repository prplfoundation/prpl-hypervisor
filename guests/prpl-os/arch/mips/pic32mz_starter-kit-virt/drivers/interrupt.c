#include <os.h>

static funcptr isr[32] = {[0 ... 31] = NULL};

/*
interrupt management routines
*/
void _irq_register(uint32_t mask, funcptr ptr)
{
	int32_t i;

	for (i = 0; i < 32; ++i)
		if (mask & (1 << i))
			isr[i] = ptr;
}

void _irq_handler(uint32_t status, uint32_t cause)
{
	//int32_t i = 0;
	//uint32_t irq;
	/*
	irq = IFS(0);
	
	do {
		if (irq & 0x1){
			if(isr[i]){
				isr[i]();
			}
		}
		irq >>= 1;
		++i;
	} while(irq);*/
    dispatch_isr(0);
}

void _irq_mask_set(uint32_t mask)
{
	/*IECSET(0) = mask;*/
}

void _irq_mask_clr(uint32_t mask)
{
	/*IECCLR(0) = mask;*/
}

void _exception_handler(uint32_t epc, uint32_t opcode)
{
}
