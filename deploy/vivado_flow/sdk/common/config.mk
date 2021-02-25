PROJECT_HLS=$(BOARD)_m_axi_16_serial_prj

PROJECT = $(TOP)_standalone
PROJECT_I2C = i2c_test

help:
	@echo "INFO: make <TAB> to show targets"
.PHONY: help

setup:
	xsct script.tcl
.PHONY: setup

sdk: setup data
	rm -f $(PROJECT)/src/helloworld.c
	cd  $(PROJECT)/src && ln -s ../../../common/main.c
	#rm -f $(PROJECT_I2C)/src/helloworld.c
	#cd  $(PROJECT_I2C)/src && ln -s ../../../main_i2c_test.c
.PHONY: sdk

sdk-irq: setup data
	rm -f $(PROJECT)/src/helloworld.c
	cd  $(PROJECT)/src && ln -s ../../../common/main_irq.c main.c
	#rm -f $(PROJECT_I2C)/src/helloworld.c
	#cd  $(PROJECT_I2C)/src && ln -s ../../../main_i2c_test.c
.PHONY: sdk-irq

gui:
	xsdk --workspace .
.PHONY: gui

SAMPLE_COUNT=64
#SAMPLE_COUNT=166000

data:
	make -C ../../utils/dat2header/sim
	../../utils/dat2header/sim/dat2header ../../hls/$(PROJECT_HLS)/tb_data/tb_input_features.dat $(PROJECT)/src/src.h src $(SAMPLE_COUNT)
	../../utils/dat2header/sim/dat2header ../../hls/$(PROJECT_HLS)/tb_data/csim_results.log $(PROJECT)/src/dst.h dst $(SAMPLE_COUNT)
.PHONY: data

clean:
	rm -rf $(PROJECT)
	rm -rf $(PROJECT)_bsp
	rm -rf $(PROJECT_I2C)
	rm -rf $(PROJECT_I2C)_bsp
	rm -rf $(TOP)_platform
	rm -rf RemoteSystemsTempFiles
	rm -rf SDK.log
	rm -rf webtalk
	rm -rf .sdk
	rm -rf .Xil
	rm -rf .metadata
.PHONY: clean
