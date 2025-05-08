#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sam.h"

/* Linker symbols */
extern uint32_t __data_text[];
extern uint32_t __data_start[];
extern uint32_t __data_end[];
extern uint32_t __bss_start[];
extern uint32_t __bss_end[];
extern uint32_t __stack[];

/* Default empty handler */
void handler_none(void) { exit(1); }

/* Cortex-M0+ core handlers */
void handler_reset         (void);
void handler_NMI           (void) __attribute__ ((weak, alias("handler_none")));
void handler_hardfault     (void);
void handler_SVC           (void) __attribute__ ((weak, alias("handler_none")));
void handler_PendSV        (void) __attribute__ ((weak, alias("handler_none")));
void handler_systick       (void) __attribute__ ((weak, alias("handler_none")));

/* Peripherals handlers */
void handler_PM            (void) __attribute__ ((weak, alias("handler_none")));
void handler_SYSCTRL       (void) __attribute__ ((weak, alias("handler_none")));
void handler_WDT           (void) __attribute__ ((weak, alias("handler_none")));
void handler_RTC           (void) __attribute__ ((weak, alias("handler_none")));
void handler_EIC           (void) __attribute__ ((weak, alias("handler_none")));
void handler_NVMCTRL       (void) __attribute__ ((weak, alias("handler_none")));
void handler_DMAC          (void) __attribute__ ((weak, alias("handler_none")));
void handler_USB           (void) __attribute__ ((weak, alias("handler_none")));
void handler_EVSYS         (void) __attribute__ ((weak, alias("handler_none")));
void handler_SERCOM0       (void) __attribute__ ((weak, alias("handler_none")));
void handler_SERCOM1       (void) __attribute__ ((weak, alias("handler_none")));
void handler_SERCOM2       (void) __attribute__ ((weak, alias("handler_none")));
void handler_SERCOM3       (void) __attribute__ ((weak, alias("handler_none")));
void handler_SERCOM4       (void) __attribute__ ((weak, alias("handler_none")));
void handler_SERCOM5       (void) __attribute__ ((weak, alias("handler_none")));
void handler_TCC0          (void) __attribute__ ((weak, alias("handler_none")));
void handler_TCC1          (void) __attribute__ ((weak, alias("handler_none")));
void handler_TCC2          (void) __attribute__ ((weak, alias("handler_none")));
void handler_TC3           (void) __attribute__ ((weak, alias("handler_none")));
void handler_TC4           (void) __attribute__ ((weak, alias("handler_none")));
void handler_TC5           (void) __attribute__ ((weak, alias("handler_none")));
void handler_ADC           (void) __attribute__ ((weak, alias("handler_none")));
void handler_AC            (void) __attribute__ ((weak, alias("handler_none")));
void handler_DAC           (void) __attribute__ ((weak, alias("handler_none")));
void handler_PTC           (void) __attribute__ ((weak, alias("handler_none")));
void handler_I2S           (void) __attribute__ ((weak, alias("handler_none")));

