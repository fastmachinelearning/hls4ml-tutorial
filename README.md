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
