# hls4ml-tutorial: Tutorial notebooks for `hls4ml`


[![Jupyter Book Badge](https://jupyterbook.org/badge.svg)](https://fastmachinelearning.org/hls4ml-tutorial)
![deploy-book](https://github.com/fastmachinelearning/hls4ml-tutorial/actions/workflows/deploy.yml/badge.svg)
[![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/fastmachinelearning/hls4ml-tutorial)


There are several ways to run the tutorial notebooks:
## Online
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/fastmachinelearning/hls4ml-tutorial/HEAD)

## Conda
Running the tutorials requires AMD Vitis HLS to be installed, see [here](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html).
After the installation, the necessary environmental variables can be set using
```
source /path/to/your/installtion/Xilinx/Vitis_HLS/202X.X/settings64.(c)sh
```

The Python environment used for the tutorials is specified in the `environment.yml` file.
It can be setup like:
```bash
conda env create -f environment.yml
conda activate hls4ml-tutorial
source /path/to/your/installtion/Xilinx/Vitis_HLS/202X.X/settings64.(c)sh
```

## Accelerator backends (Part 5)
Part 5 uses the **VitisUnified** backend of hls4ml, which produces a bitstream and a PYNQ driver for a SoC board (zcu102 or kv260).
It requires Vitis and Vivado 2023.2 (Vivado for the platform build and place-and-route):
```bash
source /path/to/your/installtion/Xilinx/Vitis/2023.2/settings64.(c)sh
```
The backend is not merged into the main branch of hls4ml yet. Install it from pull request
[#1376](https://github.com/fastmachinelearning/hls4ml/pull/1376) on top of the environment above:
```bash
pip install git+https://github.com/fastmachinelearning/hls4ml.git@refs/pull/1376/head
```

## Companion material
We have prepared a set of slides with some introduction and more details on each of the exercises.
Please find them [here](https://docs.google.com/presentation/d/1c4LvEc6yMByx2HJs8zUP5oxLtY6ACSizQdKvw5cg5Ck/edit?usp=sharing).


## Notebooks
```{tableofcontents}
```
