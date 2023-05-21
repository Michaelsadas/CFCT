package cdnn

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.rocket._
import cdnnISA._

/** Store controller, transfer data from local scratchpad (physical address) to remote memory (virtual address)
  * @param spadAddrWidth  scratchpad address width, <= 32
  * @param lgMaxDataLen   log2(the max data length in one DMA request)
  * @param dataWidth      data width in bits
  * @param hasMask        if has mask signal in the stream interface
  * @param idWidth        width of the ID for a DMA request
  * @param streamQueDepth stream queue depth, can be zero
  */
class StoreController(spadAddrWidth: Int, lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int, streamQueDepth: Int)
                    (implicit p: Parameters) extends CoreModule {
  val io = IO(new Bundle {
    val core = new CoreIF(idWidth)
    val dma = Flipped(new DMAStreamWriteIF(lgMaxDataLen, dataWidth, hasMask, idWidth))
    val spad = Flipped(new SpadStreamReadIF(spadAddrWidth, lgMaxDataLen, dataWidth, hasMask, idWidth))
  })

  val dataByte = dataWidth/8
  val s_idle :: s_spad_req :: s_dma_req :: s_stream :: s_exp :: s_resp :: s_loop :: Nil = Enum(7)
  val state = RegInit(s_idle)
  val remote_stride = RegInit(0.U(coreMaxAddrBits.W))
  val spad_stride = RegInit(0.U(spadAddrWidth.W))
  val loop_rec = RegInit(0.U(STORE_SET_CMD_RS2_LOOP_WIDTH.W))
  val loop = RegInit(0.U(STORE_SET_CMD_RS2_LOOP_WIDTH.W))
  val remote_addr = RegInit(0.U(coreMaxAddrBits.W)) // remote DRAM virtual address
  val spad_addr = RegInit(0.U(spadAddrWidth.W)) // scratchpad address from rs1
  val leftLen = RegInit(0.U(lgMaxDataLen.W)) // left data length
  val len_rec = RegInit(0.U(lgMaxDataLen.W))
  val status = Reg(new MStatus)
  val id = RegInit(0.U(idWidth.W))
  val success = RegInit(true.B)
  val st_rs1 = io.core.req.bits.cmd.rs1.asTypeOf(new StoreRs1)
  val st_rs2 = io.core.req.bits.cmd.rs2.asTypeOf(new StoreRs2)
  val stset_rs2 = io.core.req.bits.cmd.rs2.asTypeOf(new StoresetRs2)
  val funct = io.core.req.bits.cmd.inst.funct

  switch(state){
    is(s_idle){
      when(io.core.req.fire){
        when(funct === STORE_CMD){
          state := s_spad_req
          remote_addr := st_rs1.remote_addr
          spad_addr := st_rs2.spad_addr
          leftLen := st_rs2.total_len
          len_rec := st_rs2.total_len
          status := io.core.req.bits.cmd.status
          id := io.core.req.bits.id
          success := true.B
          loop := loop_rec
        }.elsewhen(funct === STORE_SET_CMD){
          state := s_idle
          loop_rec := stset_rs2.loop
          spad_stride := stset_rs2.spad_stride
          remote_stride := stset_rs2.remote_stride
        }
      }
    }
    is(s_spad_req){
      when(io.spad.req.fire){
        state := s_dma_req
      }
    }
    is(s_dma_req){
      when(io.dma.req.fire){
        state := s_stream
      }
    }
    is(s_stream){
      when(io.dma.exp.req){
        state := s_exp
        success := false.B
      }.elsewhen(io.spad.stream.fire){
        leftLen := leftLen - dataByte.U
        state := Mux(leftLen <= dataByte.U, Mux(loop <= 1.U, s_resp, s_loop), s_stream)
      }
    }
    is(s_exp){
      when(io.spad.exp.ack){
        state := Mux(loop <= 1.U, s_resp, s_loop)
      }
    }
    is(s_loop){
      state := s_spad_req
      loop := loop - 1.U
      remote_addr := remote_addr + remote_stride
      spad_addr := spad_addr + spad_stride
      leftLen := len_rec
    }
    is(s_resp){
      when(io.core.resp.fire){
        state := s_idle
      }
    }
  }

  io.core.req.ready := (state === s_idle)
  // Scratchpad request
  io.spad.req.valid := (state === s_spad_req)
  io.spad.req.bits.addrs(0) := spad_addr
  io.spad.req.bits.len := leftLen
  io.spad.req.bits.id := id
  // DMA request
  io.dma.req.valid := (state === s_dma_req)
  io.dma.req.bits.addr := remote_addr
  io.dma.req.bits.len := leftLen
  io.dma.req.bits.id := id
  io.dma.req.bits.status := status
  // data stream
  if(streamQueDepth == 0){
    io.spad.stream.ready := io.dma.stream.ready && (state === s_stream)
    io.dma.stream.valid := io.spad.stream.valid && (state === s_stream)
    io.dma.stream.bits := io.spad.stream.bits
  }else{
    val que = Module(new Queue(new DMAStream(dataWidth, hasMask, idWidth), streamQueDepth,
      false, false, false, true))
    io.spad.stream.ready := que.io.enq.ready && (state === s_stream)
    que.io.enq.valid := io.spad.stream.valid && (state === s_stream)
    que.io.enq.bits := io.spad.stream.bits
    io.dma.stream <> que.io.deq
    que.io.flush.get := io.spad.exp.req
  }
  // exception
  io.spad.exp.req := (state === s_exp)
  io.spad.exp.id := id
  io.dma.exp.ack := io.spad.exp.ack
  // response
  io.core.resp.valid := (state === s_resp)
  io.core.resp.bits.success := success
  io.core.resp.bits.id := id
}