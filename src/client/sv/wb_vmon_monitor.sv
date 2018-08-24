
/****************************************************************************
 * wb_vmon_monitor.sv
 ****************************************************************************/

  
/**
 * Module: wb_vmon_monitor
 * 
 * TODO: Add module documentation
 */
module wb_vmon_monitor #(
		parameter int WB_ADDR_WIDTH = 32,
		parameter int WB_DATA_WIDTH = 32,
		parameter int ADDRESS       = 'h0000_0000)(
	input								clk_i,
	input								rst_i,
	input[(WB_ADDR_WIDTH-1):0]			ADR,
	input[(WB_DATA_WIDTH-1):0]			DAT_W,
	input								CYC,
	input								ERR,
	input[((WB_DATA_WIDTH/8)-1):0]		SEL,
	input								STB,
	input								ACK,
	input								WE);

	wb_vmon_monitor_if #(
			WB_ADDR_WIDTH, 
			WB_DATA_WIDTH) u_core();

	assign u_core.ADDRESS = ADDRESS;
	assign u_core.clk_i = clk_i;
	assign u_core.rst_i = rst_i;
	assign u_core.ADR = ADR;
	assign u_core.DAT_W = DAT_W;
	assign u_core.CYC = CYC;
	assign u_core.ERR = ERR;
	assign u_core.SEL = SEL;
	assign u_core.STB = STB;
	assign u_core.ACK = ACK;
	assign u_core.WE = WE;

endmodule

interface wb_vmon_monitor_if #(
		parameter int WB_ADDR_WIDTH = 32,
		parameter int WB_DATA_WIDTH = 32)();
	import vmon_client_api_pkg::*;
	
	wire								clk_i;
	wire								rst_i;
	wire[(WB_ADDR_WIDTH-1):0]			ADR;
	wire[(WB_DATA_WIDTH-1):0]			DAT_W;
	wire								CYC;
	wire								ERR;
	wire[((WB_DATA_WIDTH/8)-1):0]		SEL;
	wire								STB;
	wire								ACK;
	wire								WE;
	
`ifdef HAVE_HDL_VIRTUAL_INTERFACE
	vmon_m2h_api					api;
`else
	int unsigned					m_id;
	
	import "DPI-C" context function int unsigned wb_vmon_monitor_register(string path);
	
	initial begin
		m_id = wb_vmon_monitor_register($sformatf("%m"));
	end
`endif /* HAVE_HDL_VIRTUAL_INTERFACE */
	longint unsigned				data;
	int unsigned					size;
	
	wire[WB_ADDR_WIDTH-1:0]         ADDRESS;

	wire addr_eq = (ADR[WB_ADDR_WIDTH-1:$clog2(WB_DATA_WIDTH)-1] == 
			ADDRESS[WB_ADDR_WIDTH-1:$clog2(WB_DATA_WIDTH)-1]);
	
`ifndef HAVE_HDL_VIRTUAL_INTERFACE
	import "DPI-C" task wb_vmon_monitor_write(
			int unsigned id,
			longint unsigned data,
			int unsigned sz);
`endif
	always @(posedge clk_i) begin
		if (rst_i == 0) begin
			// We've got a write
			if (CYC && STB && ACK && WE && addr_eq) begin
`ifdef HAVE_HDL_VIRTUAL_INTERFACE
				if (api == null) begin
					$display("ERROR: api for %m is null");
				end
`endif /* HAVE_HDL_VIRTUAL_INTERFACE */
				
				case (SEL)
					'b0001: begin
						data = DAT_W[7:0];
						size = 1;
					end
					'b0010: begin
						data = DAT_W[15:8];
						size = 1;
					end 
					'b0100: begin
						data = DAT_W[23:16];
						size = 1;
					end
					'b1000: begin
						data = DAT_W[31:24];
						size = 1;
					end
					'b0011: begin
						data = {DAT_W[15:0], DAT_W[7:0]};
						size = 2;
					end
					'b1100: begin
						data = {DAT_W[31:24], DAT_W[23:16]};
						size = 2;
					end
					'b1111: begin
						data = {DAT_W[31:24], DAT_W[23:16], DAT_W[15:8], DAT_W[7:0]};
						size = 4;
					end 
					default: $display("Unrecognized SEL 'b%04b", SEL);
				endcase
`ifdef HAVE_HDL_VIRTUAL_INTERFACE
				if (api != null) begin
					api.write(data, size);
				end else begin
					$display("ERROR: api for %m is null");
				end
`else
				wb_vmon_monitor_write(m_id, data, size);
				$display("VMON: write sz=%0d", size);
`endif /* HAVE_HDL_VIRTUAL_INTERFACE */
			end
		end 
	end
	
endinterface


