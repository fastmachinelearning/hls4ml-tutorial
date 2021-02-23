#include <stdio.h>
#include "xparameters.h"
#include "platform.h"
#include "xil_printf.h"

#include "xiicps.h"

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

//#define I2C_DEBUG

#define I2C_CONTROLLER_ID XPAR_XIICPS_0_DEVICE_ID
#define I2C_CLK_RATE 400000
#define I2C_DA9062_ADDR 0x58

int i2c_init(XIicPs *i2c_ps, u16 controller_id, u32 fscl_hz) {

    int status;
    XIicPs_Config *i2c_config;

    /*
     * Initialize the I2C driver so that it's ready to use.
     * Look up the configuration in the config table,
     * then initialize it.
     */

    i2c_config = XIicPs_LookupConfig(controller_id);
    if (NULL == i2c_config) {
        return XST_FAILURE;
    }

    status = XIicPs_CfgInitialize(i2c_ps, i2c_config, i2c_config->BaseAddress);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Perform a self-test to ensure that the I2C hardware was built correctly
     */
    status = XIicPs_SelfTest(i2c_ps);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Set the I2C serial clock rate
     */
    XIicPs_SetSClk(i2c_ps, I2C_CLK_RATE);

    return XST_SUCCESS;
}

int i2c_read_reg(XIicPs *i2c_ps, u8 addr, u8 reg, u8 *reg_value)
{

	s32 status;
	u32 index;

    /* Buffer for transmitting and receiving data */
    u8 SendBuffer[8];
    u8 RecvBuffer[8];

    /*
     * Reset the buffers.
     */
    for (index = 0; index < 8; index++) {
        SendBuffer[index] = 0;
        RecvBuffer[index] = 0;
    }

    SendBuffer[0] = reg;

    /*
     * Wait until bus is idle to start another transfer
     */
    while (XIicPs_BusIsBusy(i2c_ps)) {
        /* NOP */
    }

    /*
     * Send the buffer using the I2C bus
     */
    status = XIicPs_MasterSendPolled(i2c_ps, SendBuffer, 1 /* bytes */, addr);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Wait until bus is idle to start another transfer
     */
    while (XIicPs_BusIsBusy(i2c_ps)) {
        /* NOP */
    }

    /*
     * Receive the buffer using the I2C bus
     */
    status = XIicPs_MasterRecvPolled(i2c_ps, RecvBuffer, 1, addr);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

#ifdef I2C_DEBUG
    for (index = 0; index < 8; index++) {
        xil_printf("DEBUG: [%u] = 0x%X\n\r", index, RecvBuffer[index]);
    }
#endif

    /*
     * Pass the value of the register to the caller
     */
    *reg_value = RecvBuffer[0];

    return XST_SUCCESS;
}

int i2c_read_regs(XIicPs *i2c_ps, u8 addr, u8 *regs, u8 *reg_values, u32 N)
{

	s32 status;
	u32 index;

	for (index = 0; index < N; index++) {
		u8 reg_value = 0;
		status = i2c_read_reg(i2c_ps, /*I2C_SI5328_ADDR*/ addr, regs[index], &reg_value);
	    if (status != XST_SUCCESS) {
	        return XST_FAILURE;
	    }
	    reg_values[index] = reg_value;
	}

    return XST_SUCCESS;
}


int main() {
	s32 status;
	u32 index;

    init_platform();

    /* Instance of the I2C driver */
    XIicPs i2c_ps;

    /* Step 1: Initialize I2C bus */
    status = i2c_init(&i2c_ps, I2C_CONTROLLER_ID, I2C_CLK_RATE);
    if (status != XST_SUCCESS) {
        xil_printf("ERROR: Step 1: Could not initialize I2C bus (ControllerID = %X, ClockRateHz = %u)\r\n", I2C_CONTROLLER_ID, (u32)I2C_CLK_RATE);
        return XST_FAILURE;
    }
    xil_printf("INFO: Step 1: Successful initialization of I2C bus (ControllerID = %X, ClockRateHz = %u)\r\n", I2C_CONTROLLER_ID, (u32)I2C_CLK_RATE);

    /*
     * Read status registers and extract relevant bits.
     */
    u8 reg_status_values[4];
    //u8 REGS[4] = {0x181, 0x182, 0x183, 0x184};
    u8 REGS[4] = {0x0, 0x0, 0x0, 0x0};
    status = i2c_read_regs(&i2c_ps, I2C_DA9062_ADDR, REGS, reg_status_values, 4);
    if (status != XST_SUCCESS) {
    	xil_printf("ERROR: Step 2: Could not read status registers from Si5328b (Si5328 address = 0x%X)\r\n", (u32)I2C_DA9062_ADDR);
    }
    for (index = 0; index < 4; index++) {
      	xil_printf("INFO: Step 2:   - reg[%2u] = %3u 0x%02X "BYTE_TO_BINARY_PATTERN"\r\n", index, REGS[index], reg_status_values[index], BYTE_TO_BINARY(reg_status_values[index]));
    }

    cleanup_platform();
    return 0;
}
