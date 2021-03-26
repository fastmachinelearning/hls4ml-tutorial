/**
 *
 * Set Heap Size in ldscript.ld to 0x1000000 (16MB)
 *
 */

/*
 * References:
 * https://www.xilinx.com/support/documentation/application_notes/xapp745-processor-control-vhls.pdf
 * https://www.xilinx.com/support/documentation/sw_manuals/ug998-vivado-intro-fpga-design-hls.pdf
 */

#include <xjet_tagger_axi.h>  /* accelerator */
#include <unistd.h>      /* sleep */
#include <xil_printf.h>  /* smaller footprint printf */
#include <stdlib.h>
#include <malloc.h>
#include <xil_io.h>      /* peripheral read/write wrappers */
#include <xtime_l.h>     /* to measure performance of the system */
#include <xil_cache.h>   /* enable/disable caches etc */
#include <xil_printf.h>  /* UART debug print functions */
#include <xparameters.h> /* peripherals base addresses */
#include <xscugic.h>     /* interrupt controller */
#include <xgpiops.h>

#include "platform.h"    /* platform init/cleanup functions */

/* for profiling you may need to run multiple time the total batch of iterations */
#define ITERATION_FACTOR (10000)

/* enable a higher verbosity on the console */
#define __DEBUG__
/* enable profiling - ATTENTION - it uses printf() which is known to be slow and take extra space in the final binary */
#define __PROFILING__

#ifdef __PROFILING__
#include <stdio.h>
#endif

#define CPU_FREQ_MHZ ((XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ)/1000000)

#define __LOW_POWER__
#ifdef __LOW_POWER__
#include "xil_misc_psreset_api.h"
#endif

/*
 * include input feature and expected predictions as hardcoded headers
 */
#include "src.h"
#include "dst.h"

const unsigned INPUT_N_ELEMENTS = src_SAMPLE_COUNT*src_FEATURE_COUNT;
const unsigned OUTPUT_N_ELEMENTS = dst_SAMPLE_COUNT*dst_FEATURE_COUNT;

/*
 * print utilities
 */
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define SHORT_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define SHORT_TO_BINARY(byte)  \
  (byte & 0x8000 ? '1' : '0'), \
  (byte & 0x4000 ? '1' : '0'), \
  (byte & 0x2000 ? '1' : '0'), \
  (byte & 0x1000 ? '1' : '0'), \
  (byte & 0x800 ? '1' : '0'), \
  (byte & 0x400 ? '1' : '0'), \
  (byte & 0x200 ? '1' : '0'), \
  (byte & 0x100 ? '1' : '0'), \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define INT_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define INT_TO_BINARY(byte)  \
  (byte & 0x80000000 ? '1' : '0'), \
  (byte & 0x40000000 ? '1' : '0'), \
  (byte & 0x20000000 ? '1' : '0'), \
  (byte & 0x10000000 ? '1' : '0'), \
  (byte & 0x8000000 ? '1' : '0'), \
  (byte & 0x4000000 ? '1' : '0'), \
  (byte & 0x2000000 ? '1' : '0'), \
  (byte & 0x1000000 ? '1' : '0'), \
  (byte & 0x800000 ? '1' : '0'), \
  (byte & 0x400000 ? '1' : '0'), \
  (byte & 0x200000 ? '1' : '0'), \
  (byte & 0x100000 ? '1' : '0'), \
  (byte & 0x80000 ? '1' : '0'), \
  (byte & 0x40000 ? '1' : '0'), \
  (byte & 0x20000 ? '1' : '0'), \
  (byte & 0x10000 ? '1' : '0'), \
  (byte & 0x8000 ? '1' : '0'), \
  (byte & 0x4000 ? '1' : '0'), \
  (byte & 0x2000 ? '1' : '0'), \
  (byte & 0x1000 ? '1' : '0'), \
  (byte & 0x800 ? '1' : '0'), \
  (byte & 0x400 ? '1' : '0'), \
  (byte & 0x200 ? '1' : '0'), \
  (byte & 0x100 ? '1' : '0'), \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

/* maximum number of elements that are printed on console by the print_data function */
#define MAX_PRINT_ELEMENTS (2)

