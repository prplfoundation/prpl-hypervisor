#include <os.h>

#if 0
PIC32MZ_DEVCFG (
    _DEVCFG0_JTAG_DISABLE |      /* Disable JTAG port */
    _DEVCFG0_TRC_DISABLE,        /* Disable trace port */
    _DEVCFG1_FNOSC_SPLL |        /* System clock supplied by SPLL */
    _DEVCFG1_POSCMOD_DISABLE |   /* Primary oscillator disabled */
    _DEVCFG1_CLKO_DISABLE,       /* CLKO output disable */

    _DEVCFG2_FPLLIDIV_1 |        /* PLL input divider = 1 */
    _DEVCFG2_FPLLRNG_5_10 |      /* PLL input range is 5-10 MHz */
    _DEVCFG2_FPLLICLK_FRC |      /* Select FRC as input to PLL */
    _DEVCFG2_FPLLMULT(50) |      /* PLL multiplier = 50x */
    _DEVCFG2_FPLLODIV_2,         /* PLL postscaler = 1/2 */
    _DEVCFG3_FETHIO |            /* Default Ethernet pins */
    _DEVCFG3_USERID(0xffff));    /* User-defined ID */
#endif 

/* hardware dependent C library stuff */
void putchar(int32_t value)
{
	while (U2STA & USTA_UTXBF);
	U2TXREG = value;	
}

int32_t kbhit(void)
{
        return (U2STA & USTA_URXDA);
}

int32_t getchar(void)
{
	while (!kbhit());
	return (uint8_t)U2RXREG;
}

/* hardware platform dependent stuff */
void delay_ms(uint32_t msec)
{
	uint32_t now = mfc0(CP0_COUNT, 0);
	uint32_t final = now + msec * (CPU_SPEED / 1000) / 2;

	for (;;){
		now = mfc0(CP0_COUNT, 0);
		if ((int32_t) (now - final) >= 0) break;
	}
}

void delay_us(uint32_t usec)
{
	uint32_t now = mfc0(CP0_COUNT, 0);
	uint32_t final = now + usec * (CPU_SPEED / 1000000) / 2;

	for (;;){
		now = mfc0(CP0_COUNT, 0);
		if ((int32_t) (now - final) >= 0) break;
	}
}

void uart_init(uint32_t baud)
{
	/* Initialize UART. */
#if 0    
	U2BRG = BRG_BAUD (CPU_SPEED / 2, baud);
	U2STA = 0;
	U2MODE = UMODE_PDSEL_8NPAR |		/* 8-bit data, no parity */
		UMODE_ON;			/* UART Enable */
	U2STASET = USTA_URXEN |	USTA_UTXEN;	/* RX / TX Enable */
#endif 	
}

/* hardware dependent basic kernel stuff */
void _hardware_init(void)
{
	uint32_t temp_CP0;
	
	/* configure board registers (clock source, multiplier...) */
/*	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	
	SYSKEY = 0x33333333;
	while (OSCCON & 0x1);
*/	
	uart_init(TERM_BAUD);
	
	/* configure the interrupt controller to compatibility mode */
	asm volatile("di");			/* Disable all interrupts */
	mtc0 (CP0_EBASE, 1, 0x9d000000);	/* Set an EBase value of 0x9D000000 */
	temp_CP0 = mfc0(CP0_CAUSE, 0);		/* Get Cause */
	temp_CP0 |= CAUSE_IV;			/* Set Cause IV */
	mtc0(CP0_CAUSE, 0, temp_CP0);		/* Update Cause */
	/*INTCONCLR = INTCON_MVEC;*/		/* Clear the MVEC bit */
	temp_CP0 = mfc0(CP0_STATUS, 0);		/* Get Status */
	temp_CP0 &= ~STATUS_BEV;		/* Clear Status IV */
	mtc0(CP0_STATUS, 0, temp_CP0);		/* Update Status */

    temp_CP0 = mfc0(12,1); /* intCTL IV must be different of 0 to allow EIC mode. */
    temp_CP0 |= 8<<5;
    mtc0(12, 1, temp_CP0);
    
}

void _vm_init(void)
{
	kprintf("\nHAL: _vm_init()");
	heapinit(krnl_heap, sizeof(krnl_heap));
}

void _sched_init(void)
{
	kprintf("\nHAL: _sched_init()");
}

void _timer_init(void)
{
	kprintf("\nHAL: _timer_init()");
#if TIME_SLICE == 0
#if 0    
	_irq_register(0x00000200, dispatch_isr);
	/* setup timer2 - check PIC32MZ datasheet, page 118, section 7.2 */
	T2CON = 0x0;
	TMR2 = 0x0;
	PR2 = 0xffff;
	IPCSET(2) = 0x00001f00;
	IFSCLR(0) = 0x00000200;
	IECSET(0) = 0x00000200;
	T2CON |= 0x8000;
#endif    
#else
	kprintf("\nHAL: timer not set!");
#endif
}

void _irq_init(void)
{
	kprintf("\nHAL: _irq_init()");
}

void _device_init(void)
{
	kprintf("\nHAL: _device_init()");
	// ETHERNET!
}

void _task_init(void)
{
	kprintf("\nHAL: _task_init()");
	// network service
}

void _set_task_sp(uint16_t task, uint32_t stack)
{
	krnl_tcb[task].task_context[10] = stack;
}

uint32_t _get_task_sp(uint16_t task)
{
	return krnl_tcb[task].task_context[10];
}

void _set_task_tp(uint16_t task, void (*entry)())
{
	krnl_tcb[task].task_context[11] = (uint32_t)entry;
}

void *_get_task_tp(uint16_t task)
{
	return (void *)krnl_tcb[task].task_context[11];
}

void _timer_reset(void)
{
#if TIME_SLICE == 0
//	IFSCLR(0) = 0x00000200;
#else
	kprintf("\nHAL: timer not set!");
#endif
}

void _cpu_idle(void)
{
}

uint32_t _readcounter(void)
{
	return mfc0(9, 0);
}

