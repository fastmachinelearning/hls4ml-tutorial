1. Understand why hls4ml throws a warning when parsing Add nodes in QONNX (i.e. using bias in Brevitas model)

2. Understand why QONNX needs softmax = legacy (if it's a bug in hls4ml, we should fix it; if not, we should add a note to the notebook)

3. Add HGQ2 notebook (to replace current HGQ tutorial) in part 2

4. Once PQuant support is merged to hls4ml main, update environment.yml and add notebook to part 2

5. Add more models to `Advanced Models`: From Imperial HGQ CNN, GNN and MLP

6. Add accelerator backend notebooks once the accelerator backends are merged

7. Add detailed README.md with some progression figure indicating what tutorials to follow & update ToC

8. Think about how to source Vivado/Vitis HLS - adding XILINX_HLS to the PATH may not work for all systems (e.g., some clusters use module)

9. Add a notebook (e.g., 3c) on the different implementations of GEMV/Dense layers: Latency, Resource, da4ml

10. Add a part 0 on basics of neural networks, FPGAs, etc.,
