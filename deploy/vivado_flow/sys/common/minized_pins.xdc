#
# See: https://github.com/Avnet/hdl/tree/master/Boards/MINIZED
#

########################################################################
## Arduino 8-pin connector
########################################################################
## To ARD_D0 on Arduino 8-pin  Pin 1
#set_property PACKAGE_PIN R8 [get_ports ARD_DAT0]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT0]
#
## To ARD_D1 on Arduino 8-pin  Pin 2
#set_property PACKAGE_PIN P8 [get_ports ARD_DAT1]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT1]
#
## To ARD_D2 on Arduino 8-pin  Pin 3
#set_property PACKAGE_PIN P9 [get_ports ARD_DAT2]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT2]
#
## To ARD_D3 on Arduino 8-pin  Pin 4
#set_property PACKAGE_PIN R7 [get_ports ARD_DAT3]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT3]
#
## To ARD_D4 on Arduino 8-pin  Pin 5
#set_property PACKAGE_PIN N7 [get_ports ARD_DAT4]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT4]
#
## To ARD_D5 on Arduino 8-pin  Pin 6
#set_property PACKAGE_PIN R10 [get_ports ARD_DAT5]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT5]
#
## To ARD_D6 on Arduino 8-pin  Pin 7
#set_property PACKAGE_PIN P10 [get_ports ARD_DAT6]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT6]
#
## To ARD_D7 on Arduino 8-pin  Pin 8
#set_property PACKAGE_PIN N8 [get_ports ARD_DAT7]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT7]
#
########################################################################
## Arduino 10-pin connector
########################################################################
## To ARD_D8 on Arduino 10-pin  Pin 1
#set_property PACKAGE_PIN M9 [get_ports ARD_DAT8]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT8]
#
## To ARD_D9 on Arduino 10-pin  Pin 2
#set_property PACKAGE_PIN N9 [get_ports ARD_DAT9]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT9]
#
## To ARD_D10 on Arduino 10-pin  Pin 3
#set_property PACKAGE_PIN M10 [get_ports ARD_DAT10]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT10]
#
## To ARD_D11 on Arduino 10-pin  Pin 4
#set_property PACKAGE_PIN M11 [get_ports ARD_DAT11]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT11]
#
## To ARD_D12 on Arduino 10-pin  Pin 5
#set_property PACKAGE_PIN R11 [get_ports ARD_DAT12]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT12]
#
## To ARD_D13 on Arduino 10-pin  Pin 6
#set_property PACKAGE_PIN P11 [get_ports ARD_DAT13]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_DAT13]
#
########################################################################
## Arduino 6-pin connector
########################################################################
## To ARD_A0 on Arduino 6-pin  Pin 6
#set_property PACKAGE_PIN F14 [get_ports ARD_ADDR0]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_ADDR0]
#
## To ARD_A1 on Arduino 6-pin  Pin 5
#set_property PACKAGE_PIN F13 [get_ports ARD_ADDR1]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_ADDR1]
#
## To ARD_A2 on Arduino 6-pin  Pin 4
#set_property PACKAGE_PIN F12 [get_ports ARD_ADDR2]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_ADDR2]
#
## To ARD_A3 on Arduino 6-pin  Pin 3 (and PL_LED Green)
##set_property PACKAGE_PIN E13 [get_ports ARD_ADDR3]
##set_property IOSTANDARD LVCMOS33 [get_ports ARD_ADDR3]
#set_property PACKAGE_PIN E13 [get_ports PL_LED_G]
#set_property IOSTANDARD LVCMOS33 [get_ports PL_LED_G]
#
## To ARD_A4 on Arduino 6-pin  Pin 2 (and PL_LED Red)
##set_property PACKAGE_PIN E12 [get_ports ARD_ADDR4]
##set_property IOSTANDARD LVCMOS33 [get_ports ARD_ADDR4]
#set_property PACKAGE_PIN E12 [get_ports PL_LED_R]
#set_property IOSTANDARD LVCMOS33 [get_ports PL_LED_R]
#
## To ARD_A5 on Arduino 6-pin  Pin 1 (and PL_SW)
##set_property PACKAGE_PIN E11 [get_ports ARD_ADDR5]
##set_property IOSTANDARD LVCMOS33 [get_ports ARD_ADDR5]
##set_property PACKAGE_PIN E11 [get_ports PL_SW]
##set_property IOSTANDARD LVCMOS33 [get_ports PL_SW]
#
########################################################################
## I2C
########################################################################
## To SDA on Arduino 10-pin  Pin 9 and Motion Sensor
##set_property PACKAGE_PIN F15 [get_ports I2C_SDA]
##set_property IOSTANDARD LVCMOS33 [get_ports I2C_SDA]
#set_property PACKAGE_PIN F15 [get_ports iic_rtl_sda_io]
#set_property IOSTANDARD LVCMOS33 [get_ports iic_rtl_sda_io]
#
## To SCL on Arduino 10-pin  Pin 10 and Motion Sensor
##set_property PACKAGE_PIN G15 [get_ports I2C_SCL]
##set_property IOSTANDARD LVCMOS33 [get_ports I2C_SCL]
#set_property PACKAGE_PIN G15 [get_ports iic_rtl_scl_io]
#set_property IOSTANDARD LVCMOS33 [get_ports iic_rtl_scl_io]
#
########################################################################
## Pmod #1
########################################################################
#set_property PACKAGE_PIN L15 [get_ports PMOD1_PIN1]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD1_PIN1]
#
#set_property PACKAGE_PIN M15 [get_ports PMOD1_PIN2]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD1_PIN2]
#
#set_property PACKAGE_PIN L14 [get_ports PMOD1_PIN3]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD1_PIN3]
#
#set_property PACKAGE_PIN M14 [get_ports PMOD1_PIN4]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD1_PIN4]
#
#set_property PACKAGE_PIN K13 [get_ports PMOD1_PIN7]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD1_PIN7]
#
#set_property PACKAGE_PIN L13 [get_ports PMOD1_PIN8]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD1_PIN8]
#
#set_property PACKAGE_PIN N13 [get_ports PMOD1_PIN9]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD1_PIN9]
#
#set_property PACKAGE_PIN N14 [get_ports PMOD1_PIN10]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD1_PIN10]
#
########################################################################
## Pmod #2
########################################################################
#set_property PACKAGE_PIN P13 [get_ports PMOD2_PIN1]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD2_PIN1]
#
#set_property PACKAGE_PIN P14 [get_ports PMOD2_PIN2]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD2_PIN2]
#
#set_property PACKAGE_PIN N11 [get_ports PMOD2_PIN3]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD2_PIN3]
#
#set_property PACKAGE_PIN N12 [get_ports PMOD2_PIN4]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD2_PIN4]
#
#set_property PACKAGE_PIN P15 [get_ports PMOD2_PIN7]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD2_PIN7]
#
#set_property PACKAGE_PIN R15 [get_ports PMOD2_PIN8]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD2_PIN8]
#
#set_property PACKAGE_PIN R12 [get_ports PMOD2_PIN9]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD2_PIN9]
#
#set_property PACKAGE_PIN R13 [get_ports PMOD2_PIN10]
#set_property IOSTANDARD LVCMOS33 [get_ports PMOD2_PIN10]
#
########################################################################
## Microphone
########################################################################
#set_property PACKAGE_PIN L12 [get_ports AUDIO_CLK]
#set_property IOSTANDARD LVCMOS33 [get_ports AUDIO_CLK]
#
#set_property PACKAGE_PIN M12 [get_ports AUDIO_DAT]
#set_property IOSTANDARD LVCMOS33 [get_ports AUDIO_DAT]

