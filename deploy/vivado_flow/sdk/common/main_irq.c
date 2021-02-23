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

#include "src.h"
#include "dst.h"

#define __DEBUG__

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


#define MAX_PRINT_ELEMENTS (4)

#define ITERATION_FACTOR (100)

const unsigned INPUT_N_ELEMENTS = src_SAMPLE_COUNT*src_FEATURE_COUNT;
const unsigned OUTPUT_N_ELEMENTS = dst_SAMPLE_COUNT*dst_FEATURE_COUNT;

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

/* the driver instance for GPIO device. */
XGpioPs gpio_ps;
XGpioPs_Config *gpio_ps_config;

/* */
int disable_peripherals()
{
	s32 status;
	u32 counter;

	gpio_ps_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);

	status = XGpioPs_CfgInitialize(&gpio_ps, gpio_ps_config, gpio_ps_config->BaseAddr);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * LED over the GPIO
	 */
	u32 led_r_pin = 52;
	u32 led_g_pin = 53;

	/*
	 * USB reset over the GPIO
	 */
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


	/* set the GPIO output to be low */
	//XGpioPs_WritePin(&gpio_ps, output_pin, 0x1);
#if 0
	for (counter = 0; counter < 4; counter++)
	{
		sleep(1);
		XGpioPs_WritePin(&gpio_ps, led_r_pin, 0x1);
		sleep(1);
		XGpioPs_WritePin(&gpio_ps, led_g_pin, 0x0);
		sleep(1);
		XGpioPs_WritePin(&gpio_ps, led_r_pin, 0x0);
		sleep(1);
		XGpioPs_WritePin(&gpio_ps, led_g_pin, 0x1);
	}
#endif
	XGpioPs_WritePin(&gpio_ps, led_r_pin, 0x0);
	XGpioPs_WritePin(&gpio_ps, led_g_pin, 0x0);

	/* disable USB */
	//XGpioPs_WritePin(&gpio_ps, usb_rst_pin, 0x1);
	xil_printf("INFO: Press any key to test USB reset: ");
	u32 dummy = inbyte();
	for (counter = 0; counter < 20; counter++)
	{
		sleep(10);
		xil_printf("USB OFF\n\r");
		XGpioPs_WritePin(&gpio_ps, usb_rst_pin, 0x0);
		sleep(10);
		xil_printf("USB ON\n\r");
		XGpioPs_WritePin(&gpio_ps, usb_rst_pin, 0x1);
	}

	return XST_SUCCESS;
}

/* accelerator(s) configuration */
XJet_tagger_axi do_jet_tagger;
XJet_tagger_axi_Config *do_jet_tagger_cfg;

/* accelerator(s) initialization routine */
void init_accelerators()
{
    xil_printf("INFO: Initializing accelerator\n\r");
    int status = XJet_tagger_axi_Initialize(&do_jet_tagger, XPAR_JET_TAGGER_AXI_DEVICE_ID);
    if (status != XST_SUCCESS)
    {
        xil_printf("ERROR: Initializing accelerator\n\r");
    }
}

/* instance of the Interrupt Controller */
XScuGic interrupt_controller;
int jet_tagger_axi_done;
int jet_tagger_axi_run;

void jet_tagger_axi_start(void *instance_ptr) {

	XJet_tagger_axi *p_do_jet_tagger = (XJet_tagger_axi *) instance_ptr;

	XJet_tagger_axi_InterruptEnable(p_do_jet_tagger, 1);

	XJet_tagger_axi_InterruptGlobalEnable(p_do_jet_tagger);

	XJet_tagger_axi_Start(p_do_jet_tagger);
}

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

	/*
	 * self test
	 */
	status = XScuGic_SelfTest(&interrupt_controller);
	if(status != XST_SUCCESS){
		return status;
	}

	/*
	 * initialize the exception handler
	 */
	Xil_ExceptionInit();

	/*
	 * register the exception handler
	 */
	 Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &interrupt_controller);

	 /*
	  * enable the exception handler
	  */
	 Xil_ExceptionEnable();


	 /*
	  * connect the accelerator ISR to the exception table
	  */
	 status = XScuGic_Connect(&interrupt_controller, XPAR_FABRIC_JET_TAGGER_AXI_INTERRUPT_INTR, (Xil_InterruptHandler)jet_tagger_axi_isr, &do_jet_tagger);
	 if(status != XST_SUCCESS){
	 return status;
	 }

	 /*
	  * enable the interrupts for the accelerator
	  */
	 XScuGic_Enable(&interrupt_controller, XPAR_FABRIC_JET_TAGGER_AXI_INTERRUPT_INTR);


	 return XST_SUCCESS;
}

/* golden model of the accelerator in software */
int jet_tagger_sw(unsigned short *src, unsigned short *dst, unsigned input_n_elements, unsigned output_n_elements)
{
    xil_printf("INFO: Golden results are pre-compiled. It would be nice to run a software model here.\n\r");
    // See src.h and dst.h for input and golden output respectively.
    return 0;
}

/* profiling function */
double get_elapsed_time(XTime start, XTime stop)
{
    return 1.0 * (stop - start) / (COUNTS_PER_SECOND);
}

/* dump data to the console */
void dump_data(const char* label, unsigned short* data, unsigned sample_count, unsigned feature_count, unsigned print_hex, unsigned print_bin)
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

