/*
 * This code is based on
 * - https://github.com/ATaylorCEngFIET/MicroZed-Chronicles/blob/master/main_part163.c
 * - https://www.xilinx.com/support/documentation/user_guides/ug585-Zynq-7000-TRM.pdf#page=684
 * - https://forums.xilinx.com/t5/Xcell-Daily-Blog-Archived/Adam-Taylor-s-MicroZed-Chronicles-Part-163-Zynq-Power-Management/ba-p/742116
 */
#define __INTERRUPT__

#include <stdlib.h>
#include <unistd.h>
#include "platform.h"
#include "xgpiops.h" /* drivers for GPIO configuration and use */
#include "xgpiops.h" /* drivers for GIC (General Interrupt Controller) configuration and use */
#include "xparameters.h" /* defines processor device IDs */
#include "xstatus.h"

#include "xil_misc_psreset_api.h"

#ifdef __INTERRUPT__
#include "xscugic.h" /* drivers for the interrupt controller */
#include "xil_exception.h" /* exception functions for the ARM Cortex-A9 processor */
#endif

/* PS GPIO driver. */
static XGpioPs gpio_ps;

/* Configuration settings of PS GPIO. */
static XGpioPs_Config *gpio_ps_config;

#if 0
static XScuGic my_Gic;
static void USR_button_ISR(void *CallBackRef);
#endif

#ifdef __INTERRUPT__
/* The instance of the interrupt controller driver */
static XScuGic interrupt_controller;
#endif

static const unsigned PS_LED_R_PIN = 52u;
static const unsigned PS_LED_G_PIN = 53u;

static unsigned PBSW_PIN = 0;

int test_leds(unsigned iterations) {
	int status;
	unsigned i;

	/* Initialize the PS GPIO Driver. */
	gpio_ps_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	status = XGpioPs_CfgInitialize(&gpio_ps, gpio_ps_config, gpio_ps_config->BaseAddr);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Set the direction of the GPIO pin and enable the output. */
	XGpioPs_SetDirectionPin(&gpio_ps, PS_LED_R_PIN, 0x1);
	XGpioPs_SetOutputEnablePin(&gpio_ps, PS_LED_R_PIN, 0x1);
	XGpioPs_SetDirectionPin(&gpio_ps, PS_LED_G_PIN, 0x1);
	XGpioPs_SetOutputEnablePin(&gpio_ps, PS_LED_G_PIN, 0x1);

	/* Write the desired output value to the GPIO pin. */
	xil_printf("INFO: Testing green and red LEDs...\n\r");
	for (i = 0; i < iterations; i++) {
		XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x0);
		XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x1);
		sleep(1);
		XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x1);
		XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x0);
		sleep(1);
	}
	XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x0);
	XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x0);
	xil_printf("INFO: Done! Turn off LEDs.\n\r");

	return XST_SUCCESS;
}

int test_pushbutton() {
	int status;

	/* Initialize the PS GPIO Driver. */
	gpio_ps_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	status = XGpioPs_CfgInitialize(&gpio_ps, gpio_ps_config, gpio_ps_config->BaseAddr);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Set the direction of the GPIO pin and enable the output. */
	XGpioPs_SetDirectionPin(&gpio_ps, PS_LED_R_PIN, 0x1); /* output pint */
	XGpioPs_SetOutputEnablePin(&gpio_ps, PS_LED_R_PIN, 0x1);
	XGpioPs_SetDirectionPin(&gpio_ps, PS_LED_G_PIN, 0x1); /* output pint */
	XGpioPs_SetOutputEnablePin(&gpio_ps, PS_LED_G_PIN, 0x1);
	XGpioPs_SetDirectionPin(&gpio_ps, PBSW_PIN, 0x0); /* input pint */

	/* Reset LEDs */
	XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x0);
	XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x0);

	xil_printf("INFO: Testing PS button. Push it 5 times...\n\r");
	int toggle = 0;
	int sw = 0;
	unsigned i;
	while (1) {
		/* Read the value from the GPIO pin. */
		sw = XGpioPs_ReadPin(&gpio_ps, PBSW_PIN);
		sleep(1); /* Simple debouncing implementation, just sleep. */

		if (sw == 1) {
			toggle = !toggle;
			i++;
			xil_printf("INFO:   %u\n\r", i);
		}

		XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, toggle);
		XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, !toggle);
		if (i >= 5) break;
	}
	xil_printf("INFO: Done! Turn off LEDs.\n\r");
	XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x0);
	XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x0);

	return XST_SUCCESS;
}

