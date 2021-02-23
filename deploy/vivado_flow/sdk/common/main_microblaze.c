/**
 *
 * Set Heap Size in ldscript.ld to 0x2000
 *
 */

#include "xjet_tagger_axi.h"  /* accelerator */
#include "stdio.h"       /* printf */
#include "unistd.h"      /* sleep */
#include "stdlib.h"
#include "malloc.h"
#include "xil_io.h"      /* peripheral read/write wrappers */
#if 0
#include "xtime_l.h"     /* to measure performance of the system */
#endif
#include "platform.h"    /* platform init/cleanup functions */
#include "xil_cache.h"   /* enable/disable caches etc */
#include "xil_printf.h"  /* UART debug print functions */
#include "xparameters.h" /* peripherals base addresses */


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

#define ITERATION_FACTOR (10000000)

const unsigned INPUT_N_ELEMENTS = src_SAMPLE_COUNT*src_FEATURE_COUNT;
const unsigned OUTPUT_N_ELEMENTS = dst_SAMPLE_COUNT*dst_FEATURE_COUNT;

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

/* accelerator configuration */
XJet_tagger_axi do_jet_tagger;
XJet_tagger_axi_Config *do_jet_tagger_cfg;

/* accelerator initialization routine */
void init_accelerators()
{
    printf("INFO: Initializing accelerator\n\r");
    do_jet_tagger_cfg = XJet_tagger_axi_LookupConfig(XPAR_JET_TAGGER_AXI_DEVICE_ID);
    if (do_jet_tagger_cfg)
    {
        int status  = XJet_tagger_axi_CfgInitialize(&do_jet_tagger, do_jet_tagger_cfg);
        if (status != XST_SUCCESS)
        {
            printf("ERROR: Initializing accelerator\n\r");
        }
    }
}

//#if defined(__HPC_ACCELERATOR__) || defined(__ACP_ACCELERATOR__)
///*
// *  TODO: remember to edit core_baremetal_polling_bsp/psu_cortexa53_0/libsrc/standalon_v6_5/src/bspconfig.h
// *
// *  #define EL1_NONSECURE 1
// *
// */
//void init_accelerator_coherency(UINTPTR base_addr)
//{
//    /* Enable snooping of APU caches from CCI */
//    Xil_Out32(0xFD6E4000, 0x1);
//
//    /* Configure AxCACHE for write-back read and write-allocate (ARCACHE is [7:4], AWCACHE is [11:8]) */
//    /* Configure AxPROT[2:0] for data access [2], secure access [1], unprivileged access [0] */
//    Xil_Out32(base_addr, 0xFF0);
//}
//#endif

/* golden model of the accelerator in software */
int jet_tagger_sw(unsigned short *src, unsigned short *dst, unsigned input_n_elements, unsigned output_n_elements)
{
    printf("INFO: Golden results are pre-compiled. It would be nice to run a software model here.\n");
    // See src.h and dst.h for input and golden output respectively.
    return 0;
}

#if 0
/* profiling function */
double get_elapsed_time(XTime start, XTime stop)
{
    return 1.0 * (stop - start) / (COUNTS_PER_SECOND);
}
#endif

/* dump data to the console */
void dump_data(const char* label, unsigned short* data, unsigned sample_count, unsigned feature_count, unsigned print_hex, unsigned print_bin)
{
    printf("INFO:   %s[%u][%u]:\n\r", label, sample_count, feature_count);
    /* print at most MAX_PRINT_ELEMENTS */
    for (unsigned i = 0; i < sample_count && i < MAX_PRINT_ELEMENTS; i++)
    {
        printf("INFO:     [%u] ", i);
        if (print_hex)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                printf("%03X ", data[index]);
            }
        if (print_bin)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                printf(""SHORT_TO_BINARY_PATTERN, SHORT_TO_BINARY(data[index]));
                printf(" ");
            }
        printf("\n\r");
    }
    for (unsigned i = sample_count - MAX_PRINT_ELEMENTS; i < sample_count; i++)
    {
        printf("INFO:     [%u] ", i);
        if (print_hex)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                printf("%03X ", data[index]);
            }
        if (print_bin)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                printf(""SHORT_TO_BINARY_PATTERN, SHORT_TO_BINARY(data[index]));
                printf(" ");
            }
        printf("\n\r");
    }
}

