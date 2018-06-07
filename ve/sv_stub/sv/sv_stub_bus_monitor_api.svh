
/****************************************************************************
 * sv_stub_bus_monitor_api.svh
 ****************************************************************************/

typedef class sv_stub_test;
  
/**
 * Class: sv_stub_bus_monitor_api
 * 
 * TODO: Add class documentation
 */
class sv_stub_bus_monitor_api extends vmon_bus_monitor_api;
	sv_stub_test				m_test;

	function new(sv_stub_test test);
		m_test = test;
	endfunction
	
	/**
	 * Function: write
	 *
	 * Override from class vmon_bus_monitor_api
	 */
	virtual function void write(input byte unsigned data[8], input int unsigned nbytes);
		m_test.write(data, nbytes);
	endfunction


endclass


