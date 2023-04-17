FROM jupyter/tensorflow-notebook:tensorflow-2.11.1

# Install hls4ml and dependencies
USER ${NB_USER}
RUN mamba install -y -c conda-forge \
    graphviz==7.1.0 \
    pydot==1.4.2 \
    tensorflow-datasets==4.8.3 \
    jupyter-book==0.15.1
RUN pip install \
    hls4ml[profiling]==0.7.0rc1 \
    qkeras==0.9.0 \
    conifer==0.2b0
RUN mamba clean --all -f -y && \
    mamba list && \
    fix-permissions "${CONDA_DIR}" && \
    fix-permissions "/home/${NB_USER}"