/* the top of the hill :-) */
int main(int argc, char** argv)
{

#if 0
    XTime start, stop;
    double calibration_time;
    double sw_elapsed;
#endif
    char __attribute__ ((unused)) dummy; /* dummy input */

    int hw_errors;
    double hw_elapsed = 0;
    double cache_elapsed = 0;

    printf("\n\r");
    printf("INFO: ===============================================\n\r");
    printf("INFO: Jet Tagger (w/ polling)\n\r");
    printf("INFO: ===============================================\n\r");

    /* initialize platform (uart and caches) */
    init_platform();

    init_accelerators();

    src_mem = malloc(INPUT_N_ELEMENTS * sizeof(unsigned short));
    dst_mem = malloc(OUTPUT_N_ELEMENTS * sizeof(unsigned short));
    gld_mem = malloc(OUTPUT_N_ELEMENTS * sizeof(unsigned short));

#if 0
    /* calibration */
    XTime_GetTime(&start);
    sleep(1);
    XTime_GetTime(&stop);
    calibration_time = get_elapsed_time(start, stop);
    printf("INFO: Time calibration for one second (%lf sec)\n\r", calibration_time);
#endif

    /* initialize memory */
    printf("INFO: Initialize memory\n\r");
    printf("INFO:   - Sample count: %u\n\r", src_SAMPLE_COUNT); /* Same as dst_SAMPLE_COUNT */
    printf("INFO:   - Input-feature count: %u\n\r", src_FEATURE_COUNT);
    printf("INFO:   - Output-class count: %u\n\r", dst_FEATURE_COUNT);
    printf("INFO:   - Data size: %u B\n\r", sizeof(unsigned short));
    printf("INFO:   - Total input size: %u B, %.2f KB, %.2f MB\n\r", src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short), (src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short)) / (float)1024, (src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short)) / (float)(1024*1024));
    printf("INFO:   - Total output size: %u B, %.2f KB, %.2f MB\n\r", dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short), (dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short)) / (float)1024, (dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short)) / (float)(1024*1024));

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
    printf("INFO: Start SW accelerator\n\r");
#endif
#if 0
    XTime_GetTime(&start);
#endif

    jet_tagger_sw(src_mem, gld_mem, INPUT_N_ELEMENTS, OUTPUT_N_ELEMENTS);
#if 0
    XTime_GetTime(&stop);
    sw_elapsed = get_elapsed_time(start, stop);
#endif
#ifdef __DEBUG__
    printf("INFO: Number of accelerator invocations: %u (= %u * %u)\n\r", ITERATION_FACTOR*src_SAMPLE_COUNT, ITERATION_FACTOR, src_SAMPLE_COUNT);
    printf("INFO:   - Iteration factor: %u\n\r", ITERATION_FACTOR);
    printf("INFO:   - Sample count : %u\n\r", src_SAMPLE_COUNT);
#endif

    /* ****** ACCELERATOR ****** */
    xil_printf("INFO: Press any key to start the accelerator: ");
    dummy = inbyte();
    printf("\n\rINFO: \n\r");

#ifdef __DEBUG__
    printf("INFO: Configure and start accelerator\n\r");
#endif

    for (unsigned j = 0; j < ITERATION_FACTOR; j++) {

#if 0
        XTime_GetTime(&start);
#endif
        Xil_DCacheFlushRange((UINTPTR)src_mem, INPUT_N_ELEMENTS * sizeof(unsigned short));
        Xil_DCacheFlushRange((UINTPTR)dst_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
        Xil_DCacheFlushRange((UINTPTR)gld_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
#if 0
        XTime_GetTime(&stop);
        cache_elapsed = get_elapsed_time(start, stop);
#endif

    	unsigned short *src_mem_i = src_mem;
    	unsigned short *dst_mem_i = dst_mem;

    	for (unsigned i = 0; i < src_SAMPLE_COUNT; i++) {

    		/* Configure the accelerator */
#if 0
    		XTime_GetTime(&start);
#endif
    		XJet_tagger_axi_Set_in_V(&do_jet_tagger, (unsigned)src_mem_i);
    		XJet_tagger_axi_Set_out_V(&do_jet_tagger, (unsigned)dst_mem_i);

    		XJet_tagger_axi_Start(&do_jet_tagger);

    		/* polling */
    		while (!XJet_tagger_axi_IsDone(&do_jet_tagger));

    		/* get error status */
    		//hw_flags = XJet_tagger_axi_Get_return(&do_jet_tagger);
#if 0
    		XTime_GetTime(&stop);
    		hw_elapsed += get_elapsed_time(start, stop);
#endif
    		src_mem_i += src_FEATURE_COUNT;
    		dst_mem_i += dst_FEATURE_COUNT;
    	}
    }

#if 0
    XTime_GetTime(&start);
    Xil_DCacheFlushRange((UINTPTR)dst_mem, OUTPUT_N_ELEMENTS * sizeof(unsigned short));
    XTime_GetTime(&stop);
    cache_elapsed += get_elapsed_time(start, stop);
#endif
    /* ****** VALIDATION ****** */

#ifdef __DEBUG__
    printf("INFO: ================== Validation =================\n\r");
    printf("INFO: Dump data\n\r");
    dump_data("src", (unsigned short*)src_mem, src_SAMPLE_COUNT, src_FEATURE_COUNT, 1, 0);
    dump_data("sw_dst", (unsigned short*)gld_mem, dst_SAMPLE_COUNT, dst_FEATURE_COUNT, 1, 0);
    dump_data("hw_dst", (unsigned short*)dst_mem, dst_SAMPLE_COUNT, dst_FEATURE_COUNT, 1, 0);
#endif

#if 0
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
            printf("ERROR: [%d]: Accelerator hw %03X != sw %03X\n\r", i, dst_mem[i], gld_mem[i]);
            hw_errors++;
        }
    }
    printf("INFO: Total errors = %d (out of %d elements)\n\r", hw_errors, OUTPUT_N_ELEMENTS);
    if (hw_errors > 0)
        printf("INFO: Accelerator validation: FAIL\n\r");
    else
        printf("INFO: Accelerator validation: PASS!\n\r");

    printf("INFO: done!\n\r");
    printf("INFO: ===============================================\n\r");

    cleanup_platform();

    return 0;
}


