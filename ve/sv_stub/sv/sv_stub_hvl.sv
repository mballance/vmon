
/****************************************************************************
 * sv_stub_hvl.sv
 ****************************************************************************/

  
/**
 * Module: sv_stub_hvl
 * 
 * TODO: Add module documentation
 */
module sv_stub_hvl;
	import sv_stub_test_pkg::*;
	
	initial begin
		automatic virtual vmon_bus_monitor_if vif = sv_stub_hdl.u_mon.monitor_if;
		automatic virtual wb_master_bfm_core #(32, 32) wb_vif = sv_stub_hdl.u_bfm.u_core;
		automatic sv_stub_test test = new(wb_vif);
		
		vif.api = test.m_bus_monitor;
		
		test.run();
	end


endmodule


