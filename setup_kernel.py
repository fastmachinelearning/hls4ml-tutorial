import os

if 'XILINX_VIVADO' in os.environ:
    l = '#!/bin/bash\n'
    l += 'source {}/settings64.sh\n'.format(os.environ['XILINX_VIVADO'])
    if 'XILINXD_LICENSE_FILE' in os.environ:
        l += 'export XILINXD_LICENSE_FILE={}\n'.format(os.environ['XILINXD_LICENSE_FILE'])
    if 'LM_LICENSE_FILE' in os.environ:
        l += 'export LM_LICENSE_FILE={}\n'.format(os.environ['LM_LICENSE_FILE'])
    l += 'exec python "$@"\n'
    with open('python_wrapper.sh','w') as f:
        f.write(l)
    os.system('chmod +x python_wrapper.sh')

    os.makedirs('{}/.local/share/jupyter/kernels/xilinx'.format(os.environ['HOME']),exist_ok=True)
    l = '{\n'
    l += '"display_name": "Xilinx",\n'
    l += '"language": "python",\n'
    l += '"argv": [\n'
    l += '"{}/python_wrapper.sh",\n'.format(os.environ['PWD'])
    l += '"-m",\n'
    l += '"ipykernel_launcher",\n'
    l += '"-f",\n'
    l += '"{connection_file}"\n'
    l += ']\n'
    l += '}\n'
    
    with open ('{}/.local/share/jupyter/kernels/xilinx/kernel.json'.format(os.environ['HOME']),'w') as f:
        f.write(l)

else:
    print('Is Vivado environment set up? No XILINX_VIVADO environment variable found')

