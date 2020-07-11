# hls4ml-tutorial
Tutorial notebooks for hls4ml 

The Python environment used for the tutorials is specified in the `environment.yml` file. It can be setup like:
```
conda env create -f environment.yml
conda activate hls4ml-tutorial
```

The second step to create a "Xilinx" Jupyter kernel is (assuming Vivado is set up):
```
python setup_kernel.py
```

The tutorial was tested with hls4ml master (hls-fpga-machine-learning/hls4ml@e9d0576) and qkeras master (google/qkeras@a1fb541).
