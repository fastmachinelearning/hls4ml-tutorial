# hls4ml-tutorial
Tutorial notebooks for `hls4ml`

The Python environment used for the tutorials is specified in the `environment.yml` file. It can be setup like:
```
conda env create -f environment.yml
conda activate hls4ml-tutorial
```

## Docker
Follow these steps to build a Docker image that can be used locally, or on a JupyterHub instance, e.g. as the [single user image](https://zero-to-jupyterhub.readthedocs.io/en/latest/customizing/user-environment.html#choose-and-use-an-existing-docker-image).
The Dockerfile was modified from one created using `repo2docker`.
You can build the image straight from the Github:
`docker build --build-arg NB_USER=jovyan --build-arg NB_UID=1000 https://github.com/hls-fpga-machine-learning/hls4ml-tutorial.git -f docker/Dockerfile`
Alternatively, you can clone the repository and build locally (you might want to do this to add Vivado installation, for example):
```
git clone https://github.com/hls-fpga-machine-learning/hls4ml-tutorial.git
cd hls4ml-tutorial
# modify something
docker build --build-arg NB_USER=jovyan --build-arg NB_UID=1000 . -f docker/Dockerfile
```
Then to start the container:
`docker run -p 8888:8888 <IMAGE ID>`
When the container starts, the Jupyter notebook server is started, and the link to open it in your browser is printed.