#ifdef __INTERRUPT__

#define l2cpl310_reg15 	0xF8F02F80
#define scu_control_reg 0xF8F00000
#define topsw_clk 		0xF800016C
#define slcr_unlock 	0xF8000008
#define ddrc_ctrl_reg1 	0xF8006060
#define ddrc_para_reg3 	0xF8006020
#define ddr_clk_ctrl 	0xF8000124
#define dci_clk_ctrl 	0xF8000128
#define arm_pll_ctrl 	0xF8000100
#define ddr_pll_ctrl 	0xF8000104
#define io_pll_ctrl  	0xF8000108
#define arm_clk_ctrl 	0xF8000120
#define gpio_int_en0 	0xE000A210
#define pll_status 		0xF800010C
#define uart_sel	    0xF8000154
#define aper_reg        0xF800012C

static void interrupt_handler_pushbutton(void *callback, int bank, u32 status) {
	static unsigned toggle = 0;

	u32 ddrc;

	XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, toggle);
	XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, !toggle);

#if 1
	if (toggle) {
		ddrc = 0x1177cb4; //enable standby mode and dynamic clock gating
		Xil_Out32(arm_clk_ctrl, ddrc);

		ddrc = Xil_In32(io_pll_ctrl);
		//printf("io_pll_ctrl = %x\n\r", (unsigned int) ddrc);
		ddrc &= ~0x00000002; //remove standby mode and dynamic clock gating
		Xil_Out32(io_pll_ctrl, ddrc);

		ddrc = Xil_In32(ddr_pll_ctrl);
		//printf("ddr_pll_ctrl = %x\n\r", (unsigned int) ddrc);
		ddrc &= ~0x00000002; //remove standby mode and dynamic clock gating
		Xil_Out32(ddr_pll_ctrl, ddrc);

		ddrc = Xil_In32(arm_pll_ctrl);
		//printf("arm_pll_ctrl = %x\n\r", (unsigned int) ddrc);
		ddrc &= ~0x00000002; //remove standby mode and dynamic clock gating
		Xil_Out32(arm_pll_ctrl, ddrc);

		ddrc = Xil_In32(pll_status);
		while(ddrc != 0x0000003f){ //wait for DLL to lock and be stable
			ddrc = Xil_In32(pll_status);
		}

		ddrc = Xil_In32(arm_pll_ctrl);
		//printf("arm_pll_ctrl = %x\n\r", (unsigned int) ddrc);
		ddrc &= ~0x00000010; //enable standby mode and dynamic clock gating
		Xil_Out32(arm_pll_ctrl, ddrc);
		ddrc = Xil_In32(arm_pll_ctrl);
		//printf("arm_pll_ctrl = %x\n\r", (unsigned int) ddrc);

		ddrc = Xil_In32(ddr_pll_ctrl);
		//printf("ddr_pll_ctrl = %x\n\r", (unsigned int) ddrc);
		ddrc &= ~0x00000010; //enable standby mode and dynamic clock gating
		Xil_Out32(ddr_pll_ctrl, ddrc);
		ddrc = Xil_In32(ddr_pll_ctrl);
		//printf("ddr_pll_ctrl = %x\n\r", (unsigned int) ddrc);

		ddrc = Xil_In32(io_pll_ctrl);
		ddrc &= ~0x00000010; //enable standby mode and dynamic clock gating
		Xil_Out32(io_pll_ctrl, ddrc);
		ddrc = Xil_In32(io_pll_ctrl);
	}
#endif

	toggle = !toggle;
}

