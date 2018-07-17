package vmon

import chisel3._
import chisel3.util._
import chisel3.experimental._
import chisel3.core.BlackBox
import std_protocol_if.Wishbone

class WishboneVmonMonitor(
    p : Wishbone.Parameters,
    ADDRESS : Int) extends Module {
  
  val io = IO(new Bundle {
    val m = Input(new Wishbone(p))
  });
  
  val vmon_monitor = Module(new wb_vmon_monitor(
      p.ADDR_WIDTH, p.DATA_WIDTH, ADDRESS))
  
  vmon_monitor.io.clk_i := clock
  vmon_monitor.io.rst_i := reset
  vmon_monitor.io.ADR := io.m.req.ADR
  vmon_monitor.io.DAT_W := io.m.req.DAT_W
  vmon_monitor.io.CYC := io.m.req.CYC
  vmon_monitor.io.ERR := io.m.rsp.ERR
  vmon_monitor.io.SEL := io.m.req.SEL
  vmon_monitor.io.STB := io.m.req.STB
  vmon_monitor.io.ACK := io.m.rsp.ACK
  vmon_monitor.io.WE := io.m.req.WE
}

class wb_vmon_monitor(
    val WB_ADDR_WIDTH : Int = 32,
    val WB_DATA_WIDTH : Int = 32,
    val ADDRESS       : Int) extends BlackBox(
        Map("WB_ADDR_WIDTH" -> WB_ADDR_WIDTH,
            "WB_DATA_WIDTH" -> WB_DATA_WIDTH,
            "ADDRESS" -> ADDRESS)) {
          
    val io = IO(new Bundle {
      val clk_i = Input(Clock())
      val rst_i = Input(Bool())
      val ADR = Input(UInt(WB_ADDR_WIDTH.W))
      val DAT_W = Input(UInt(WB_DATA_WIDTH.W))
      val CYC = Input(Bool())
      val ERR = Input(Bool())
      val SEL = Input(UInt((WB_DATA_WIDTH/8).W))
      val STB = Input(Bool())
      val ACK = Input(Bool())
      val WE = Input(Bool())
    });
  
}