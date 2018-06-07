
/****************************************************************************
 * sv_stub_hdl.sv
 ****************************************************************************/

  
/**
 * Module: sv_stub_hdl
 * 
 * TODO: Add module documentation
 */
module sv_stub_hdl;
	
	reg clk = 0;
	reg reset = 0;
	
	initial begin
		reset <= 1;
		repeat (100) begin
			#10ns;
			clk <= ~clk;
		end
		reset <= 0;
		forever begin
			#10ns;
			clk <= ~clk;
		end
	end
	
	wire[31:0]		addr;
	wire[31:0]		data;
	wire[3:0]		byte_en;
	wire			write_en;
	
	wb_if #(
		.WB_ADDR_WIDTH  (32 ), 
		.WB_DATA_WIDTH  (32 )
		) m_if ();
	
	wb_master_bfm #(
		.WB_ADDR_WIDTH  (32 ), 
		.WB_DATA_WIDTH  (32 )
		) u_bfm (
		.clk            (clk           ), 
		.rstn           (~reset        ), 
		.master         (m_if.master   ));
	
	generic_sram_byte_en_if #(
		.NUM_ADDR_BITS  (32 ), 
		.NUM_DATA_BITS  (32 )
		) u_sram_if (
		);
	
	wb_generic_byte_en_sram_bridge #(
		.ADDRESS_WIDTH  (32 ), 
		.DATA_WIDTH     (32    )
		) u_wb2sram (
		.clk            (clk           ), 
		.rstn           (~reset        ), 
		.wb_s           (m_if.slave    ), 
		.sram_m         (u_sram_if.sram_client));

	assign addr = u_sram_if.addr;
	assign data = u_sram_if.write_data;
	assign write_en = u_sram_if.write_en;
	assign byte_en = u_sram_if.byte_en;
	
	vmon_bus_monitor #(
		.ADDR_WIDTH     (32            ), 
		.DATA_WIDTH     (32            ), 
		.ADDR           ('h0000_0000   ), 
		.LITTLE_ENDIAN  (1 )
		) u_mon (
		.clk            (clk           ), 
		.reset          (reset         ), 
		.addr           (addr          ), 
		.data           (data          ), 
		.byte_en        (byte_en       ), 
		.write_en       (write_en      ));


endmodule