__attribute__((used, section(".vectors")))
const DeviceVectors handler_vector = {

	/* Configure Initial Stack Pointer, using linker-generated symbols */
	.pvStack               = __stack,
	.pfnReset_Handler      = handler_reset,
	.pfnNMI_Handler        = handler_NMI,
	.pfnHardFault_Handler  = handler_hardfault,
	.pvReservedM12         = NULL,
	.pvReservedM11         = NULL,
	.pvReservedM10         = NULL,
	.pvReservedM9          = NULL,
	.pvReservedM8          = NULL,
	.pvReservedM7          = NULL,
	.pvReservedM6          = NULL,
	.pfnSVC_Handler        = handler_SVC,
	.pvReservedM4          = NULL,
	.pvReservedM3          = NULL,
	.pfnPendSV_Handler     = handler_PendSV,
	.pfnSysTick_Handler    = handler_systick,

	/* Configurable interrupts */
	.pfnPM_Handler         = handler_PM,       //  0 Power Manager
	.pfnSYSCTRL_Handler    = handler_SYSCTRL,  //  1 System Control
	.pfnWDT_Handler        = handler_WDT,      //  2 Watchdog Timer
	.pfnRTC_Handler        = handler_RTC,      //  3 Real-Time Counter
	.pfnEIC_Handler        = handler_EIC,      //  4 External Interrupt Controller
	.pfnNVMCTRL_Handler    = handler_NVMCTRL,  //  5 Non-Volatile Memory Controller
	.pfnDMAC_Handler       = handler_DMAC,     //  6 Direct Memory Access Controller
	.pfnUSB_Handler        = handler_USB,      //  7 Universal Serial Bus
	.pfnEVSYS_Handler      = handler_EVSYS,    //  8 Event System Interface
	.pfnSERCOM0_Handler    = handler_SERCOM0,  //  9 Serial Communication Interface 0
	.pfnSERCOM1_Handler    = handler_SERCOM1,  // 10 Serial Communication Interface 1
	.pfnSERCOM2_Handler    = handler_SERCOM2,  // 11 Serial Communication Interface 2
	.pfnSERCOM3_Handler    = handler_SERCOM3,  // 12 Serial Communication Interface 3
	.pfnSERCOM4_Handler    = handler_SERCOM4,  // 13 Serial Communication Interface 4
	.pfnSERCOM5_Handler    = handler_SERCOM5,  // 14 Serial Communication Interface 5
	.pfnTCC0_Handler       = handler_TCC0,     // 15 Timer Counter Control 0
	.pfnTCC1_Handler       = handler_TCC1,     // 16 Timer Counter Control 1
	.pfnTCC2_Handler       = handler_TCC2,     // 17 Timer Counter Control 2
	.pfnTC3_Handler        = handler_TC3,      // 18 Basic Timer Counter 0
	.pfnTC4_Handler        = handler_TC4,      // 19 Basic Timer Counter 1
	.pfnTC5_Handler        = handler_TC5,      // 20 Basic Timer Counter 2
	.pvReserved21          = NULL,             // 21 Reserved
	.pvReserved22          = NULL,             // 22 Reserved
	.pfnADC_Handler        = handler_ADC,      // 23 Analog Digital Converter
	.pfnAC_Handler         = handler_AC,       // 24 Analog Comparators 0
	.pfnDAC_Handler        = handler_DAC,      // 25 Digital Analog Converter
	.pfnPTC_Handler        = handler_PTC,      // 26 Peripheral Touch Controller
	.pfnI2S_Handler        = handler_I2S,      // 27 Inter-IC Sound Interface
	.pvReserved28          = NULL,             // 28 Reserved
};

