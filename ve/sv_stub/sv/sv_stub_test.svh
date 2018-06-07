
/****************************************************************************
 * sv_stub_test.svh
 ****************************************************************************/

  
/**
 * Class: sv_stub_test
 * 
 * TODO: Add class documentation
 */
class sv_stub_test;
	sv_stub_bus_monitor_api					m_bus_monitor;
	sv_stub_test_m2h_path					m_m2h;
	vmon_m2h_if								m_client_m2h;
	virtual wb_master_bfm_core #(32,32)		m_wb_bfm;
	vmon_client								m_client;
	byte unsigned							m_inq[$];
	semaphore								m_inq_sem = new(0);

	function new(virtual wb_master_bfm_core #(32,32) wb_bfm);
		m_bus_monitor = new(this);
		m_wb_bfm = wb_bfm;
		m_m2h = new;
		m_m2h.m_vif = m_wb_bfm;
		m_m2h.m_addr = 0;
		
		m_client = new();
		m_client_m2h = new();
		m_client.add_m2h_if(m_client_m2h);
	endfunction
	
	task run();
		$display("RUN");
		fork
			inq_thread();
		join_none
		vmon_monitor_init();
		vmon_monitor_add_m2h_path(m_m2h);
		
		vmon_monitor_msg("Hello World!");
		
		vmon_monitor_tracepoint('h0004);
		vmon_monitor_tracepoint(8000);
	
	
		// Low-level way to emit a tracepoint
		// 16-bit write of 0x02a5
		// 32-bit write of {value[24], 0x05}
		m_wb_bfm.wb_master_bfm_set_data(0, 'h0000_02a5);
		m_wb_bfm.wb_master_bfm_request(0, 0, 0, 'h3, 1);
		m_wb_bfm.wb_master_bfm_set_data(0, {24'd9000, 8'h05});
		m_wb_bfm.wb_master_bfm_request(0, 0, 0, 'hf, 1);
//		
//		m_wb_bfm.wb_master_bfm_set_data(0, 'h04030201);
//		m_wb_bfm.wb_master_bfm_request(0, 0, 0, 'h3, 1);
//		
//		m_wb_bfm.wb_master_bfm_set_data(0, 'h04030201);
//		m_wb_bfm.wb_master_bfm_request(0, 0, 0, 'hC, 1);
//		
//		m_wb_bfm.wb_master_bfm_set_data(0, 'h04030201);
//		m_wb_bfm.wb_master_bfm_request(0, 0, 0, 'hF, 1);
		
	endtask
	
	task inq_thread();
		byte unsigned data;
		byte unsigned data_t[64];
		
		forever begin
			m_inq_sem.get(1);

			while (m_inq.size() > 0) begin
				int idx = 0;
				for (idx=0; idx<64 && m_inq.size() > 0; idx++) begin
					data_t[idx] = m_inq.pop_front();
				end
				m_client_m2h.write(data_t, idx);
			end
		end
	endtask

	function void write(input byte unsigned data[8], input int unsigned nbytes);
		for (int i=0; i<nbytes; i++) begin
			m_inq.push_back(data[i]);
		end
		m_inq_sem.put(1);
	endfunction

endclass


