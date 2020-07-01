# hls4ml-tutorial
Tutorial notebooks for hls4ml 

The Python environment used for the tutorials is specified in the hls4ml-tutorial-py36.txt file. It can be setup like:
```conda create --name hls4ml-tutorial --file hls4ml-tutorial-py36.txt
conda activate hls4ml-tutorial
```

Then, hls4ml needs to be installed following the instructions found at https://fastmachinelearning.org/hls4ml/setup/QUICKSTART.html
The master branch (at 773039f) or v0.2.1 (when it's created) should be used.

The final notebook introduces quantization aware training with qkeras, which also needs to be cloned from github.
Instructions are at https://github.com/google/qkeras
v0.7.4 was used for these notebooks.
