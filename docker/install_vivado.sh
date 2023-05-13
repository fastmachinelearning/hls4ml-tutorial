#!/usr/bin/env bash

function main() {
    # Install Vivado; to speed up build, download files from local webserver
    # See: https://stackoverflow.com/questions/26692708/how-to-add-a-file-to-an-image-in-dockerfile-without-using-the-add-or-copy-direct
    cd /tmp/
    curl https://www.dropbox.com/s/wvp50u7h2jroict/vivado.tar.gz?dl=1 -L -o vivado.tar.gz
    # curl http://169.228.130.58:8000/vivado.tar.gz -o vivado.tar.gz
    tar -xzf vivado.tar.gz
    cd Xilinx_Vivado_2019.2_1106_2127
    ./xsetup --agree XilinxEULA,3rdPartyEULA,WebTalkTerms --batch Install --config /tmp/vivado_cfg.txt
    cd ..
    rm -r Xilinx_Vivado_2019.2_1106_2127
    rm vivado.tar.gz
    rm /tmp/vivado_cfg.txt

    # Install the pynq-z2 board files
    curl https://www.dropbox.com/s/meufyrhgcg38i12/pynq-z2.zip?dl=1 -L -o pynq-z2.zip
    # curl http://169.228.130.58:8000/pynq-z2.zip -o pynq-z2.zip
    unzip pynq-z2.zip
    rm pynq-z2.zip
    mv pynq-z2 /opt/Xilinx/Vivado/2019.2/data/boards/board_files/

    # Apply Vivado's y2k22 patch
    curl https://www.dropbox.com/s/3gv1jq9074d582o/y2k22_patch.zip?dl=1 -L -o y2k22_patch.zip
    # curl http://169.228.130.58:8000/y2k22_patch.zip -o y2k22_patch.zip
    mv y2k22_patch.zip /opt/Xilinx
    cd /opt/Xilinx
    unzip y2k22_patch.zip
    python y2k22_patch/patch.py
}

main "$@" || exit 1
