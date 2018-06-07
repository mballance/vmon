
/****************************************************************************
 * vmon_bus_monitor_api_pkg.sv
 ****************************************************************************/

  
/**
 * Package: vmon_bus_monitor_api_pkg
 * 
 * TODO: Add package documentation
 */
package vmon_bus_monitor_api_pkg;
	
	class vmon_bus_monitor_api;
		
		virtual function void write(byte unsigned data[8], int unsigned nbytes);
		endfunction
			
	endclass


endpackage


