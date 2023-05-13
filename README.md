# hls4ml-tutorial: Tutorial notebooks for `hls4ml`


[![Jupyter Book Badge](https://jupyterbook.org/badge.svg)](https://fastmachinelearning.org/hls4ml-tutorial)
![deploy-book](https://github.com/fastmachinelearning/hls4ml-tutorial/actions/workflows/deploy.yml/badge.svg)
[![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/fastmachinelearning/hls4ml-tutorial)


There are several ways to run the tutorial notebooks:
## Online
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/fastmachinelearning/hls4ml-tutorial)

## Conda
The Python environment used for the tutorials is specified in the `environment.yml` file.
It can be setup like:
```bash
conda env create -f environment.yml
conda activate hls4ml-tutorial
```

## Docker without Vivado
Pull the prebuilt image from the GitHub Container Registry:
```bash
docker pull ghcr.io/fastmachinlearning/hls4ml-tutorial/hls4ml-0.7.0:latest
```

Follow these steps to build a Docker image that can be used locally, or on a JupyterHub instance.
You can build the image (without Vivado):
```bash
docker build https://github.com/fastmachinelearning/hls4ml-tutorial -f docker/Dockerfile
```
Alternatively, you can clone the repository and build locally:
```bash
git clone https://github.com/fastmachinelearning/hls4ml-tutorial
cd hls4ml-tutorial
docker build -f docker/Dockerfile -t ghcr.io/fastmachinlearning/hls4ml-tutorial/hls4ml-0.7.0:latest .
```
Then to start the container:
```bash
docker run -p 8888:8888 ghcr.io/fastmachinlearning/hls4ml-tutorial/hls4ml-0.7.0:latest
```
When the container starts, the Jupyter notebook server is started, and the link to open it in your browser is printed.
You can clone the repository inside the container and run the notebooks.

## Docker with Vivado
Pull the prebuilt image from the GitHub Container Registry:
```bash
docker pull ghcr.io/fastmachinlearning/hls4ml-tutorial/hls4ml-0.7.0-vivado-2019.2:latest
```

To build the image with Vivado, run (Warning: takes a long time and requires a lot of disk space):
```bash
docker build -f docker/Dockerfile.vivado -t ghcr.io/fastmachinlearning/hls4ml-tutorial:hls4ml-0.7.0-vivado-2019.2:latest .
```
Then to start the container:
```bash
docker run -p 8888:8888 ghcr.io/fastmachinlearning/hls4ml-tutorial/hls4ml-0.7.0-vivado-2019.2:latest
```

## Companion material
We have prepared a set of slides with some introduction and more details on each of the exercises.
Please find them [here](https://docs.google.com/presentation/d/1c4LvEc6yMByx2HJs8zUP5oxLtY6ACSizQdKvw5cg5Ck/edit?usp=sharing).


## Notebooks
```{tableofcontents}
```
