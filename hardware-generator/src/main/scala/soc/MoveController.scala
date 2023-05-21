package cdnn

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.rocket._
import cdnnISA._

class MoveController(spadAddrWidth: Int, spadAddrNum: Int, lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int)
                    (implicit p: Parameters) extends CoreModule {
  val io = IO(new Bundle {
    val core = new CoreIF(idWidth)
    val spadread = Flipped(new SpadStreamReadIF(spadAddrWidth, lgMaxDataLen, dataWidth, hasMask, idWidth))
    val spadwrite = Flipped(new SpadStreamWriteIF(spadAddrWidth, spadAddrNum, lgMaxDataLen, dataWidth, hasMask, idWidth))
  })

  val dataByte = dataWidth / 8
  val s_idle :: s_read_req :: s_write_req :: s_stream :: s_resp :: Nil = Enum(5)
  val state = RegInit(s_idle)
  val spad_addr_from = RegInit(0.U(spadAddrWidth.W))
  val spad_addr_to = RegInit(0.U(spadAddrWidth.W))
  val leftLen = RegInit(0.U(lgMaxDataLen.W))
  val success = RegInit(true.B)
  val rs1 = io.core.req.bits.cmd.rs1.asTypeOf(new MoveRs1)
  val rs2 = io.core.req.bits.cmd.rs2.asTypeOf(new MoveRs2)

  switch(state){
    is(s_idle) {
      when(io.core.req.fire){
        state := s_read_req
        spad_addr_from := rs1.spad_addr_from
        spad_addr_to := rs2.spad_addr_to
        leftLen := rs2.total_len
        success := true.B
      }
    }
    is(s_read_req) {
      when(io.spadread.req.fire) {
        state := s_write_req
      }
    }
    is(s_write_req){
      when(io.spadwrite.req.fire){
        state :=  s_stream
      }
    }
    is(s_stream){
      when(io.spadread.stream.fire){
        leftLen := leftLen - dataByte.U
        state := Mux(leftLen <= dataByte.U, s_resp, s_stream)
      }
    }
    is(s_resp){
      when(io.core.resp.fire){
        state := s_idle
      }
    }
  }

  io.core.req.ready := (state === s_idle)

  io.spadread.req.valid := (state === s_read_req)
  io.spadread.req.bits.addrs(0) := spad_addr_from
  io.spadread.req.bits.len := leftLen

  io.spadwrite.req.valid := (state === s_write_req)
  io.spadwrite.req.bits.addrs(0) := spad_addr_to
  io.spadwrite.req.bits.num := 0.U
  io.spadwrite.req.bits.len := leftLen

  io.spadread.stream.ready := io.spadwrite.stream.ready && (state === s_stream)
  io.spadwrite.stream.valid := io.spadread.stream.valid && (state === s_stream)
  io.spadwrite.stream.bits := io.spadread.stream.bits

  io.core.resp.valid := (state === s_resp)
  io.core.resp.bits.success := success
}