/* the top of the hill :-) */
int main(int argc, char** argv)
{
#ifdef PROFILING
    XTime start, stop;
    double calibration_time;
    double sw_elapsed;
#endif
    char __attribute__ ((unused)) dummy; /* dummy input */

    int hw_errors;
#ifdef PROFILING
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

    /* initialize the accelerator(s) */
    init_accelerators();

    /* initialize accelerator interrupt(s) */
    init_interrupts();

    src_mem = malloc(INPUT_N_ELEMENTS * sizeof(unsigned short));
    dst_mem = malloc(OUTPUT_N_ELEMENTS * sizeof(unsigned short));
    gld_mem = malloc(OUTPUT_N_ELEMENTS * sizeof(unsigned short));

    /* calibration */
#ifdef PROFILING
    XTime_GetTime(&start);
    sleep(1);
    XTime_GetTime(&stop);
    calibration_time = get_elapsed_time(start, stop);
    xil_printf("INFO: Time calibration for one second (%lf sec)\n\r", calibration_time);
#endif

    /* initialize memory */
    xil_printf("INFO: Initialize memory\n\r");
    xil_printf("INFO:   - Sample count: %u\n\r", src_SAMPLE_COUNT); /* Same as dst_SAMPLE_COUNT */
    xil_printf("INFO:   - Input-feature count: %u\n\r", src_FEATURE_COUNT);
    xil_printf("INFO:   - Output-class count: %u\n\r", dst_FEATURE_COUNT);
    xil_printf("INFO:   - Data size: %u B\n\r", sizeof(unsigned short));
    xil_printf("INFO:   - Total input size: %u B, %.2f KB, %.2f MB\n\r", src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short), (src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short)) / (float)1024, (src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short)) / (float)(1024*1024));
    xil_printf("INFO:   - Total output size: %u B, %.2f KB, %.2f MB\n\r", dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short), (dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short)) / (float)1024, (dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short)) / (float)(1024*1024));

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
#ifdef PROFILING
    XTime_GetTime(&start);
#endif
    jet_tagger_sw(src_mem, gld_mem, INPUT_N_ELEMENTS, OUTPUT_N_ELEMENTS);
#ifdef PROFILING
    XTime_GetTime(&stop);
    sw_elapsed = get_elapsed_time(start, stop);
#endif
#ifdef __DEBUG__
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
#ifdef PROFILING
        XTime_GetTime(&start);
#endif
        Xil_DCacheFlushRange((UINTPTR)src_mem, INPUT_N_ELEMENTS * sizeof(unsigned short));
        Xil_DCacheFlushRange((UINTPTR)dst_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
        Xil_DCacheFlushRange((UINTPTR)gld_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
#ifdef PROFILING
        XTime_GetTime(&stop);
        cache_elapsed = get_elapsed_time(start, stop);
#endif
    	unsigned short *src_mem_i = src_mem;
    	unsigned short *dst_mem_i = dst_mem;

    	for (unsigned i = 0; i < src_SAMPLE_COUNT; i++) {

    		/* Configure the accelerator */
#ifdef PROFILING
    		XTime_GetTime(&start);
#endif
    		XJet_tagger_axi_Set_in_V(&do_jet_tagger, (unsigned)src_mem_i);
    		XJet_tagger_axi_Set_out_V(&do_jet_tagger, (unsigned)dst_mem_i);

    		jet_tagger_axi_start(&do_jet_tagger);

    		/* wait for interrupt */
    		while(!jet_tagger_axi_done);

    		jet_tagger_axi_done = 0;

    		/* get error status */
    		//hw_flags = XJet_tagger_axi_Get_return(&do_jet_tagger);
#ifdef PROFILING
    		XTime_GetTime(&stop);
    		hw_elapsed += get_elapsed_time(start, stop);
#endif
    		src_mem_i += src_FEATURE_COUNT;
    		dst_mem_i += dst_FEATURE_COUNT;
    	}
    }

#ifdef PROFILING
    XTime_GetTime(&start);
#endif
    Xil_DCacheFlushRange((UINTPTR)dst_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
#ifdef PROFILING
    XTime_GetTime(&stop);
    cache_elapsed += get_elapsed_time(start, stop);
#endif

    /* ****** VALIDATION ****** */

#ifdef __DEBUG__
    xil_printf("INFO: ================== Validation =================\n\r");
    xil_printf("INFO: Dump data\n\r");
    dump_data("src", (unsigned short*)src_mem, src_SAMPLE_COUNT, src_FEATURE_COUNT, 1, 0);
    dump_data("sw_dst", (unsigned short*)gld_mem, dst_SAMPLE_COUNT, dst_FEATURE_COUNT, 1, 0);
    dump_data("hw_dst", (unsigned short*)dst_mem, dst_SAMPLE_COUNT, dst_FEATURE_COUNT, 1, 0);
#endif
#ifdef PROFILING
    xil_printf("INFO: Software execution time: %f sec\n\r", sw_elapsed);

    xil_printf("INFO: Accelerator execution time: %f sec\n\r", hw_elapsed);
    xil_printf("INFO: Cache flush time: %f sec\n\r", cache_elapsed);
    xil_printf("INFO: Accelerator/software speedup (the sofware is fake so this does not count...): %.2f X\n\r", (sw_elapsed >= (hw_elapsed+cache_elapsed))?(sw_elapsed/(hw_elapsed+cache_elapsed)):-((hw_elapsed+cache_elapsed)/sw_elapsed));
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


