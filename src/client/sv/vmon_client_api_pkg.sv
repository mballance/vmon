
/****************************************************************************
 * vmon_client_api_pkg.sv
 ****************************************************************************/

  
/**
 * Package: vmon_client_api_pkg
 * 
 * TODO: Add package documentation
 */
package vmon_client_api_pkg;
	
	class vmon_m2h_api;
		virtual task write(
			input byte unsigned		data[64],
			input int unsigned		size);
			// TODO: must be implemented 
		endtask
	endclass

endpackage


