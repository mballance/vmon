/****************************************************************************
 * vmon_client_agent.svh
 ****************************************************************************/

/**
 * Class: vmon_client_agent
 * 
 * TODO: Add class documentation
 */
class vmon_client_agent 
		extends uvm_component
		implements vmon_client_ep0_if;
	`uvm_component_utils(vmon_client_agent)
	vmon_client			m_client;
	uvm_phase			m_run_phase;

	function new(string name, uvm_component parent);
		super.new(name, parent);
	endfunction
	
	/**
	 * Function: build_phase
	 *
	 * Override from class 
	 */
	virtual function void build_phase(input uvm_phase phase);
		m_client = new();
	endfunction

	/**
	 * Function: connect_phase
	 *
	 * Override from class 
	 */
	virtual function void connect_phase(input uvm_phase phase);
		m_client.add_ep0_listener(this);
	endfunction
	

	/**
	 * Task: run_phase
	 *
	 * Override from class 
	 */
	virtual task run_phase(input uvm_phase phase);
		m_run_phase = phase;
		phase.raise_objection(this, "Software Test");
	endtask

	virtual function void msg(string m);
		`uvm_info(get_name(), $sformatf("\033[31;1;4m%0s\033[0m", m), UVM_LOW);
	endfunction
	
	virtual function void tracepoint(int unsigned tp);
		`uvm_info(get_name(), $sformatf("Tracepoint: %0d", tp), UVM_LOW);
	endfunction
	
	virtual function void endtest(int status);
		$display("NOTE (UVM): endtest %0d", status);
		if (m_run_phase != null) begin
			`uvm_info(get_name(), $sformatf("Dropping objection objection=%p", 
					m_run_phase.get_objection()), UVM_LOW);
			m_run_phase.drop_objection(this, "Software Test");
		end else begin
			`uvm_fatal(get_name(), "m_run_phase is null");
		end
	endfunction
	

endclass


