
/****************************************************************************
 * vmon_monitor_pkg.sv
 ****************************************************************************/

  
/**
 * Package: vmon_monitor_pkg
 * 
 * The classes in this package expose the vmon monitor via SystemVerilog.
 * Thus far, this is only used for testing
 */
package vmon_monitor_pkg;

	function automatic void vmon_monitor_init();
		_vmon_monitor_init();
	endfunction
	import "DPI-C" function _vmon_monitor_init();
	
	class vmon_data_f;
		virtual task access(
			byte unsigned 	data[8], 
			int unsigned 	sz,
			output int		ret);
		endtask
	endclass
	
	vmon_data_f		m2h_path_l[int unsigned];
		
	function automatic int unsigned vmon_monitor_add_m2h_path(vmon_data_f f);
		int unsigned id = _vmon_monitor_add_m2h_path();
		m2h_path_l[id] = f;
	endfunction
	
	import "DPI-C" function int unsigned _vmon_monitor_add_m2h_path();

	task vmon_monitor_msg(string msg);
		_vmon_monitor_msg(msg);
	endtask
	import "DPI-C" context task _vmon_monitor_msg(string msg);
	
	task m2h_access_data(
		int unsigned 		id,
		input byte unsigned	data[8],
		input int unsigned		sz,
		output int				ret);
		m2h_path_l[id].access(data, sz, ret);
	endtask
	export "DPI-C" task m2h_access_data;

	task vmon_monitor_tracepoint(int unsigned tp);
		_vmon_monitor_tracepoint(tp);
	endtask
	import "DPI-C" context task _vmon_monitor_tracepoint(int unsigned tp);
	
endpackage


