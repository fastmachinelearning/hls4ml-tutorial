# hls4ml-tutorial

<!-- vim-markdown-toc GFM -->

* [Getting Started](#getting-started)
    - [Xilinx Vivado HLS](#xilinx-vivado-hls)
    - [Jupyter Notebook Setup](#jupyter-notebook-setup)
        + [Install Miniconda](#install-miniconda)
        + [Create and Activate a Conda Environment](#create-and-activate-a-conda-environment)
        + [Update `hls4ml` Package in the Conda Environment](#update-hls4ml-package-in-the-conda-environment)
        + [Run Jupyter Notebook](#run-jupyter-notebook)
    - [Running Jupyter Notebook on a Remote Server](#running-jupyter-notebook-on-a-remote-server)
    - [Update Tutorial Repository](#update-tutorial-repository)

<!-- vim-markdown-toc -->

We adopt the `hls4ml-tutorial` notebooks for the course [CSEE E6868 Spring 2021](http://www.cs.columbia.edu/~cseesoc/esp_html) at Columbia University. Please remember to use the branch `csee-e6868-spring2021`. For additional information you can visit the [hls4ml](https://fastmachinelearning.org/hls4ml) official webpage.

You can set up your working space either on your local machine or the [CSEE E6868 Spring 2021](http://www.cs.columbia.edu/~cseesoc/esp_html) servers. Ask the instructors to get the credential for the latter.

## Online
[![hls4ml](https://img.shields.io/badge/launch-hls4ml%20hub-%2359188e)](http://cern.ch/ssummers/hls4ml-tutorial)
[![badge](https://img.shields.io/badge/launch-CERN%20binder-E66581.svg?logo=data:image/png;base64,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)](https://binder.cern.ch/v2/gh/hls-fpga-machine-learning/hls4ml-tutorial/HEAD)
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/hls-fpga-machine-learning/hls4ml-tutorial/HEAD)


## Getting Started

### Xilinx Vivado HLS

Make sure you have the [Xilinx Vivado Design Suite](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools.html) on your working machine. The path on the Columbia servers is `/opt/Xilinx/Vivado/2018.2/bin`.

A version of the design suite between ver. *2018.2* and *2019.2* has been tested. If you have more questions, please ask the instructors.

### Jupyter Notebook Setup

When you clone this repository, please remember to specify the course branch.

```
git clone --recursive -b csee-e6868-spring2021 https://github.com/fastmachinelearning/hls4ml-tutorial.git
```

#### Install Miniconda

Please follow these [instructions](https://docs.conda.io/en/latest/miniconda.html) to install Miniconda (Python 3.8).

```
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh
```

#### Create and Activate a Conda Environment

The Conda environment used for the tutorial is specified in the `environment.yml` file. Creating the environment is a _on-time_ operation:
```
cd hls4ml-tutorial
conda env create -f environment.yml
```

You should close the terminal and open a new one to make sure that Conda is correctly setup in your environment.

In any new console, remember to activate the newly created environemnt:
```
conda activate hls4ml-tutorial-cu
```

#### Update `hls4ml` Package in the Conda Environment

Because the `hls4ml` project is constantly evolving, it may be necessary to update the package in your Conda environment. The easiest way is:

```
conda activate hls4ml-tutorial-cu
pip uninstall hls4ml
pip install git+https://github.com/GiuseppeDiGuglielmo/hls4ml.git@gdg/cosmetics#egg=hls4ml[profiling]
```

[Here](https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html) you can find more instructions on how to create and manage a Conda environment.

#### Run Jupyter Notebook

Copy in your browser the URL that you obtain after running:
```
jupyter notebook
```

### Running Jupyter Notebook on a Remote Server

This is the case you have a `<remote_host>` that you can use, e.g., a server different from the ones provided for the course and with many CPU cores/threads for running Design Space Exploration. The Columbia servers may run a local firewall, ask the instructors if you want to run the notebook this way.

1. Launch Jupyter Notebook from remote server using port `8080`, for example:
   ```
   ssh <remote_user>@<remote_host>
   jupyter notebook --no-browser --port=8080
   ```
2. You can access the notebook from your remote machine over SSH by setting up a SSH tunnel. Run the following command from your local machine:
   ```
   ssh -4 -L 8080:localhost:<port> <remote_user>@<remote_host>
   ```
3. Open a browser from your local machine and navigate to `http://localhost:8080`, the Jupyter Notebook web interface. Replace 8080 with your port number used in step 1.

### Update Tutorial Repository

If you have previously cloned the tutorial repository, you may need to get the latest versions of the notebooks.

First check the status of your repository:
```
cd hls4ml-tutorial
make clean
git status 
```

You may have some _modified_ notebooks. For example:

```
# On branch csee-e6868-spring2021
# Changes not staged for commit:
#   (use "git add <file>..." to update what will be committed)
#   (use "git checkout -- <file>..." to discard changes in working directory)
#
#   modified:   part1_getting_started.ipynb
#   modified:   part2_advanced_config.ipynb
#   modified:   part2b_advanced_config.ipynb
#
no changes added to commit (use "git add" and/or "git commit -a")
```

You can make a copy of those modified notebooks if you had significat changes, otherwise the easiest thing to do is to discard those changes.

**ATTENTION** You will loose your local changes!

```
git checkout *.ipynb
```

At this point, you can update you copy of the repository:
```
git pull
```