static int setup_interrupt_system_pushbutton(XScuGic *interrupt_controller, XGpioPs *gpio_ps, u16 gpio_interrupt_id) {

	int status;

	/* Instance of the interrupt controller */
	XScuGic_Config *interrupt_controller_config;

	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	interrupt_controller_config = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (interrupt_controller_config == NULL) {
		return XST_FAILURE;
	}

	status = XScuGic_CfgInitialize(interrupt_controller, interrupt_controller_config, interrupt_controller_config->CpuBaseAddress);
    if (status != XST_SUCCESS) {
        xil_printf("INFO: GIC configuration failed!\n\r");
        return XST_FAILURE;
    }

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, interrupt_controller);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	status = XScuGic_Connect(interrupt_controller, gpio_interrupt_id, (Xil_ExceptionHandler)XGpioPs_IntrHandler, (void *)gpio_ps);
    if (status != XST_SUCCESS) {
        xil_printf("INFO: Interrupt handler connection failed!\n\r");
        return XST_FAILURE;
    }

	/*
	 * Enable  interrupts for all the pins in bank 0.
	 */
	/* XGpioPs_SetIntrTypePin(gpio_ps, PBSW_PIN, XGPIOPS_IRQ_TYPE_EDGE_RISING); */

    /* Enable level-edge interrupts for all the pins in bank 0 except pin 0,
     * where push button is connected.
     * For pin 0, we set raising-edge-triggered interrupt
     */
    XGpioPs_SetIntrType(gpio_ps, XGPIOPS_BANK0, 0x01, 0xFFFFFFFF, 0x00);

	/*
	 * Set the handler for GPIO interrupts.
	 */
	XGpioPs_SetCallbackHandler(gpio_ps, (void *)gpio_ps, interrupt_handler_pushbutton);

	/* Enable the GPIO interrupts of Bank 0. */
	/* XGpioPs_IntrEnablePin(gpio_ps, PBSW_PIN); */
	XGpioPs_IntrEnable(gpio_ps, XGPIOPS_BANK0, (1 << PBSW_PIN));


	/* Enable the interrupt for the GPIO device. */
	XScuGic_Enable(interrupt_controller, gpio_interrupt_id);

	/* Enable interrupts in the Processor. */
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	return XST_SUCCESS;
}

static int cleanup_interrupt_system_pushbutton(XScuGic *interrupt_controller, XGpioPs *gpio_ps, u16 gpio_interrupt_id) {

	int status;


    /*XGpioPs_IntrDisablePin(gpio_ps, (u32)PBSW_PIN);*/
    XGpioPs_IntrDisable(gpio_ps, XGPIOPS_BANK0, (1 << PBSW_PIN));

    /* usleep(1000000);*/
    XGpioPs_IntrClear(gpio_ps, 1,0xFFFFFFFF); //clear interrupts after delay to remove bounce

    /*XGpioPs_IntrEnable(gpio_ps, XGPIOPS_BANK0, (1 << PBSW_PIN));*/
    XGpioPs_IntrEnable(gpio_ps, XGPIOPS_BANK0, (1 << PBSW_PIN));

	return XST_SUCCESS;
}

int test_pushbutton_with_interrupt() {
	int status;

	/* Initialize the PS GPIO Driver. */
	gpio_ps_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	status = XGpioPs_CfgInitialize(&gpio_ps, gpio_ps_config, gpio_ps_config->BaseAddr);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Set the direction for the PS push button pin to be input */
	XGpioPs_SetDirectionPin(&gpio_ps, PBSW_PIN, 0x0);

	/* Set the direction for the red PS LED pin to be output. */
	XGpioPs_SetDirectionPin(&gpio_ps, PS_LED_R_PIN, 0x1); /* output pin */
	XGpioPs_SetOutputEnablePin(&gpio_ps, PS_LED_R_PIN, 0x1);
	XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x0);

	/* Set the direction for the green PS LED pin to be output. */
	XGpioPs_SetDirectionPin(&gpio_ps, PS_LED_G_PIN, 0x1); /* output pin */
	XGpioPs_SetOutputEnablePin(&gpio_ps, PS_LED_G_PIN, 0x1);
	XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x0);

	status = setup_interrupt_system_pushbutton(&interrupt_controller, &gpio_ps, XPAR_XGPIOPS_0_INTR);
	if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

	xil_printf("INFO: Testing PS button. Push it 5 times...\n\r");
	int i = 0;
	while (1) {

		__asm__("wfi");

		i++;
		xil_printf("INFO:   %u\n\r", i);

		if (i >= 5) break;
	}
	xil_printf("INFO: Done! Turn off LEDs.\n\r");
	XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x0);
	XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x0);

	status = cleanup_interrupt_system_pushbutton(&interrupt_controller, &gpio_ps, XPAR_XGPIOPS_0_INTR);
	if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

	return status;
}

