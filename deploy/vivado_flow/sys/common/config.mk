help:
	@echo "INFO: make <TAB> for targets"
.PHONY: help

sys-gui:
	vivado -source tcl/script.tcl -mode gui
.PHONY: sys-gui

sys:
	vivado -source tcl/script.tcl -mode batch
.PHONY: sys

gui:
	vivado $(PROJECT)/$(PROJECT).xpr
.PHONY: gui

clean:
	rm -rf *.log *.txt *.jou *.str *_project NA
.PHONY: clean
