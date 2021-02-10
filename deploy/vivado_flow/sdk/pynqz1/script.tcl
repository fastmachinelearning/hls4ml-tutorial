setws .
createhw -name jet_tagger_design_wrapper_hw_platform_0 -hwspec hdf/jet_tagger_design_wrapper.hdf
createapp -name jet_tagger_baremetal_polling -app {Hello World} -proc ps7_cortexa9_0 -hwproject jet_tagger_design_wrapper_hw_platform_0 -os standalone