int enter_sleep_mode() {

	/* Disable interrupts. */
	//cpsidf();

    /*
     * The System-Level Control registers (SLCR) consist of various registers
     * that are used to control the PS behavior. These registers are accessible
     * via the central interconnect using load and store instructions.
     * See: https://www.xilinx.com/support/documentation/user_guides/ug585-Zynq-7000-TRM.p  df#page=114
     */
    u32 data;

    /* Unlock SLCR registers which can be protected by default. */
    data = Xil_In32(XSLCR_UNLOCK_ADDR);
    Xil_Out32(XSLCR_UNLOCK_ADDR, XSLCR_UNLOCK_CODE);


	/* Configure wake-up device. */

	/* Enable L2 cache dynamic clock gating. */

	/* Enable SCU standby mode. */

	/* Enable topswitch clock stop. */

	/* Enable Cortex-A9 dynamic clock gating. */

	/* Put the external DDR memory into self-refresh mode. */

	/* Put the PLLs into bypass mode. */

	/* Shut down the PLLs. */

	/* Increase the clock divisor to slow down the CPU clock. */



#if 1
    data = Xil_In32(aper_reg);  //clock gate unused peripherals
    xil_printf("aper_reg = %x\n\r", (unsigned int) data);
    data = 0x1600001;
    Xil_Out32(aper_reg,data);
    data = Xil_In32(aper_reg);
    xil_printf("aper_reg = %x\n\r", (unsigned int) data);

    data = Xil_In32(arm_clk_ctrl);  //clock gate unused peripherals
    xil_printf("arm_clk_ctrl = %x\n\r", (unsigned int) data);

    //data = Xil_In32(uart_sel);
    //xil_printf("uart_sel = %x\n\r", (unsigned int) data);
    //data |= 0x00000030;
    //Xil_Out32(uart_sel,data);
    data = Xil_In32(uart_sel);
    xil_printf("uart_sel = %x\n\r", (unsigned int) data);
    usleep(1000000);
    data = Xil_In32(arm_clk_ctrl);
    xil_printf("arm_clk_ctrl = %x\n\r", (unsigned int) data);
    //data = Xil_In32(gpio_int_en0);
    data = 0x00000400;
    Xil_Out32(gpio_int_en0,data);
    data = Xil_In32(gpio_int_en0);
    xil_printf("gpio_int_en0 = %x\n\r", (unsigned int) data);
    usleep(1000000);

    //configure cache for low power mode
    data = Xil_In32(l2cpl310_reg15);
    xil_printf("ls cache reg 15 power = %x\n\r", (unsigned int) data);
    data = 0x3; //enable standby mode and dynamic clock gating
    Xil_Out32(l2cpl310_reg15,data);
    data = Xil_In32(l2cpl310_reg15);
    xil_printf("ls cache reg 15 power = %x\n\r", (unsigned int) data);
    usleep(1000000);

    //configure scu
    data = Xil_In32(scu_control_reg);
    xil_printf("scu control reg = %x\n\r", (unsigned int) data);
    data |=  0x00000020; //enable standby mode and dynamic clock gating
    Xil_Out32(scu_control_reg,data);
    data = Xil_In32(scu_control_reg);
    xil_printf("scu control reg = %x\n\r", (unsigned int) data);
    usleep(1000000);

    //configure slcr
    data = Xil_In32(topsw_clk);
    xil_printf("slcr control reg = %x\n\r", (unsigned int) data);
    data |=  0x00000001; //enable standby mode and dynamic clock gating
    Xil_Out32(topsw_clk,data);
    data = Xil_In32(topsw_clk);
    xil_printf("slcr control reg = %x\n\r", (unsigned int) data);
    usleep(1000000);

    //set CP15
    data = mfcp(XREG_CP15_POWER_CTRL);
    xil_printf("cp15 Reg = %x\n\r", (unsigned int) data);

    mtcp(XREG_CP15_POWER_CTRL,0x701);
    data = mfcp(XREG_CP15_POWER_CTRL);
    xil_printf("cp15 Reg = %x\n\r", (unsigned int) data);
    usleep(1000000);

    //set up ddr
    data = Xil_In32(ddrc_ctrl_reg1);
    data |= 0x00001000; //enable standby mode and dynamic clock gating
    Xil_Out32(ddrc_ctrl_reg1,data);

    data = Xil_In32(ddrc_para_reg3);
    data |= 0x00100000; //enable standby mode and dynamic clock gating
    Xil_Out32(ddrc_para_reg3,data);

    data = Xil_In32(ddr_clk_ctrl);
    data &= 0xFFFFFFF0; //enable standby mode and dynamic clock gating
    Xil_Out32(ddr_clk_ctrl,data);

    data = Xil_In32(dci_clk_ctrl);
    data &= 0xFFFFFFF0; //enable standby mode and dynamic clock gating
    Xil_Out32(dci_clk_ctrl,data);

    //	arm_pll_ctrl
    data = Xil_In32(arm_pll_ctrl);
    data |= 0x00000010; //enable standby mode and dynamic clock gating
    Xil_Out32(arm_pll_ctrl,data);

    data = Xil_In32(ddr_pll_ctrl);
    data |= 0x00000010; //enable standby mode and dynamic clock gating
    Xil_Out32(ddr_pll_ctrl,data);

    data = Xil_In32(io_pll_ctrl);
    data |= 0x00000010; //enable standby mode and dynamic clock gating
    Xil_Out32(io_pll_ctrl,data);

    data = Xil_In32(arm_pll_ctrl);
    data |= 0x00000002; //enable standby mode and dynamic clock gating
    Xil_Out32(arm_pll_ctrl,data);

    data = Xil_In32(ddr_pll_ctrl);
    data |= 0x00000002; //enable standby mode and dynamic clock gating
    Xil_Out32(ddr_pll_ctrl,data);

    data = Xil_In32(io_pll_ctrl);
    data |= 0x00000002; //enable standby mode and dynamic clock gating
    Xil_Out32(io_pll_ctrl,data);

    data = Xil_In32(arm_clk_ctrl);
    data |= 0x0003f00; //enable standby mode and dynamic clock gating
    Xil_Out32(arm_clk_ctrl,data);
#endif
	return XST_SUCCESS;
}

