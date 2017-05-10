/****************************************************************************
 * vmon_client_pkg.sv
 ****************************************************************************/

/**
 * Package: vmon_client_pkg
 * 
 * TODO: Add package documentation
 */
package vmon_client_pkg;
	
	class vmon_client;
		chandle				m_client;
		
		function new();
			m_client = _vmon_client_new();
		endfunction
		
		task connect(output bit ok);
			_vmon_client_connect(m_client, ok);
		endtask
		
	endclass
	

	import "DPI-C" function context chandle _vmon_client_new();
	
	import "DPI-C" task context _vmon_client_connect(
			chandle			client,
			output int		ok);


endpackage