/* dump data to the console */
void print_data(const char* label, unsigned short* data, unsigned sample_count, unsigned feature_count, unsigned print_hex, unsigned print_bin)
{
    xil_printf("INFO:   %s[%u][%u]:\n\r", label, sample_count, feature_count);
    /* print at most MAX_PRINT_ELEMENTS */
    for (unsigned i = 0; i < sample_count && i < MAX_PRINT_ELEMENTS; i++)
    {
        xil_printf("INFO:     [%u] ", i);
        if (print_hex)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf("%03X ", data[index]);
            }
        if (print_bin)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf(""SHORT_TO_BINARY_PATTERN, SHORT_TO_BINARY(data[index]));
                xil_printf(" ");
            }
        xil_printf("\n\r");
    }
    for (unsigned i = sample_count - MAX_PRINT_ELEMENTS; i < sample_count; i++)
    {
        xil_printf("INFO:     [%u] ", i);
        if (print_hex)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf("%03X ", data[index]);
            }
        if (print_bin)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf(""SHORT_TO_BINARY_PATTERN, SHORT_TO_BINARY(data[index]));
                xil_printf(" ");
            }
        xil_printf("\n\r");
    }
}


/*
 * hardcoded addresses for memory scratchpads vs. dynamic allocation
 */
#if 0
/* base address for the accelerator */
#define MEM_BASE_ADDR XPAR_PS7_DDR_0_S_AXI_BASEADDR

/* data offsets and pointers */
#define SRC_BUFFER_BASE (MEM_BASE_ADDR + 0x00000000)
unsigned short *src_mem = (unsigned short*)SRC_BUFFER_BASE;

#define GLD_BUFFER_BASE (MEM_BASE_ADDR + 0x00010000)
unsigned short *gld_mem = (unsigned short*)GLD_BUFFER_BASE;

#define DST_BUFFER_BASE (MEM_BASE_ADDR + 0x00020000)
unsigned short *dst_mem = (unsigned short*)DST_BUFFER_BASE;
#else
unsigned short *src_mem;
unsigned short *gld_mem;
unsigned short *dst_mem;
#endif

/*
 * the driver instance for GPIO device.
 */
XGpioPs gpio_ps;
XGpioPs_Config *gpio_ps_config;


#define wfi() __asm__("wfi")

/* disable a certain set of peripheral to reduce power */
int disable_peripherals()
{
	s32 status;

	gpio_ps_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);

	status = XGpioPs_CfgInitialize(&gpio_ps, gpio_ps_config, gpio_ps_config->BaseAddr);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * LED and USB GPIO pins
	 */
	u32 led_r_pin = 52;
	u32 led_g_pin = 53;
	u32 usb_rst_pin = 7;

	/*
	 * set the direction for the pins to be output and
	 * enable the output enable for the pins
	 */
	XGpioPs_SetDirectionPin(&gpio_ps, led_r_pin, 1);
	XGpioPs_SetOutputEnablePin(&gpio_ps, led_r_pin, 1);
	XGpioPs_SetDirectionPin(&gpio_ps, led_g_pin, 1);
	XGpioPs_SetOutputEnablePin(&gpio_ps, led_g_pin, 1);
	XGpioPs_SetDirectionPin(&gpio_ps, usb_rst_pin, 1);
	XGpioPs_SetOutputEnablePin(&gpio_ps, usb_rst_pin, 1);

	/* set the GPIO outputs to be low (disabled)*/
	XGpioPs_WritePin(&gpio_ps, led_r_pin, 0x0);
	XGpioPs_WritePin(&gpio_ps, led_g_pin, 0x0);
	XGpioPs_WritePin(&gpio_ps, usb_rst_pin, 0x0);

	xil_printf("INFO: Software-controlled power reduction\n\r");
	xil_printf("INFO:   - Disable red PS-LED (GPIO pin %u)\n\r", led_r_pin);
	xil_printf("INFO:   - Disable green PS-LED (GPIO pin %u)\n\r", led_g_pin);
	xil_printf("INFO:   - Disable USB 2.0 host controller (GPIO pin %u)\n\r", usb_rst_pin);

	/*
	 * The System-Level Control registers (SLCR) consist of various registers
	 * that are used to control the PS behavior. These registers are accessible
	 * via the central interconnect using load and store instructions.
	 * See: https://www.xilinx.com/support/documentation/user_guides/ug585-Zynq-7000-TRM.pdf#page=114
	 */
	u32 data;

	/* unlock SLCR registers which can be protected by default */
	data = Xil_In32(XSLCR_UNLOCK_ADDR);
	Xil_Out32(XSLCR_UNLOCK_ADDR, XSLCR_UNLOCK_CODE);


	/*
	 * AMBA peripheral clock control
	 */