#######################################################################
# Wireless Module
#######################################################################

#set_property PACKAGE_PIN J15 [get_ports WL_SDIO_CLK]
#set_property IOSTANDARD LVCMOS33 [get_ports WL_SDIO_CLK]
#
#set_property PACKAGE_PIN J11 [get_ports WL_SDIO_CMD]
#set_property IOSTANDARD LVCMOS33 [get_ports WL_SDIO_CMD]
#
#set_property PACKAGE_PIN J13 [get_ports {WL_SDIO_DAT[0]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {WL_SDIO_DAT[0]}]
#
#set_property PACKAGE_PIN H11 [get_ports {WL_SDIO_DAT[1]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {WL_SDIO_DAT[1]}]
#
#set_property PACKAGE_PIN K15 [get_ports {WL_SDIO_DAT[2]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {WL_SDIO_DAT[2]}]
#
#set_property PACKAGE_PIN J14 [get_ports {WL_SDIO_DAT[3]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {WL_SDIO_DAT[3]}]

set_property PACKAGE_PIN K11 [get_ports WL_REG_ON]
set_property IOSTANDARD LVCMOS33 [get_ports WL_REG_ON]

#set_property PACKAGE_PIN K12 [get_ports WL_HOST_WAKE]
#set_property IOSTANDARD LVCMOS33 [get_ports WL_HOST_WAKE]
#
#set_property PACKAGE_PIN G14 [get_ports BT_TXD_OUT]
#set_property IOSTANDARD LVCMOS33 [get_ports BT_TXD_OUT]
#
#set_property PACKAGE_PIN G11 [get_ports BT_RXD_IN]
#set_property IOSTANDARD LVCMOS33 [get_ports BT_RXD_IN]
#
#set_property PACKAGE_PIN H12 [get_ports BT_RTS_OUT_N]
#set_property IOSTANDARD LVCMOS33 [get_ports BT_RTS_OUT_N]
#
#set_property PACKAGE_PIN G12 [get_ports BT_CTS_IN_N]
#set_property IOSTANDARD LVCMOS33 [get_ports BT_CTS_IN_N]

set_property PACKAGE_PIN H13 [get_ports BT_REG_ON]
set_property IOSTANDARD LVCMOS33 [get_ports BT_REG_ON]

#set_property PACKAGE_PIN H14 [get_ports BT_HOST_WAKE]
#set_property IOSTANDARD LVCMOS33 [get_ports BT_HOST_WAKE]
