/****************************************************************************
 * vmon_bus_monitor_h2m_base.svh
 ****************************************************************************/

/**
 * Class: vmon_bus_monitor_h2m_base
 * 
 * 
 */
class vmon_bus_monitor_h2m_base implements vmon_h2m_if;
	byte							m_data[$];

	function new();

	endfunction

	virtual task recv(
			output byte	unsigned	data[64],
			input int				size,
			input int				timeout,
			output int				ret);
		int idx = 0;
		
		ret = 0;
		
		// TODO: work with timeout
		
		do begin
			data[ret++] = m_data.pop_front();
		end while (m_data.size() > 0 && ret < size);
		
	endtask


endclass


