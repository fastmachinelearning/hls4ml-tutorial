# Script

# Set accelerator
set accname "jet_tagger_axi"

# Set directory
set proj_dir "./jet_tagger_project"

# Set project
set proj "jet_tagger_project"

# Set design name
set design_name "jet_tagger_design"

# Set board name
set board_name "cmoda735t"

# Create project
create_project $proj $proj_dir -part xc7a35tcpg236-1

# Set project properties
set_property board_part digilentinc.com:cmod_a7-35t:part0:1.1 [current_project]

# Set IP repository paths
set_property ip_repo_paths ../../hls/$board_name\_m_axi_16_serial_prj/jet_tagger_prj [current_project]
update_ip_catalog -rebuild

# Create the design block
create_bd_design $design_name

# Create clock wizard
create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0
apply_board_connection -board_interface "sys_clock" -ip_intf "clk_wiz_0/clock_CLK_IN1" -diagram $design_name

# Setup reset
set_property -dict [list CONFIG.RESET_BOARD_INTERFACE {reset}] [get_bd_cells clk_wiz_0]
apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {reset ( Reset (BTN0) ) } Manual_Source {New External Port (ACTIVE_HIGH)}}  [get_bd_pins clk_wiz_0/reset]

# Create UART interface
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uartlite_0
apply_board_connection -board_interface "usb_uart" -ip_intf "axi_uartlite_0/UART" -diagram "jet_tagger_design"

# Create instance of MicroBlaze
create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:11.0 microblaze
apply_bd_automation -rule xilinx.com:bd_rule:microblaze \
  -config { \
      axi_intc {0} \
      axi_periph {Enabled} \
      cache {None} \
      clk {/clk_wiz_0/clk_out1 (100 MHz)} \
      debug_module {Debug Only} \
      ecc {None} \
      local_mem {128KB} \
      preset {None} }  [get_bd_cells microblaze]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 \
  -config { \
      Clk_master {/clk_wiz_0/clk_out1 (100 MHz)} \
      Clk_slave {Auto} \
      Clk_xbar {Auto} \
      Master {/microblaze (Periph)} \
      Slave {/axi_uartlite_0/S_AXI} \
      intc_ip {New AXI Interconnect} \
      master_apm {0} }  [get_bd_intf_pins axi_uartlite_0/S_AXI]

# Add accelerator and connect s-axi interface
create_bd_cell -type ip -vlnv xilinx.com:hls:$accname:1.0 $accname
apply_bd_automation -rule xilinx.com:bd_rule:axi4 \
  -config { Clk_master {/clk_wiz_0/clk_out1 (100 MHz)} \
            Clk_slave {Auto} \
            Clk_xbar {/clk_wiz_0/clk_out1 (100 MHz)} \
            Master {/microblaze (Periph)} \
            Slave {/$accname/s_axi_CTRL_BUS} \
            intc_ip {/microblaze_axi_periph} \
            master_apm {0} }  [get_bd_intf_pins $accname/s_axi_CTRL_BUS]

# Reconfigure local memory to support accelerator DMA
set_property -dict [list CONFIG.C_NUM_LMB {2}] [get_bd_cells microblaze_local_memory/dlmb_bram_if_cntlr]
delete_bd_objs [get_bd_intf_nets microblaze_local_memory/microblaze_ilmb_bus]
connect_bd_intf_net [get_bd_intf_pins microblaze_local_memory/ilmb_v10/LMB_Sl_0] [get_bd_intf_pins microblaze_local_memory/dlmb_bram_if_cntlr/SLMB1]
delete_bd_objs [get_bd_intf_nets microblaze_local_memory/microblaze_ilmb_cntlr] [get_bd_cells microblaze_local_memory/ilmb_bram_if_cntlr]

# Add AXI interconnect
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_0
set_property -dict [list CONFIG.NUM_SI {2} CONFIG.NUM_MI {1}] [get_bd_cells axi_interconnect_0]

# Add AXI BRAM controller
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bram_ctrl]

# Interconnect ports
connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_0/M00_AXI] [get_bd_intf_pins axi_bram_ctrl/S_AXI]
connect_bd_intf_net [get_bd_intf_pins axi_bram_ctrl/BRAM_PORTA] [get_bd_intf_pins microblaze_local_memory/lmb_bram/BRAM_PORTB]
move_bd_cells [get_bd_cells microblaze_local_memory] [get_bd_cells axi_bram_ctrl]
connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_0/S00_AXI] [get_bd_intf_pins $accname/m_axi_IN_BUS]
connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_0/S01_AXI] [get_bd_intf_pins $accname/m_axi_OUT_BUS]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (100 MHz)" }  [get_bd_pins axi_interconnect_0/ACLK]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (100 MHz)" }  [get_bd_pins axi_interconnect_0/S00_ACLK]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (100 MHz)" }  [get_bd_pins axi_interconnect_0/M00_ACLK]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (100 MHz)" }  [get_bd_pins axi_interconnect_0/S01_ACLK]

# Assign the address in memory for the accelerator execution
assign_bd_address [get_bd_addr_segs {microblaze_local_memory/dlmb_bram_if_cntlr/SLMB1/Mem }]
assign_bd_address [get_bd_addr_segs {microblaze_local_memory/axi_bram_ctrl/S_AXI/Mem0 }]

# Validate the design block we created
validate_bd_design

# Save design
save_bd_design

# Create verilog top
make_wrapper -files [get_files $proj_dir/$proj.srcs/sources_1/bd/$design_name/$design_name.bd] -top
add_files -norecurse $proj_dir/$proj.srcs/sources_1/bd/$design_name/hdl/$design_name\_wrapper.v

# Run the synthesis step
launch_runs synth_1
wait_on_run -timeout 360 synth_1

# Run the implementation step
puts "INFO: Run bistream generation..."
launch_runs impl_1 -to_step write_bitstream
wait_on_run -timeout 360 impl_1

if {[get_property PROGRESS [get_runs impl_1]] != "100%"} {
    error "ERROR: bitstream generation failed!"
}

# Export the bitstream and the hardware for the SDK
puts "INFO: Export hardware..."
file copy -force $proj_dir/$proj.runs/impl_1/$design_name\_wrapper.sysdef \
    ../../sdk/$board_name/hdf/$design_name\_m_axi_32_serial_wrapper.hdf

