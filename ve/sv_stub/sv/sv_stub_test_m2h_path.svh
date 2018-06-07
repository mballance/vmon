
/****************************************************************************
 * sv_stub_test_m2h_path.svh
 ****************************************************************************/

  
/**
 * Class: sv_stub_test_m2h_path
 * 
 * TODO: Add class documentation
 */
class sv_stub_test_m2h_path extends vmon_data_f;
	virtual wb_master_bfm_core #(32,32) m_vif;
	bit[31:0]							m_addr;

	function new();
	endfunction
	
	virtual task access(
		byte unsigned 	data[8], 
		int unsigned 	sz,
		output int		ret);
		int i=0;
		
		if (sz == 0) begin
			return;
		end
		
		while (i<sz) begin
			int r_sz = (sz-i);
			if (r_sz >= 4) begin
				bit[31:0] data_t = {data[i+3], data[i+2], data[i+1], data[i+0]};
				m_vif.wb_master_bfm_set_data(0, data_t);
				m_vif.wb_master_bfm_request(m_addr, 0, 0, 'hf, 1);
				i+=4;
			end else if (r_sz >= 2) begin
				bit[31:0] data_t = {data[i+1], data[i+0]};
				m_vif.wb_master_bfm_set_data(0, data_t);
				m_vif.wb_master_bfm_request(m_addr, 0, 0, 'h3, 1);
				i+=2;
			end else if (r_sz >= 1) begin
				bit[31:0] data_t = {data[i+0]};
				m_vif.wb_master_bfm_set_data(0, data_t);
				m_vif.wb_master_bfm_request(m_addr, 0, 0, 'h1, 1);
				i++;
			end else begin
				$display("TODO: access sz=%0d", sz);
				break;
			end
		end
		
		ret = sz;
	endtask


endclass


