
/****************************************************************************
 * vmon_bus_monitor.sv
 ****************************************************************************/

  
/**
 * Module: vmon_bus_monitor
 * 
 * TODO: Add module documentation
 */
module vmon_bus_monitor #(
		parameter int ADDR_WIDTH 			= 32,
		parameter int DATA_WIDTH 			= 32,
		parameter bit[ADDR_WIDTH-1:0] ADDR 	= 0,
		parameter bit LITTLE_ENDIAN			= 1	
		)(
		input							clk,
		input							reset,
		input [ADDR_WIDTH-1:0]			addr,
		input [DATA_WIDTH-1:0]			data,
		input [DATA_WIDTH/8-1:0]		byte_en,
		input 							write_en
		);

	vmon_bus_monitor_if		monitor_if(
			.clk(clk),
			.reset(reset),
			.addr(addr),
			.data(data),
			.byte_en(byte_en),
			.write_en(write_en));

endmodule

interface vmon_bus_monitor_if #(
		parameter int ADDR_WIDTH 			= 32,
		parameter int DATA_WIDTH 			= 32,
		parameter bit[ADDR_WIDTH-1:0] ADDR 	= 0,
		parameter bit LITTLE_ENDIAN			= 1	
		)(
		input							clk,
		input							reset,
		input [ADDR_WIDTH-1:0]			addr,
		input [DATA_WIDTH-1:0]			data,
		input [DATA_WIDTH/8-1:0]		byte_en,
		input 							write_en
		);
	import vmon_bus_monitor_api_pkg::*;
	
	vmon_bus_monitor_api		api;
	
	wire[3:0]					data_sz;
	wire[3:0]					first_byte;
	
	always @(posedge clk) begin
		if (reset == 1) begin
		end else begin
			static byte unsigned data_arr_t[8];
			static byte unsigned idx=0;
		
			// TODO: need to ignore address low bits
			if (write_en && addr == ADDR) begin
				idx=0;
				if (LITTLE_ENDIAN == 1) begin
					for (int i=0; i<(DATA_WIDTH/8); i++) begin
						if (byte_en[i]) begin
							data_arr_t[idx++] = data[8*i+:8];
						end
					end
				end else begin
					for (int i=(DATA_WIDTH/8)-1; i>=0; i--) begin
						if (byte_en[i]) begin
							data_arr_t[idx++] = data[8*i+:8];
						end
					end
				end
		
				if (api != null) begin
					api.write(data_arr_t, idx);
				end else begin
					$display("Error: api is null in %m");
				end
			end
		end
	end
	
endinterface
