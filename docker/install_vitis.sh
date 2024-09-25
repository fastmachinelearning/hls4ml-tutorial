#!/usr/bin/env bash

function main() {
    # Install Vivado; to speed up build, download files from local webserver
    # See: https://stackoverflow.com/questions/26692708/how-to-add-a-file-to-an-image-in-dockerfile-without-using-the-add-or-copy-direct
    cd /tmp/
    curl http://10.164.29.48:8000//FPGAs_AdaptiveSoCs_Unified_2024.1_0522_2023.tar.gz?dl=1 -L -o FPGAs_AdaptiveSoCs_Unified_2024.1_0522_2023.tar.gz
    # curl http://169.228.130.58:8000/vivado.tar.gz -o vivado.tar.gz
    tar -xzf FPGAs_AdaptiveSoCs_Unified_2024.1_0522_2023.tar.gz --no-same-owner
    cd FPGAs_AdaptiveSoCs_Unified_2024.1_0522_2023
    ./xsetup -a XilinxEULA,3rdPartyEULA -b Install -c /tmp/vitis_cfg.txt
    ./installLibs.sh
    cd ..
    rm -rf FPGAs_AdaptiveSoCs_Unified_2024.1_0522_2023
    rm FPGAs_AdaptiveSoCs_Unified_2024.1_0522_2023.tar.gz
    rm /tmp/vitis_cfg.txt
    ls -lah /opt/Xilinx/

    # Install the pynq-z2 board files
    curl https://www.dropbox.com/s/meufyrhgcg38i12/pynq-z2.zip?dl=1 -L -o pynq-z2.zip
    # curl http://169.228.130.58:8000/pynq-z2.zip -o pynq-z2.zip
    unzip pynq-z2.zip
    rm pynq-z2.zip
    mv pynq-z2 /opt/Xilinx/Vitis_HLS/2024.1/data/boards/board_files/
}

main "$@" || exit 1
