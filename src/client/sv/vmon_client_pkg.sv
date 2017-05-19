/****************************************************************************
 * vmon_client_pkg.sv
 ****************************************************************************/

/**
 * Package: vmon_client_pkg
 * 
 * TODO: Add package documentation
 */
package vmon_client_pkg;
	
	class vmon_m2h_if;
		virtual task recv(
			output byte	unsigned	data[64],
			input int				size,
			input int				timeout,
			output int				ret);
			ret = -1;
		endtask
			
	endclass
	
	class vmon_h2m_if;
		virtual task send(
			input byte unsigned 	data[64],
			input int				size,
			output int				ret);
			ret = -1;
		endtask
			
	endclass

	// Global maps between native and SV handles
	vmon_m2h_if			m_m2h_if[chandle];
	vmon_h2m_if			m_h2m_if[chandle];
	
	class vmon_client;
		chandle				m_client;
		
		function new();
			m_client = _vmon_client_new();
		endfunction
		
		function void add_m2h_if(vmon_m2h_if ifc);
			chandle if_chndl = _vmon_client_add_m2h_if(m_client);
			m_m2h_if[if_chndl] = ifc;
		endfunction

		function void add_h2m_if(vmon_h2m_if ifc);
			chandle if_chndl = _vmon_client_add_h2m_if(m_client);
			m_h2m_if[if_chndl] = ifc;
		endfunction
		
		task connect(output bit ok);
			_vmon_client_connect(m_client, ok);
		endtask
		
		task read(
			longint unsigned		addr,
			ref byte unsigned		data[],
			input shortint unsigned	len);
			byte unsigned tmp[1024];
			int read_max = 1024;
			
			for (int i=0; i<len; i+=read_max) begin
				int read_limit = ((len-i)<read_max)?(len-i):read_max;
				_vmon_client_read(m_client, addr+i, tmp, read_limit);
				
				for (int j=i; j<read_limit; j++) begin
					data[j] = tmp[j-i];
				end
			end
		endtask
		
	endclass
	

	import "DPI-C" context function chandle _vmon_client_new();

	import "DPI-C" context function chandle _vmon_client_add_m2h_if(chandle client);

	import "DPI-C" context function chandle _vmon_client_add_h2m_if(chandle client);
	
	import "DPI-C" context task _vmon_client_connect(
			chandle			client,
			output int		ok);
	
	import "DPI-C" context task _vmon_client_read(
			input chandle			client,
			input longint unsigned	addr,
			output byte unsigned	data[1024],
			input int unsigned		len);

	task automatic _vmon_m2h_if_recv(
		input chandle			if_h,
		output byte	unsigned	data[64],
		input int				size,
		input int				timeout,
		output int				ret);
		vmon_m2h_if h = m_m2h_if[if_h];
		h.recv(data, size, timeout, ret);
	endtask
	export "DPI-C" task _vmon_m2h_if_recv;
	
	task automatic _vmon_h2m_if_send(
		input chandle			if_h,
		input byte	unsigned	data[64],
		input int				size,
		output int				ret);
		vmon_h2m_if h = m_h2m_if[if_h];
		h.send(data, size, ret);
	endtask
	export "DPI-C" task _vmon_h2m_if_send;

endpackage


