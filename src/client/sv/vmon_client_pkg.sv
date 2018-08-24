/****************************************************************************
 * vmon_client_pkg.sv
 ****************************************************************************/

/**
 * Package: vmon_client_pkg
 * 
 * TODO: Add package documentation
 */
package vmon_client_pkg;
	import vmon_client_api_pkg::*;

	typedef class vmon_client;
	
	class vmon_m2h_if extends vmon_m2h_api;
		chandle						m_client_ep;
		vmon_client					m_client;

		task write(
			input byte unsigned		data[64],
			input int unsigned		size);
			m_client.notify_m2h_write();
			_vmon_m2h_client_ep_write(m_client_ep, data, size);
		endtask
	endclass
	
	interface class vmon_h2m_if;
		pure virtual task send(
			input byte unsigned 	data[64],
			input int				size,
			output int				ret);
	endclass

	`include "vmon_databuf.svh"
	`include "vmon_client_ep_if.svh"
	`include "vmon_client_ep0_if.svh"

	// Global maps between native and SV handles
	
	class vmon_client;
		chandle						m_client;
		vmon_m2h_if					m_m2h_if_l[$];
		static vmon_client			m_client_map[chandle];
		static vmon_client_ep0_if	m_ep0_client_if[chandle];
		static vmon_h2m_if			m_h2m_if[chandle];
		static vmon_m2h_if			m_m2h_if[chandle];
		vmon_client_ep_if			m_client_ep_if[$];
		semaphore					m_m2h_write_sem = new(0);
		
		vmon_client_ep0_if			m_ep0_listeners[$];
		
		function new();
			m_client = _vmon_client_new();
			m_client_map[m_client] = this;
		endfunction
		
		function void add_ep0_listener(vmon_client_ep0_if l);
			m_ep0_listeners.push_back(l);
		endfunction
		
		function void set_ep_listener(int unsigned ep, vmon_client_ep_if l);
			while (ep <= m_client_ep_if.size()) begin
				m_client_ep_if.push_back(null);
			end
			// Enable messages for this endpoint
			_vmon_client_set_ep_listener(m_client, ep);
			m_client_ep_if[ep] = l;
		endfunction
		
		function void add_m2h_if(vmon_m2h_if ifc);
			ifc.m_client_ep = _vmon_client_add_m2h_if(m_client);
			ifc.m_client = this;
			m_m2h_if_l.push_back(ifc);
		endfunction

		function void add_h2m_if(vmon_h2m_if ifc);
			chandle if_chndl = _vmon_client_add_h2m_if(m_client);
			m_h2m_if[if_chndl] = ifc;
		endfunction
		
		task connect(output bit ok);
			_vmon_client_connect(m_client, ok);
		endtask
		
		function longint unsigned get_entry_addr(string path);
			return _vmon_client_get_entry_addr(m_client, path);
		endfunction
		
		task exec(longint unsigned addr);
			_vmon_client_exec(m_client, addr);
		endtask
		
		task exit();
			_vmon_client_exit(m_client);
		endtask
		
		task wait_endtest(output int status);
			_vmon_client_wait_endtest(m_client, status);
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
		
		task poll(
			longint unsigned		mask,
			int						timeout,
			output int				ret
			);
			// Clear out the semaphore
			while (m_m2h_write_sem.try_get(1)) begin
			end
		
			// TODO: handle timeout
		
			$display("--> poll");
			m_m2h_write_sem.get(1);
			$display("<-- poll");
		endtask
		
		task notify_m2h_write();
			m_m2h_write_sem.put(1);
		endtask
		
		function void process_msg(byte unsigned ep, chandle data_h);
			
			if (m_client_ep_if.size() < ep && 
					m_client_ep_if[ep] != null) begin
					vmon_databuf data = new(data_h);
					m_client_ep_if[ep].process_msg(ep, data);
			end else begin
				$display("Error: no listener registered for EP %0d", ep);
			end
				
		endfunction
		
		function void ep0_msg(string m);
			foreach (m_ep0_listeners[i]) begin
				m_ep0_listeners[i].msg(m);
			end
		endfunction
		
		function void ep0_tracepoint(int unsigned tp);
			foreach (m_ep0_listeners[i]) begin
				m_ep0_listeners[i].tracepoint(tp);
			end
		endfunction
		
		function void ep0_endtest(int status);
			foreach (m_ep0_listeners[i]) begin
				m_ep0_listeners[i].endtest(status);
			end
		endfunction
		
	endclass
	

	import "DPI-C" context function chandle _vmon_client_new();
	
	import "DPI-C" context function void _vmon_client_set_ep_listener(chandle client, int unsigned ep);
	
	import "DPI-C" context function int unsigned _vmon_databuf_sz(chandle databuf);
	
	import "DPI-C" context function byte unsigned _vmon_databuf_at(chandle databuf, int unsigned idx);

	import "DPI-C" context function chandle _vmon_client_add_m2h_if(chandle client);

	import "DPI-C" context task _vmon_m2h_client_ep_write(
			chandle					m2h_h,
			input byte unsigned		data[64],
			input int unsigned		sz);

	import "DPI-C" context function chandle _vmon_client_add_h2m_if(chandle client);
	
	import "DPI-C" context task _vmon_client_connect(
			chandle			client,
			output int		ok);
	
	import "DPI-C" context function longint unsigned _vmon_client_get_entry_addr(
			chandle			client,
			string			path);
	
	import "DPI-C" context task _vmon_client_exec(
			chandle				client,
			longint unsigned	addr);
	
	import "DPI-C" context task _vmon_client_exit(
			chandle				client);
	
	import "DPI-C" context task _vmon_client_wait_endtest(
			chandle				client,
			output int			status);
			
	
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
		vmon_m2h_if h = vmon_client::m_m2h_if[if_h];
		$display("TODO: _vmon_m2h_if_recv");
//		h.recv(data, size, timeout, ret);
	endtask
	export "DPI-C" task _vmon_m2h_if_recv;
	
	task automatic _vmon_h2m_if_send(
		input chandle			if_h,
		input byte	unsigned	data[64],
		input int				size,
		output int				ret);
		vmon_h2m_if h = vmon_client::m_h2m_if[if_h];
		h.send(data, size, ret);
	endtask
	export "DPI-C" task _vmon_h2m_if_send;
	
	task automatic _vmon_client_poll(
		input chandle			if_h,
		longint unsigned		mask,
		input int				timeout,
		output int				ret
		);
		vmon_client::m_client_map[if_h].poll(mask, timeout, ret);
	endtask
	export "DPI-C" task _vmon_client_poll;
	
	function automatic void _vmon_client_ep_process_msg(
		input chandle			client_h,
		input byte unsigned		ep,
		input chandle			data_h);
		vmon_client::m_client_map[client_h].process_msg(ep, data_h);
	endfunction
	export "DPI-C" function _vmon_client_ep_process_msg;
	
	function automatic void _vmon_client_ep0_msg(
		input chandle			client_h,
		input string			msg);
		vmon_client::m_client_map[client_h].ep0_msg(msg);
	endfunction
	export "DPI-C" function _vmon_client_ep0_msg;
	
	function automatic void _vmon_client_ep0_tracepoint(
		input chandle			client_h,
		int unsigned			tp);
		vmon_client::m_client_map[client_h].ep0_tracepoint(tp);
	endfunction
	export "DPI-C" function _vmon_client_ep0_tracepoint;
	
	function automatic void _vmon_client_ep0_endtest(
		input chandle			client_h,
		input int				status);
		vmon_client::m_client_map[client_h].ep0_endtest(status);
	endfunction
	export "DPI-C" function _vmon_client_ep0_endtest;
	
endpackage


