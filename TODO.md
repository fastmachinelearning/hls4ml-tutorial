1. Understand why hls4ml throws a warning when parsing Add nodes in QONNX (i.e. using bias in Brevitas model)

2. Understand why QONNX needs softmax = legacy (if it's a bug in hls4ml, we should fix it; if not, we should add a note to the notebook)

3. Once QKeras v3 support is merged to hls4ml main from Marius' branch, update environment.yml

4. Add HGQ2 notebook (to replace current HGQ tutorial) in part 2

5. Once PQuant support is merged to hls4ml main, update environment.yml and add notebook to part 2

6. Add more models to `Advanced Models`: From Imperial HGQ CNN, GNN and MLP

7. Add accelerator backend notebooks once the accelerator backends are merged

8. Add detailed README.md with some progression figure indicating what tutorials to follow & update ToC

9. Think about how to source Vivado/Vitis HLS - adding XILINX_HLS to the PATH may not work for all systems (e.g., some clusters use module)

10. Add a notebook (e.g., 3c) on the different implementations of GEMV/Dense layers: Latency, Resource, da4ml

11. Add a part 0 on basics of neural networks, FPGAs, etc.,