int test_sleep_mode() {
	int status;

	/* Initialize the PS GPIO Driver. */
	gpio_ps_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	status = XGpioPs_CfgInitialize(&gpio_ps, gpio_ps_config, gpio_ps_config->BaseAddr);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Set the direction for the PS push button pin to be input */
	XGpioPs_SetDirectionPin(&gpio_ps, PBSW_PIN, 0x0);

	/* Set the direction for the red PS LED pin to be output. */
	XGpioPs_SetDirectionPin(&gpio_ps, PS_LED_R_PIN, 0x1); /* output pin */
	XGpioPs_SetOutputEnablePin(&gpio_ps, PS_LED_R_PIN, 0x1);
	XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x0);

	/* Set the direction for the green PS LED pin to be output. */
	XGpioPs_SetDirectionPin(&gpio_ps, PS_LED_G_PIN, 0x1); /* output pin */
	XGpioPs_SetOutputEnablePin(&gpio_ps, PS_LED_G_PIN, 0x1);
	XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x0);

	status = setup_interrupt_system_pushbutton(&interrupt_controller, &gpio_ps, XPAR_XGPIOPS_0_INTR);
	if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

	xil_printf("INFO: Testing PS button. Push it 5 times...\n\r");
	int i = 0;
	while (1) {

		enter_sleep_mode();

		/* Execute the wfi instruction to enter WFI mode. */
		__asm__("wfi");

		i++;
		xil_printf("INFO:   %u\n\r", i);

		if (i >= 5) break;
	}
	xil_printf("INFO: Done! Turn off LEDs.\n\r");
	XGpioPs_WritePin(&gpio_ps, PS_LED_R_PIN, 0x0);
	XGpioPs_WritePin(&gpio_ps, PS_LED_G_PIN, 0x0);

	status = cleanup_interrupt_system_pushbutton(&interrupt_controller, &gpio_ps, XPAR_XGPIOPS_0_INTR);
	if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

	return status;
}

#endif

int main() {

	int status;

    init_platform();

    status = test_leds(10);
    if (status != XST_SUCCESS) {
    	xil_printf("ERROR: LED test failed!\n\r");
    }

#ifdef __INTERRUPT__
#if 0
    status = test_pushbutton_with_interrupt();
    if (status != XST_SUCCESS) {
    	xil_printf("ERROR: Pushbutton with interrupt test failed!\n\r");
    }
#else
    status = test_sleep_mode();
    if (status != XST_SUCCESS) {
    	xil_printf("ERROR: Pushbutton with interrupt test failed!\n\r");
    }
#endif
#else
    status = test_pushbutton();
    if (status != XST_SUCCESS) {
    	xil_printf("ERROR: Pushbutton test failed!\n\r");
    }
#endif

    cleanup_platform();

    xil_printf("INFO: Done!\n\r");

    return 0;
}
