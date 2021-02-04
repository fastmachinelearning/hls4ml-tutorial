BOARD=minized

TOP = jet_tagger
PROJECT = $(TOP)_baremetal_polling

help:
	@echo "INFO: make <TAB> to show targets"
.PHONY: help

setup:
	xsct script.tcl
.PHONY: setup

sdk: setup data
	rm -f $(PROJECT)/src/helloworld.c
	cd  $(PROJECT)/src && ln -s ../../../main.c
.PHONY: sdk

gui:
	xsdk --workspace .
.PHONY: gui

#SAMPLE_COUNT=166000
SAMPLE_COUNT=16

data:
	make -C ../../utils/dat2header/sim
	../../utils/dat2header/sim/dat2header ../../hls/$(BOARD)_prj/tb_data/tb_input_features.dat $(PROJECT)/src/src.h src $(SAMPLE_COUNT)
	../../utils/dat2header/sim/dat2header ../../hls/$(BOARD)_prj/tb_data/csim_results.log $(PROJECT)/src/dst.h dst $(SAMPLE_COUNT)
.PHONY: data

clean:
	rm -rf $(PROJECT)
	rm -rf $(PROJECT)_bsp
	rm -rf $(TOP)_design_wrapper_hw_platform_*
	rm -rf RemoteSystemsTempFiles
	rm -rf SDK.log
	rm -rf webtalk
	rm -rf .Xil
	rm -rf .metadata
.PHONY: clean