__attribute__ ((__optimize__("-fno-tree-loop-distribute-patterns")))
void handler_reset(void)
{
	/* Set the vector table base address */
	SCB->VTOR = (uintptr_t) & handler_vector;

	/* MANW default value 0 can lead to spurious writes to NVM (errata 1.11.2) */
	NVMCTRL->CTRLB.bit.MANW = 1;

	/* brown-out detector */
	SYSCTRL->BOD33.bit.ENABLE = 0;
	// minimum 3.0v - 3.2v (datasheet 37.11.3 table 37-21)
	REG_SYSCTRL_BOD33 = SYSCTRL_BOD33_LEVEL(48) | SYSCTRL_BOD33_HYST | SYSCTRL_BOD33_ACTION_NONE;
	// enable and wait for the voltage level to settle
	SYSCTRL->BOD33.bit.ENABLE = 1;
	while (!SYSCTRL->PCLKSR.bit.BOD33RDY);
	while ( SYSCTRL->PCLKSR.bit.BOD33DET);
	// reset the microcontroller on voltage drops
	SYSCTRL->BOD33.bit.ENABLE = 0;
	REG_SYSCTRL_BOD33 |= SYSCTRL_BOD33_ACTION_RESET;
	SYSCTRL->BOD33.bit.ENABLE = 1;

	/* Set CPU and APB clock dividers to 1 */
	PM->CPUSEL.bit.CPUDIV = PM_CPUSEL_CPUDIV_DIV1;
	PM->APBASEL.bit.APBADIV = PM_APBASEL_APBADIV_DIV1_Val;
	PM->APBBSEL.bit.APBBDIV = PM_APBBSEL_APBBDIV_DIV1_Val;
	PM->APBCSEL.bit.APBCDIV = PM_APBCSEL_APBCDIV_DIV1_Val;

	/* Set led pins as output */
	REG_PORT_DIRSET0 = PORT_PA17 | PORT_PA18 | PORT_PA19;
	REG_PORT_OUTSET0 = PORT_PA17 | PORT_PA18 | PORT_PA19;
//	REG_PORT_OUTSET0 = PORT_PA17; // HGH LED orange 0N
//	REG_PORT_OUTSET0 = PORT_PA18; // HGH LED blue R 0N
//	REG_PORT_OUTSET0 = PORT_PA19; // HGH LED blue T 0N

	/* Set CPU frequency to 48MHz */

	// DFLL clock must be requested before being configured (errata 1.2.1)
	SYSCTRL->DFLLCTRL.bit.ONDEMAND = 0;
	while (SYSCTRL->PCLKSR.bit.DFLLRDY == 0);

	// calibrate DFLL using values from NVM (datasheet 10.3.2)
	uint32_t coarse = ((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR)&FUSES_DFLL48M_COARSE_CAL_Msk)>>FUSES_DFLL48M_COARSE_CAL_Pos;
	uint32_t fine = ((*(uint32_t*)FUSES_DFLL48M_FINE_CAL_ADDR)&FUSES_DFLL48M_FINE_CAL_Msk)>>FUSES_DFLL48M_FINE_CAL_Pos;
	REG_SYSCTRL_DFLLVAL = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);

	// sync with USB start-of-frame thus 48MHz / 1kHz and set finestep (datasheet 41.14)
	REG_SYSCTRL_DFLLMUL = SYSCTRL_DFLLMUL_MUL(48000) | SYSCTRL_DFLLMUL_FSTEP(0xA);
	// start DFLL using USB Clock Recovery Mode (datasheet 17.6.7.2.2)
	REG_SYSCTRL_DFLLCTRL = SYSCTRL_DFLLCTRL_CCDIS | SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_ENABLE;
	while (SYSCTRL->PCLKSR.bit.DFLLRDY == 0);

	// set number of wait states for 48MHz @ 3.3V (datasheet 37.12)
	NVMCTRL->CTRLB.bit.RWS = 1;

	// switch to DFLL48M as source for GCLK0
	REG_GCLK_GENCTRL = GCLK_GENCTRL_ID(0) | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_IDC;
	while (GCLK->STATUS.bit.SYNCBUSY) {}

	/* Quality of Service for SRAM access */

	// set CPU to medium (datasheet 11.4.3)
	SBMATRIX->SFR[SBMATRIX_SLAVE_HMCRAMC0].reg = 2;
	// set DMA to medium (datasheet 20.8.7)
	REG_DMAC_QOSCTRL = DMAC_QOSCTRL_DQOS_MEDIUM | DMAC_QOSCTRL_FQOS_MEDIUM | DMAC_QOSCTRL_WRBQOS_MEDIUM;
	// set USB to high in tinyusb dcd_samd.c

	/* Initialize USB controller */

	// enable peripheral and route clock
	PM->AHBMASK.reg |= PM_AHBMASK_USB;
	PM->APBBMASK.reg |= PM_APBBMASK_USB;
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_USB_Val) |
		GCLK_CLKCTRL_GEN(GCLK_CLKCTRL_GEN_GCLK0_Val) | GCLK_CLKCTRL_CLKEN;
	// configure pin multiplexer
	REG_PORT_WRCONFIG0 =
		PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_WRPMUX |
		PORT_WRCONFIG_PMUX(MUX_PA24G_USB_DM) | PORT_WRCONFIG_PMUXEN | PORT_PA24G_USB_DM>>16 | PORT_PA25G_USB_DP>>16;

	/* Disable the System Timer */
	SysTick->CTRL = 0u;
	SysTick->VAL  = 0u;

	/* Initialize OSCULP32K at 1kHz */
	REG_GCLK_GENDIV = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(4);
	REG_GCLK_GENCTRL = GCLK_GENCTRL_ID(1) | GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_GENEN;

	/* Initialize Watchdog */
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_WDT | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1;
	// bootload after half a second
	REG_WDT_EWCTRL = WDT_EWCTRL_EWOFFSET_512;
	REG_WDT_INTENSET = WDT_INTENSET_EW;
	NVIC_EnableIRQ(WDT_IRQn);
	// reset after one second
	REG_WDT_CONFIG = WDT_CONFIG_PER_1K;
	// clear and enable
	REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
	REG_WDT_CTRL = WDT_CTRL_ENABLE;

	/* Initialize Real-Time Clock */
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_RTC | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1;
	PM->APBAMASK.reg |= PM_APBAMASK_RTC;
	RTC->MODE2.CTRL.bit.ENABLE = 0;
	REG_RTC_MODE2_EVCTRL = RTC_MODE2_EVCTRL_PEREO0; // event every 8ms
	REG_RTC_MODE2_CTRL = RTC_MODE2_CTRL_MODE_COUNT32 | RTC_MODE2_CTRL_PRESCALER_DIV1024 | RTC_MODE2_CTRL_ENABLE;

	/* Initialize Counter capturing Cycles */
	PM->APBCMASK.reg |= PM_APBCMASK_TC4 | PM_APBCMASK_EVSYS;
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_TC4_TC5 | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0;
	// configure timer counter
	REG_TC4_EVCTRL = TC_EVCTRL_TCEI;
	REG_TC4_CTRLC = TC_CTRLC_CPTEN0;
	REG_TC4_CTRLA = TC_CTRLA_MODE_COUNT32 | TC_CTRLA_ENABLE;
	// event system captures every 8ms
	REG_EVSYS_USER = EVSYS_USER_CHANNEL(2+1) | EVSYS_USER_USER(EVSYS_ID_USER_TC4_EVU);
	REG_EVSYS_CHANNEL = EVSYS_CHANNEL_CHANNEL(2) | EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_RTC_PER_0) | EVSYS_CHANNEL_PATH_ASYNCHRONOUS;
	// request continuous read of COUNT32 reg
	REG_TC4_READREQ = TC_READREQ_RREQ | TC_READREQ_RCONT | TC_READREQ_ADDR(0x10);

	/* Initialize C runtime */

	uint32_t *dst, *src = __data_text;
	// relocate the DATA segment and zero the BSS
	dst = __data_start; while (dst < __data_end) *dst++ = *src++;
	dst = __bss_start;  while (dst < __bss_end)  *dst++ = 0x00000000;

