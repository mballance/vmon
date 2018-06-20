
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
		parameter int WB_DATA_WIDTH = 32)(
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

	wb_vmon_monitor_if #(WB_ADDR_WIDTH, WB_DATA_WIDTH) u_core();
	
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
	
	vmon_m2h_api					api;
	byte unsigned					data[64];
	int unsigned					size;
	
	always @(posedge clk_i) begin
		if (rst_i == 0) begin
			// We've got a write
			if (CYC && STB && ACK && WE) begin
				if (api == null) begin
					$display("ERROR: api for %m is null");
				end
				
				case (SEL)
					'b0001: begin
						data[0] = DAT_W[7:0];
						size = 1;
					end
					'b0010: begin
						data[0] = DAT_W[15:8];
						size = 1;
					end 
					'b0100: begin
						data[0] = DAT_W[23:16];
						size = 1;
					end
					'b1000: begin
						data[0] = DAT_W[31:24];
						size = 1;
					end
					'b0011: begin
						data[0] = DAT_W[7:0];
						data[1] = DAT_W[15:8];
						size = 2;
					end
					'b1100: begin
						data[0] = DAT_W[23:16];
						data[1] = DAT_W[31:24];
						size = 2;
					end
					'b1111: begin
						data[0] = DAT_W[7:0];
						data[1] = DAT_W[15:8];
						data[2] = DAT_W[23:16];
						data[3] = DAT_W[31:24];
						size = 4;
					end 
					default: $display("Unrecognized SEL 'b%04b", SEL);
				endcase
				
				api.write(data, size);
			end
		end 
	end
	
endinterface


