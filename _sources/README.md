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

Note that part 7 of the tutorial makes use of the `VivadoAccelator` backend of hls4ml for which no Vitis equivalent is available yet. For this part of the tutorial it is therefore necesary to install and source Vivado HLS version 2019.2 or 2020.1, which can be obtained [here](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/archive.html).

## Companion material
We have prepared a set of slides with some introduction and more details on each of the exercises.
Please find them [here](https://docs.google.com/presentation/d/1c4LvEc6yMByx2HJs8zUP5oxLtY6ACSizQdKvw5cg5Ck/edit?usp=sharing).


## Notebooks
```{tableofcontents}
```