//	memcpy(__data_start, __data_text, (uint8_t*) __data_end - (uint8_t*) __data_start);
//	memset(__bss_start, 0x00, (uint8_t*) __bss_end - (uint8_t*) __bss_start);

	extern void (*__preinit_array []) (void);
	extern size_t __preinit_count [];
	// execute internal cpp constructors
	for (size_t n = 0; n < (size_t) __preinit_count; n++) {
		void(*fn)(void) = __preinit_array[n]; if (fn) fn();
	}

	extern void (*__init_array []) (void);
	extern size_t __init_count [];
	// execute active constructors
	for (size_t n = 0; n < (size_t) __init_count; n++) {
		void(*fn)(void) = __init_array[n]; if (fn) fn();
	}

	/* Change execution context */
	__set_PSP((uint32_t) __stack - 1024); // process stack pointer
	__set_CONTROL(CONTROL_SPSEL_Msk | CONTROL_nPRIV_Msk); // PROCESS_STACK PROCESS_PRIVILEGED

	/* Branch to main function */
	int main(void); exit(main());
}

// living on to tell the tale
#include "tinyusb/device/usbd.h"
#include "fiber.h"
void hardfault(unsigned int * frame)
{
	// disable all fibers except tud_task
	extern fiber_t fiber;
	for (int n = 0; n < FIBER; n++) fiber.funcs[n] = 0;
	fiber_create(&tud_task,1);
	fiber.now = 1;
	fiber.funcs[fiber.now] = (int (*)(void)) 1;
	printf (
		"HardFault\r\nSP %08X\r\nLR %08X\r\nPC %08X\r\nSR %08X\r\n",
		(unsigned int) frame, frame[5], frame[6], frame[7]
	);
	exit(-1); // reboot
}

// snatching the error code from the trap frame
__attribute__((naked))
void handler_hardfault(void)
{
	asm volatile (
	"	movs	R0, #4			\n"
	"	mov		R1, LR			\n"
	"	tst		R0, R1			\n"
	"	bne		1f				\n" // branch forward to one
	"	mrs		R0, MSP			\n" // came from interrupt
	"	b		2f				\n" // branch forward to two
"	1:	mrs		R0, PSP			\n" // came from process
"	2:	ldr		R2, =hardfault	\n"
	"	ldr		R3, =0x01000000	\n" // StateReg Thumb flag
//	"	movs	R4, #3			\n"
//	"	msr		control, R4		\n"
//	"	isb		0xF				\n"
	"	push	{R1-R3}			\n" // craft frame LR PC SR
	"	sub		SP, #16			\n"
	"	push	{R0}			\n" // craft frame argument
//	"	ldr		R0, =0xFFFFFFFD \n" // EXC_RETURN_THREAD_PSP
	"	ldr		R0, =0xFFFFFFF9 \n" // EXC_RETURN_THREAD_MSP
	"	bx		R0				\n"
	:: "i"(&hardfault) : "r0","r1","r2","r3"
	);
}
