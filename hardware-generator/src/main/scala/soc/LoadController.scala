package cdnn

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.rocket._
import cdnnISA._


/** Load controller, transfer data from remote memory (virtual address) to local scratchpad (physical address)
  * @param spadAddrWidth  scratchpad address width, <= 32
  * @param spadAddrNum    scratchpad address number, be multiple to support broadcast write
  * @param lgMaxDataLen   log2(the max data length in one DMA request)
  * @param dataWidth      data width in bits
  * @param hasMask        if has mask signal in the stream interface
  * @param idWidth        width of the ID for a DMA request
  * @param streamQueDepth stream queue depth, can be zero
  * @param reqQueDepth    request queue depth
  */
class LoadController(spadAddrWidth: Int, spadAddrNum: Int, lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int, streamQueDepth: Int, reqQueDepth: Int)
                    (implicit p: Parameters) extends CoreModule {
  val io = IO(new Bundle {
    val core = new CoreIF(idWidth)
    val dma = Flipped(new DMAStreamReadIF(lgMaxDataLen, dataWidth, hasMask, idWidth))
    val spad = Flipped(new SpadStreamWriteIF(spadAddrWidth, spadAddrNum, lgMaxDataLen, dataWidth, hasMask, idWidth))
  })

  // decouple the DMA requests with the data streams
  class SpadReq(spadAddrWidth: Int, spadAddrNum: Int, lgMaxDataLen: Int, idWidth: Int) extends Bundle{
    val addrs = Vec(spadAddrNum, UInt(spadAddrWidth.W))   // start addresses
    val num = UInt(log2Ceil(spadAddrNum).W) // used address number - 1
    val len = UInt(lgMaxDataLen.W)
    val loop = UInt(LOAD_SET_CMD_RS2_LOOP_WIDTH.W)
    val id = UInt(idWidth.W)
  }
  val spadReqQue = Module(new Queue(new SpadReq(spadAddrWidth, spadAddrNum, lgMaxDataLen, idWidth), reqQueDepth))

  val dataByte = dataWidth/8
  val s_idle :: s_dma_req :: Nil = Enum(2)
  val state = RegInit(s_idle)
  val remote_stride = RegInit(0.U(coreMaxAddrBits.W))
  val spad_stride = RegInit(0.U(spadAddrWidth.W))
  val loop_rec = RegInit(0.U(LOAD_SET_CMD_RS2_LOOP_WIDTH.W))
  val loop = RegInit(0.U(LOAD_SET_CMD_RS2_LOOP_WIDTH.W))
  val remote_addr = RegInit(0.U(coreMaxAddrBits.W)) // remote DRAM virtual address
//  val spad_addr = RegInit(0.U(spadAddrWidth.W)) // scratchpad address from rs1
  val spad_addrs = RegInit(VecInit(Seq.fill(spadAddrNum){0.U(spadAddrWidth.W)})) // scratchpad address from rs1
  val addr_num = RegInit(0.U(log2Ceil(spadAddrNum).W))
  val total_len = RegInit(0.U(lgMaxDataLen.W)) // total data length
  val fused = RegInit(false.B) // can be fused with next load command, with the same remote_addr, total_len
  val status = Reg(new MStatus)
  val id = RegInit(0.U(idWidth.W))
  val ld_rs1 = io.core.req.bits.cmd.rs1.asTypeOf(new LoadRs1)
  val ld_rs2 = io.core.req.bits.cmd.rs2.asTypeOf(new LoadRs2)
  val ldset_rs2 = io.core.req.bits.cmd.rs2.asTypeOf(new LoadsetRs2)
  val funct = io.core.req.bits.cmd.inst.funct

  switch(state){
    is(s_idle){
      when(io.core.req.fire){
        when(funct === LOAD_CMD){
          remote_addr := ld_rs1.remote_addr
          spad_addrs(addr_num) := ld_rs2.spad_addr
          total_len := ld_rs2.total_len
          fused := ld_rs2.fused.asBool
          status := io.core.req.bits.cmd.status
          loop := loop_rec
          id := io.core.req.bits.id
          when(ld_rs2.fused.asBool && addr_num < (spadAddrNum-1).U){
            state := s_idle
            addr_num := addr_num + 1.U
          }.otherwise{
            state := s_dma_req
          }
        }.elsewhen(funct === LOAD_SET_CMD){
          state := s_idle
          loop_rec := ldset_rs2.loop
          spad_stride := ldset_rs2.spad_stride
          remote_stride := ldset_rs2.remote_stride
        }
      }
    }
    is(s_dma_req){
      when(io.dma.req.fire){
        when(loop <= 1.U){
          state := s_idle
          addr_num := 0.U
        }.otherwise{
          state := s_dma_req
          loop := loop - 1.U
          remote_addr := remote_addr + remote_stride
          spad_addrs.foreach{ case (sp_addr) => 
          sp_addr := sp_addr + spad_stride
          }
        }
      }
    }
  }

  io.core.req.ready := (state === s_idle)
  // DMA request
  io.dma.req.valid := (state === s_dma_req) && spadReqQue.io.enq.ready
  io.dma.req.bits.addr := remote_addr
  io.dma.req.bits.len := total_len
  io.dma.req.bits.id := id
  io.dma.req.bits.status := status
  // Cache request in Queue
  spadReqQue.io.enq.valid := (state === s_dma_req) && io.dma.req.ready
  spadReqQue.io.enq.bits.addrs := spad_addrs
  spadReqQue.io.enq.bits.num := addr_num
  spadReqQue.io.enq.bits.len := total_len
  spadReqQue.io.enq.bits.id := id
  spadReqQue.io.enq.bits.loop := loop

  // Data streams
  val s_spad_req :: s_spad_stream :: s_exp :: s_resp :: Nil = Enum(4)
  val spadState = RegInit(s_spad_req)
  val leftLen = RegInit(0.U(lgMaxDataLen.W)) // left data length
  val spadReqId = RegInit(0.U(idWidth.W))
  val spadloop = RegInit(0.U(LOAD_SET_CMD_RS2_LOOP_WIDTH.W))
  val success = RegInit(true.B)
  val leftNum = RegInit(0.U(log2Ceil(spadAddrNum).W))

  switch(spadState){
    is(s_spad_req){
      when(io.spad.req.fire){
        spadState := s_spad_stream
        leftLen := spadReqQue.io.deq.bits.len
        leftNum := spadReqQue.io.deq.bits.num
        spadReqId := spadReqQue.io.deq.bits.id
        spadloop := spadReqQue.io.deq.bits.loop
        success := true.B
      }
    }
    is(s_spad_stream){
      when(io.dma.exp.req){
        spadState := s_exp
        success := false.B
      }.elsewhen(io.dma.stream.fire){
        leftLen := leftLen - dataByte.U
        spadState := Mux(leftLen <= dataByte.U, Mux(spadloop <= 1.U, s_resp, s_spad_req), s_spad_stream)
      }
    }
    is(s_exp){
      when(io.spad.exp.ack){
        spadState := Mux(spadloop <= 1.U, s_resp, s_spad_req)
      }
    }
    is(s_resp){
      when(io.core.resp.fire){
//        leftNum := leftNum - 1.U
//        when(leftNum === 0.U){
//          spadState := s_spad_req
//        }
        spadState := s_spad_req
      }
    }
  }

  // read request from Queue
  spadReqQue.io.deq.ready := (spadState === s_spad_req) && io.spad.req.ready
  // Scratchpad request
  io.spad.req.valid := (spadState === s_spad_req) && spadReqQue.io.deq.valid
  io.spad.req.bits.addrs := spadReqQue.io.deq.bits.addrs
  io.spad.req.bits.num := spadReqQue.io.deq.bits.num
  io.spad.req.bits.len := spadReqQue.io.deq.bits.len
  io.spad.req.bits.id := spadReqQue.io.deq.bits.id
  // Exception
  io.spad.exp.req := (spadState === s_exp)
  io.spad.exp.id := spadReqId
  io.dma.exp.ack := io.spad.exp.ack
  // data stream
  if(streamQueDepth == 0){
    io.dma.stream.ready := io.spad.stream.ready && (spadState === s_spad_stream)
    io.spad.stream.valid := io.dma.stream.valid && (spadState === s_spad_stream)
    io.spad.stream.bits := io.dma.stream.bits
  }else{
    val que = Module(new Queue(new DMAStream(dataWidth, hasMask, idWidth), streamQueDepth,
      false, false, false, true))
    io.dma.stream.ready := que.io.enq.ready && (spadState === s_spad_stream)
    que.io.enq.valid := io.dma.stream.valid && (spadState === s_spad_stream)
    que.io.enq.bits := io.dma.stream.bits
    io.spad.stream <> que.io.deq
    que.io.flush.get := io.spad.exp.req
  }
  // response
  io.core.resp.valid := (spadState === s_resp)
  io.core.resp.bits.success := success
  io.core.resp.bits.id := spadReqId

}