#if 0
	const unsigned APER_CLK_CTRL = 0XF800012C;

	data = Xil_In32(APER_CLK_CTRL);
	xil_printf("INFO: Register slcr.APER_CLK_CTR    (default) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", APER_CLK_CTRL, data, INT_TO_BINARY(data));
	/* keep alive only clock to UART 1 */
	data = /*0x1 + 0x4 + 0x8 + 0x40000 + 0x80000 +*/ 0x200000 /*+ 0x400000 + 0x1000000*/;
	Xil_Out32(APER_CLK_CTRL, data);
	xil_printf("INFO:                               (updated) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", APER_CLK_CTRL, data, INT_TO_BINARY(data));
#endif

	//data = Xil_In32(XSLCR_ARM_CLK_CTRL_ADDR);
	//xil_printf("INFO: Register slcr.ARM_CLK_CTRL  @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", APER_CLK_CTRL, data, INT_TO_BINARY(data));

	/*
	 * DDR clock control
	 *
	 * https://0xstubs.org/zynq-ddr-self-refresh
	 *
	 * Procedure as described in section 10.9.6
	 * of the Zynq TRM:
	 *
	 * Register addresses and bit offsets are found in
	 * Appendix B of the TRM.
	 */

#if 0
	const unsigned XDDRC_CTRL_REG1 = XDDRC_CTRL_BASEADDR + 0x60;
	const unsigned XDDRC_CTRL_REG3 = XDDRC_CTRL_BASEADDR + 0x20;
	const unsigned DCI_CLK_CTRL = 0XF8000128;
	data = Xil_In32(XDDRC_CTRL_REG1);
	xil_printf("INFO: Register slcr.XDDRC_CTRL_REG1 (default) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XDDRC_CTRL_REG1, data, INT_TO_BINARY(data));
	data |= 0x00001000;
	Xil_Out32(XDDRC_CTRL_REG1, data);
	xil_printf("INFO:                               (updated) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XDDRC_CTRL_REG1, data, INT_TO_BINARY(data));

	data = Xil_In32(XDDRC_CTRL_REG3);
	xil_printf("INFO: Register slcr.XDDRC_CTRL_REG1 (default) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XDDRC_CTRL_REG3, data, INT_TO_BINARY(data));
	data |= 0x00100000;
	Xil_Out32(XDDRC_CTRL_REG3, data);
	xil_printf("INFO:                               (updated) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XDDRC_CTRL_REG3, data, INT_TO_BINARY(data));
#endif

	/* slcr.DDR_CLK_CTRL[DDR_2XCLKACT] = 0 */
	/* slcr.DDR_CLK_CTRL[DDR_3XCLKACT] = 0 */
	data = Xil_In32(XSLCR_DDR_CLK_CTRL_ADDR);
	xil_printf("INFO: Register slcr.DDR_CLK_CTRL    (default) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XSLCR_DDR_CLK_CTRL_ADDR, data, INT_TO_BINARY(data));
	data &= ~(1<<1); /* 0xFFFFFFFD */
	data &= ~(1<<0); /* 0xFFFFFFFE */
	Xil_Out32(XSLCR_DDR_CLK_CTRL_ADDR, data);
	xil_printf("INFO:                               (updated) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XSLCR_DDR_CLK_CTRL_ADDR, data, INT_TO_BINARY(data));

	/* slcr.DCI_CLK_CTRL[CLKACT] = 0 */
#if 0
	data = Xil_In32(DCI_CLK_CTRL);
	xil_printf("INFO: Register slcr.DCI_CLK_CTRL    (default) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", DCI_CLK_CTRL, data, INT_TO_BINARY(data));
	data &= ~(1 << 0); /* 0xFFFFFFFD */
	Xil_Out32(DCI_CLK_CTRL, data);
	xil_printf("INFO:                               (updated) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", DCI_CLK_CTRL, data, INT_TO_BINARY(data));
#endif


	/*
	 * ARM PLL control
	 */
	//data = Xil_In32(XSLCR_ARM_PLL_CTRL_ADDR);
	//xil_printf("INFO: Register slcr.ARM_PLL_CTRL  @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XSLCR_ARM_PLL_CTRL_ADDR, data, INT_TO_BINARY(data));
	//data |= 0x00000003;
	//data = 0x00030003;
	//xil_printf("INFO: Register slcr.ARM_PLL_CTRL  @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XSLCR_ARM_PLL_CTRL_ADDR, data, INT_TO_BINARY(data));

	/*
	 * DDR PLL control
     */
#if 0
	data = Xil_In32(XSLCR_DDR_PLL_CTRL_ADDR);
	xil_printf("INFO: Register slcr.DDR_PLL_CTRL    (default) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XSLCR_DDR_PLL_CTRL_ADDR, data, INT_TO_BINARY(data));
	//data |= 0x00000003;
	//data = 0x0;
	xil_printf("INFO:                               (updated) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XSLCR_DDR_PLL_CTRL_ADDR, data, INT_TO_BINARY(data));
#endif

	/* Enable dynamic clock gating (CP15) */
	/* https://developer.arm.com/documentation/ddi0388/f/System-Control/Register-descriptions/Power-Control-Register?lang=en
	 * https://github.com/imrickysu/ZYNQ-Cookbook/blob/master/recipe/HowToOperateCP15.md
	 */
#if 0
	data = mfcp(XREG_CP15_POWER_CTRL);
	xil_printf("INFO: Register XREG_CP15_POWER_CTRL (default) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XREG_CP15_POWER_CTRL, data, INT_TO_BINARY(data));
	data |= 0x1;
	mtcp(XREG_CP15_POWER_CTRL, data);
	data = mfcp(XREG_CP15_POWER_CTRL);
	xil_printf("INFO:                               (updated) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", XREG_CP15_POWER_CTRL, data, INT_TO_BINARY(data));
	usleep(10000000);
#endif


	/* Enable L2 cache standby mode and dynamic clock gating */
#if 0
	const unsigned L2C_PL310_REG15 = 0xF8F02F80;

	data = Xil_In32(L2C_PL310_REG15);
	xil_printf("INFO: Register L2CPL310_REG15       (default) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", L2C_PL310_REG15, data, INT_TO_BINARY(data));
	data = 0x3;
	Xil_Out32(L2C_PL310_REG15, data);
	data = Xil_In32(L2C_PL310_REG15);
	xil_printf("INFO:                               (updated) @ 0x%08X : 0x%08X "INT_TO_BINARY_PATTERN"\n\r", L2C_PL310_REG15, data, INT_TO_BINARY(data));
	usleep(1000000);
#endif

	return XST_SUCCESS;
}

/*
 * accelerator(s) configuration
 */
XJet_tagger_axi do_jet_tagger;
XJet_tagger_axi_Config *do_jet_tagger_cfg;

/*
 * accelerator(s) initialization routine
 */
void init_accelerators()
{
    xil_printf("INFO: Initializing accelerator\n\r");
    int status = XJet_tagger_axi_Initialize(&do_jet_tagger, XPAR_JET_TAGGER_AXI_DEVICE_ID);
    if (status != XST_SUCCESS)
    {
        xil_printf("ERROR: Initializing accelerator\n\r");
    }
}

/*
 * instance of the Interrupt Controller
 */
XScuGic interrupt_controller;
int jet_tagger_axi_done;
int jet_tagger_axi_run;

/*
 * wrap interrupt functionalities and run the accelerator,
 * after this you have to wait for the interrupt to arrive
 */
void jet_tagger_axi_start(void *instance_ptr) {

	XJet_tagger_axi *p_do_jet_tagger = (XJet_tagger_axi *) instance_ptr;

	XJet_tagger_axi_InterruptEnable(p_do_jet_tagger, 1);

	XJet_tagger_axi_InterruptGlobalEnable(p_do_jet_tagger);

	XJet_tagger_axi_Start(p_do_jet_tagger);
}

/*
 * interrupt serving routine
 */
void jet_tagger_axi_isr(void *instance_ptr){

	XJet_tagger_axi *p_do_jet_tagger = (XJet_tagger_axi *) instance_ptr;

	/*
	 * Disable the global interrupt
	 */
	XJet_tagger_axi_InterruptGlobalDisable(p_do_jet_tagger);
	XJet_tagger_axi_InterruptDisable(p_do_jet_tagger, 0xffffffff);

	/*
	 * Clear the local interrupt
	 */
	 XJet_tagger_axi_InterruptClear(p_do_jet_tagger, 1);

	jet_tagger_axi_done = 1;

	/*
	 * Restart the accelerator if it should be run again
	 */
	if (jet_tagger_axi_run) {
		jet_tagger_axi_start(p_do_jet_tagger);
	}

}

/*
 * interrupt setup
 */
int init_interrupts(){
	int status;

	XScuGic_Config *p_cfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);

	if(p_cfg == NULL){
		xil_printf("Interrupt Configuration Lookup Failed\n\r");
		return XST_FAILURE;
	}

	status = XScuGic_CfgInitialize(&interrupt_controller, p_cfg,p_cfg->CpuBaseAddress);
	if(status != XST_SUCCESS){
		return status;
	}

	/* self test */
	status = XScuGic_SelfTest(&interrupt_controller);
	if(status != XST_SUCCESS){
		return status;
	}

	/* initialize the exception handler */
	Xil_ExceptionInit();

	/* register the exception handler */
	 Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &interrupt_controller);

	 /* enable the exception handler */
	 Xil_ExceptionEnable();


	 /* connect the accelerator ISR to the exception table */
	 status = XScuGic_Connect(&interrupt_controller, XPAR_FABRIC_JET_TAGGER_AXI_INTERRUPT_INTR, (Xil_InterruptHandler)jet_tagger_axi_isr, &do_jet_tagger);
	 if(status != XST_SUCCESS){
		 return status;
	 }

	 /* enable the interrupts for the accelerator */
	 XScuGic_Enable(&interrupt_controller, XPAR_FABRIC_JET_TAGGER_AXI_INTERRUPT_INTR);


	 return XST_SUCCESS;
}

/*
 * golden model of the accelerator in software
 */
int jet_tagger_sw(unsigned short *src, unsigned short *dst, unsigned input_n_elements, unsigned output_n_elements)
{
    xil_printf("INFO: Golden results are pre-compiled. It would be nice to run a software model here.\n\r");
    // See src.h and dst.h for input and golden output respectively.
    return 0;
}

/*
 * profiling function
 */
double get_elapsed_time(XTime start, XTime stop)
{
    return 1.0 * (stop - start) / (COUNTS_PER_SECOND);
}

/*
 * the top of the hill :-)
 */
int main(int argc, char** argv)
{
#ifdef __PROFILING__
    XTime start, stop;
    double calibration_time;
    double sw_elapsed;
#endif
    char __attribute__ ((unused)) dummy; /* dummy input */

    int hw_errors;
#ifdef __PROFILING__
    double hw_elapsed = 0;
    double cache_elapsed = 0;
#endif

    xil_printf("\n\r");
    xil_printf("INFO: ===============================================\n\r");
    xil_printf("INFO: Jet Tagger (w/ interrupt)\n\r");
    xil_printf("INFO: ===============================================\n\r");

    /* initialize platform */
    init_platform();

    disable_peripherals();

#if 0
    xil_printf("INFO: XSLCR_BASEADDR: [0x%X] = 0x%X ("INT_TO_BINARY_PATTERN")\n\r", XSLCR_BASEADDR, Xil_In32(XSLCR_BASEADDR), INT_TO_BINARY(Xil_In32(XSLCR_BASEADDR)));
    xil_printf("INFO: XSLCR_ARM_CLK_CTRL_ADDR: [0x%X] = 0x%X ("INT_TO_BINARY_PATTERN")\n\r", XSLCR_ARM_CLK_CTRL_ADDR, Xil_In32(XSLCR_ARM_CLK_CTRL_ADDR), INT_TO_BINARY(Xil_In32(XSLCR_ARM_CLK_CTRL_ADDR)));
#endif

    /* initialize the accelerator(s) */
    init_accelerators();

    /* initialize accelerator interrupt(s) */
    init_interrupts();

    src_mem = malloc(INPUT_N_ELEMENTS * sizeof(unsigned short));
    dst_mem = malloc(OUTPUT_N_ELEMENTS * sizeof(unsigned short));
    gld_mem = malloc(OUTPUT_N_ELEMENTS * sizeof(unsigned short));

    /* calibration */
#ifdef __PROFILING__
    XTime_GetTime(&start);
    sleep(1);
    XTime_GetTime(&stop);
    calibration_time = get_elapsed_time(start, stop);
    printf("INFO: Time calibration for one second (%lf sec)\n\r", calibration_time);
#endif

    /* initialize memory */
    xil_printf("INFO: Initialize memory\n\r");
    xil_printf("INFO:   - Sample count: %u\n\r", src_SAMPLE_COUNT); /* Same as dst_SAMPLE_COUNT */
    xil_printf("INFO:   - Input-feature count: %u\n\r", src_FEATURE_COUNT);
    xil_printf("INFO:   - Output-class count: %u\n\r", dst_FEATURE_COUNT);
    xil_printf("INFO:   - Data size: %u B\n\r", sizeof(unsigned short));
    xil_printf("INFO:   - Total input size: %u B\n\r", src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short));
    xil_printf("INFO:   - Total output size: %u B\n\r", dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short));
    /* xil_printf("INFO:   - Total input size: %u B, %.2f KB, %.2f MB\n\r", src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short), (src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short)) / (float)1024, (src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short)) / (float)(1024*1024)); */
    /* xil_printf("INFO:   - Total output size: %u B, %.2f KB, %.2f MB\n\r", dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short), (dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short)) / (float)1024, (dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short)) / (float)(1024*1024)); */

    // Set Heap Size in ldscript.ld to 0x1000000 (16MB)
    //malloc_stats();

    for (int i = 0; i < INPUT_N_ELEMENTS; i++) {
        src_mem[i] = src_data[i];
    }
    for (int i = 0; i < OUTPUT_N_ELEMENTS; i++) {
        gld_mem[i] = dst_data[i];
        dst_mem[i] = 0x0;
    }

    /* ****** SOFTWARE REFERENCE ****** */
#ifdef __DEBUG__
    xil_printf("INFO: Start SW accelerator\n\r");
#endif
#ifdef __PROFILING__
    XTime_GetTime(&start);
#endif
    jet_tagger_sw(src_mem, gld_mem, INPUT_N_ELEMENTS, OUTPUT_N_ELEMENTS);
#ifdef __PROFILING__
    XTime_GetTime(&stop);
    sw_elapsed = get_elapsed_time(start, stop);
#endif
#ifdef __DEBUG__
    xil_printf("INFO:\n\r");
    xil_printf("INFO: Number of accelerator invocations: %u (= %u * %u)\n\r", ITERATION_FACTOR*src_SAMPLE_COUNT, ITERATION_FACTOR, src_SAMPLE_COUNT);
    xil_printf("INFO:   - Iteration factor: %u\n\r", ITERATION_FACTOR);
    xil_printf("INFO:   - Sample count : %u\n\r", src_SAMPLE_COUNT);
#endif

    /* ****** ACCELERATOR ****** */
    xil_printf("INFO: Press any key to start the accelerator: ");
    dummy = inbyte();
    xil_printf("\n\rINFO: \n\r");

#ifdef __DEBUG__
    xil_printf("INFO: Configure and start accelerator\n\r");
#endif

    for (unsigned j = 0; j < ITERATION_FACTOR; j++) {
#ifdef __PROFILING__
        XTime_GetTime(&start);
#endif
        Xil_DCacheFlushRange((UINTPTR)src_mem, INPUT_N_ELEMENTS * sizeof(unsigned short));
        Xil_DCacheFlushRange((UINTPTR)dst_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
        Xil_DCacheFlushRange((UINTPTR)gld_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
#ifdef __PROFILING__
        XTime_GetTime(&stop);
        cache_elapsed = get_elapsed_time(start, stop);
#endif
    	unsigned short *src_mem_i = src_mem;
    	unsigned short *dst_mem_i = dst_mem;

    	for (unsigned i = 0; i < src_SAMPLE_COUNT; i++) {

    		/* Configure the accelerator */
#ifdef __PROFILING__
    		XTime_GetTime(&start);
#endif
    		XJet_tagger_axi_Set_in_V(&do_jet_tagger, (unsigned)src_mem_i);
    		XJet_tagger_axi_Set_out_V(&do_jet_tagger, (unsigned)dst_mem_i);

    		jet_tagger_axi_start(&do_jet_tagger);

#if 1
    		wfi();
#else
    		/* wait for interrupt */
    		while(!jet_tagger_axi_done);
#endif

    		jet_tagger_axi_done = 0;

    		/* get error status */
    		//hw_flags = XJet_tagger_axi_Get_return(&do_jet_tagger);
#ifdef __PROFILING__
    		XTime_GetTime(&stop);
    		hw_elapsed += get_elapsed_time(start, stop);
#endif
    		src_mem_i += src_FEATURE_COUNT;
    		dst_mem_i += dst_FEATURE_COUNT;
    	}
    }

#ifdef __PROFILING__
    XTime_GetTime(&start);
#endif
    Xil_DCacheFlushRange((UINTPTR)dst_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
#ifdef __PROFILING__
    XTime_GetTime(&stop);
    cache_elapsed += get_elapsed_time(start, stop);
#endif

    /* ****** VALIDATION ****** */

#ifdef __DEBUG__
    xil_printf("INFO: ================== Validation =================\n\r");
    xil_printf("INFO: Dump data\n\r");
    print_data("src", (unsigned short*)src_mem, src_SAMPLE_COUNT, src_FEATURE_COUNT, 1, 0);
    print_data("sw_dst", (unsigned short*)gld_mem, dst_SAMPLE_COUNT, dst_FEATURE_COUNT, 1, 0);
    print_data("hw_dst", (unsigned short*)dst_mem, dst_SAMPLE_COUNT, dst_FEATURE_COUNT, 1, 0);
#endif
#ifdef __PROFILING__
    printf("INFO: CPU frequency: %u MHz\n\r", CPU_FREQ_MHZ);
    printf("INFO: Software execution time: %f sec\n\r", sw_elapsed);
    printf("INFO: Accelerator execution time: %f sec\n\r", hw_elapsed);
    printf("INFO: Cache flush time: %f sec\n\r", cache_elapsed);
    printf("INFO: Accelerator/software speedup (the sofware is fake so this does not count...): %.2f X\n\r", (sw_elapsed >= (hw_elapsed+cache_elapsed))?(sw_elapsed/(hw_elapsed+cache_elapsed)):-((hw_elapsed+cache_elapsed)/sw_elapsed));
#endif

    /* Accelerator validation */
    hw_errors = 0;
    for (int i = 0; i < OUTPUT_N_ELEMENTS; i++)
    {
        if (dst_mem[i] != gld_mem[i])
        {
            xil_printf("ERROR: [%d]: Accelerator hw %03X != sw %03X\n\r", i, dst_mem[i], gld_mem[i]);
            hw_errors++;
        }
    }
    xil_printf("INFO: Total errors = %d (out of %d elements)\n\r", hw_errors, OUTPUT_N_ELEMENTS);
    if (hw_errors > 0)
        xil_printf("INFO: Accelerator validation: FAIL\n\r");
    else
        xil_printf("INFO: Accelerator validation: PASS!\n\r");

    xil_printf("INFO: done!\n\r");
    xil_printf("INFO: ===============================================\n\r");

    cleanup_platform();

    return 0;
}


