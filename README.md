# hls4ml-tutorial
Tutorial notebooks for `hls4ml`

Build a Docker image that can be used locally, or on a JupyterHub instance
The Dockerfile was obtained from repo2docker, then modified
`docker build --build-arg NB_USER=jovyan --build-arg NB_UID=1000 .`
Then to start the container:
`docker run -p 8888:8888 <IMAGE ID>`
When the container starts, the Jupyter notebook server is started, and the link to open it in your browser is printed.
