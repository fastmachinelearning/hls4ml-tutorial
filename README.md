# hls4ml-tutorial
Tutorial notebooks for `hls4ml`

There are several ways to run the tutorial notebooks:
## Online
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/fastmachinelearning/hls4ml-tutorial/HEAD)

## Conda
The Python environment used for the tutorials is specified in the `environment.yml` file.
It can be setup like:
```bash
conda env create -f environment.yml
conda activate hls4ml-tutorial
```

## Docker without Vivado
Pull the prebuilt image from DockerHub:
```bash
docker pull docker pull jmduarte/hls4ml-tutorial:hls4ml-0.7.0rc1
```

Follow these steps to build a Docker image that can be used locally, or on a JupyterHub instance.
You can build the image (without Vivado) straight from the GitHub:
```bash
docker build https://github.com/fastmachinelearning/hls4ml-tutorial -f docker/Dockerfile
```
Alternatively, you can clone the repository and build locally (you might want to do this to add Vivado installation, for example):
```bash
git clone https://github.com/fastmachinelearning/hls4ml-tutorial
cd hls4ml-tutorial
# modify something
docker build -f docker/Dockerfile -t hls4ml-tutorial .
```
Then to start the container:
```bash
docker run -p 8888:8888 hls4ml-tutorial
```
When the container starts, the Jupyter notebook server is started, and the link to open it in your browser is printed.

## Docker with Vivado
Pull the prebuilt image from DockerHub:
```bash
docker pull docker pull jmduarte/hls4ml-tutorial:hls4ml-0.7.0rc1-vivado-2019.2
```

To build the image with Vivado, you will need to edit `docker/install_vivado.sh` to use the Dropbox links instead of the local web server for downloading the Vivado installation files.
Then, you can build the image:
```bash
docker build -f docker/Dockerfile.vivado -t hls4ml-tutorial .
```

## Companion material
We have prepared a set of slides with some introduction and more details on each of the exercises.
Please find them [here](https://docs.google.com/presentation/d/1c4LvEc6yMByx2HJs8zUP5oxLtY6ACSizQdKvw5cg5Ck/edit?usp=sharing).
