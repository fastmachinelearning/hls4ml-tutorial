all:
	@echo "INFO: make <TAB> for targets"
.PHONY: all

clean:
	@rm -f *.npy *.png
	@rm -rf model_1
	@rm -rf .ipynb_checkpoints
	@rm -rf __pycache__
.PHONY: clean
