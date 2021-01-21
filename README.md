# hls4ml-tutorial

We adopt the `hls4ml-tutorial` notebooks for the course [CSEE E6868 Spring 2021](http://www.cs.columbia.edu/~cseesoc/esp_html) at Columbia University. Please remember to use the branch `csee-e6868-spring2021`. For additional information you can visit the [hls4ml](https://fastmachinelearning.org/hls4ml) official webpage.

You can set up your working space either on your local machine or the [CSEE E6868 Spring 2021](http://www.cs.columbia.edu/~cseesoc/esp_html) servers. Ask the instructors to get the credential for the latter.

<!-- vim-markdown-toc GFM -->

* [Getting Started](#getting-started)
    - [Xilinx Vivado HLS](#xilinx-vivado-hls)
    - [Jupyter Notebook Setup](#jupyter-notebook-setup)
        + [Install Miniconda](#install-miniconda)
        + [Create and Activate a Conda Environment](#create-and-activate-a-conda-environment)
        + [Run Jupyter Notebook](#run-jupyter-notebook)

<!-- vim-markdown-toc -->

## Getting Started

### Xilinx Vivado HLS

Make sure you have the [Xilinx Vivado Design Suite](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools.html) on your working machine. The path on the Columbia servers is `/opt/Xilinx/Vivado/2018.2/bin`.

A version of the design suite between ver. *2018.2* and *2019.2* has been tested. If you have more questions, please ask the instructors.

### Jupyter Notebook Setup

When you clone this repository, please remember to specify the course branch.

```
git clone -b csee-e6868-spring2021 git@github.com:fastmachinelearning/hls4ml-tutorial.git
```

#### Install Miniconda

Please follow these [instructions](https://docs.conda.io/en/latest/miniconda.html) to install Miniconda (Python 3.8).

#### Create and Activate a Conda Environment

The Conda environment used for the tutorial is specified in the `environment.yml` file. Creating the environment is a _on-time_ operation:
```
cd hls4ml-tutorial
conda env create -f environment.yml
```

In any new console, remember to activate the newly created environemnt:
```
conda activate hls4ml-tutorial-cu
```

#### Run Jupyter Notebook

Copy in your browser the URL that you obtain after running:
```
jupyter notebook
```
