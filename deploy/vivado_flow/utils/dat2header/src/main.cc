#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>

#if 1
#include <ap_fixed.h>
#else
#include <ac_fixed.h>
#endif

// Word width should be a multiple of 8
#define W_AXI_WIDTH 16
#define I_AXI_WIDTH 6

#define W_WIDTH 16
#define I_WIDTH 6

#if 1
#define DATA_AXI_T ap_fixed<W_AXI_WIDTH, I_AXI_WIDTH>
#define DATA_T ap_fixed<W_WIDTH, I_WIDTH>
#else
#define DATA_AXI_T ac_fixed<W_AXI_WIDTH, I_AXI_WIDTH>
#define DATA_T ac_fixed<W_WIDTH, I_WIDTH>
#endif

// Strip '0b' and '.' from input string
std::string strip_0b(std::string val) {
    std::string tmp = val.substr(2, val.size());
    tmp.erase(std::remove(tmp.begin(), tmp.end(), '.'), tmp.end());
    return tmp;
}

int main(int argc, char **argv) {
    if (argc != 5) {
        std::cout << "ERROR: Usage: " << argv[0] << " <data.dat> <header.dat> <array_name> <sample_max>" << std::endl;
        return 1;
    }

    const char *data_dat_filename = argv[1];
    const char *data_h_filename = argv[2];
    const char *array_name = argv[3];
    const size_t sample_max = atoi(argv[4]);

    std::ifstream fin(data_dat_filename);
    std::ofstream fout(data_h_filename);

    std::cout << "INFO: Input file: " << data_dat_filename << std::endl;
    std::cout << "INFO: Output file: " << data_h_filename << std::endl;

    if (!fin.is_open()) {
        std::cout << "ERROR: Fail to open file: " << data_dat_filename << std::endl;
        return 1;
    }

    if (!fout.is_open()) {
        std::cout << "ERROR: Fail to open file: " << data_h_filename << std::endl;
        return 1;
    }

    size_t feature_count = 0;
    size_t sample_count = 0;

    std::string iline;
    std::vector<float> data_flt_vec;
    while (std::getline(fin, iline)) {
        sample_count++;
        char *cstr = const_cast<char*>(iline.c_str());
        char *current = strtok(cstr," ");
        while (current!=NULL) {
            data_flt_vec.push_back(atof(current));
            current = strtok(NULL," ");
        }
        if (sample_count >= sample_max) break;
    }
    feature_count = data_flt_vec.size() / sample_count;
    fout << "#define " << array_name << "_SAMPLE_COUNT " << sample_count << std::endl;
    fout << "#define " << array_name << "_FEATURE_COUNT " << feature_count << std::endl;

    std::vector<DATA_AXI_T> data_fxd_vec;
    //std::copy(data_flt_vec.cbegin(), data_flt_vec.cend(), data_fxd_vec.begin());
    for (std::vector<float>::const_iterator i = data_flt_vec.cbegin(); i != data_flt_vec.cend(); i++) {
        DATA_T data(*i);
        DATA_AXI_T data_axi(data);
        data_fxd_vec.push_back(data_axi);
    }

    fout << "static unsigned short " << array_name << "_data[(" << array_name << "_SAMPLE_COUNT * " << array_name << "_FEATURE_COUNT)] = {";
    unsigned j = 0;
    for (std::vector<DATA_AXI_T>::const_iterator i = data_fxd_vec.cbegin(); i != data_fxd_vec.cend(); i++) {
        if (j++ % feature_count == 0) fout << std::endl;
#if 1
        //fout << (*i).to_string(2) << ((i+1 != data_fxd_vec.cend()) ? ", " : "\n");
        fout << "0b";
        for (int k = W_AXI_WIDTH-1; k >= 0; k--) {
            fout << (*i)[k];
        }
        fout << ((i+1 != data_fxd_vec.cend()) ? ", " : "\n");
#else
#if 1
        fout << "0b" << strip_0b((*i).to_string(AC_BIN)) << ((i+1 != data_fxd_vec.cend()) ? ", " : "\n");
#else
        std::string data_str = strip_0b((*i).to_string(AC_BIN));
        fout << "0x" << std::hex << std::stoi(data_str, nullptr, 2) << std::dec << ((i+1 != data_fxd_vec.cend()) ? ", " : "\n");
#endif
#endif
    }

    fout << "};" << std::endl;

    fin.close();
    fout.close();

    return 0;
}
