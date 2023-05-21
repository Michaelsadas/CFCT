package cdnn

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.rocket._
import testchipip.StreamChannel
import cdnnISA._

class CoreReq(idWidth: Int)(implicit p: Parameters) extends Bundle {
  val cmd = new RoCCCommand
  val id = UInt(idWidth.W)
}

class CoreResp(idWidth: Int)(implicit p: Parameters) extends Bundle {
  val success = Bool()
  val id = UInt(idWidth.W)
}

class CoreIF(idWidth: Int)(implicit p: Parameters) extends Bundle {
  val req = Flipped(Decoupled(new CoreReq(idWidth)))
  val resp = Decoupled(new CoreResp(idWidth))
}

object Tool {
  class UDValid[T <: Data](t: T) extends Bundle {
    val valid = Bool()
    val bits = t.cloneType

    def push(b: T): Unit = {
      valid := true.B
      bits := b
    }

    def pop(dummy: Int = 0): T = {
      valid := false.B
      bits
    }
  }

  object UDValid {
    def apply[T <: Data](t: T): UDValid[T] = new UDValid(t)
  }
}

/** Reservation Station
  * @param cmdQueDepth    command queue depth
  * @param loadQueDepth   load queue depth
  * @param storeQueDepth  store queue depth
  * @param exeQueDepth    execute queue depth
  * @param idWidth        width of the command ID
  */
class ReservationStation(loadQueDepth: Int, storeQueDepth: Int, exeQueDepth: Int, idWidth: Int)
                    (implicit p: Parameters) extends CoreModule {
  val io = IO(new Bundle {
    val cmd = Flipped(Decoupled(new RoCCCommand))
    val resp = Decoupled(new RoCCResponse)
    val ld = Flipped(new CoreIF(idWidth))
    val st = Flipped(new CoreIF(idWidth))
    val ex = Flipped(new CoreIF(idWidth))
    val mv = Flipped(new CoreIF(idWidth))
    val busy = Output(Bool()) // has command not completed
    val flush = Output(Bool())
    val move = Output(Bool())
  })

  // id is composed of sub_task_id and task_id.
  val taskIdWidth = cdnnISA.TASK_ID_WIDTH
  assert(idWidth == taskIdWidth)
  val count_length = 24
  val cfgQueDepth = 3

  val count = RegInit(1.U(count_length.W))
  when(io.cmd.fire) {
    count := count + 1.U
  }

  class Output_rec extends Bundle {
    val count = UInt(count_length.W)
    val addr = UInt(LOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
  }

  class Input_rec extends Bundle {
    val count = UInt(count_length.W)
    val addr = UInt(LOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
  }

  class Config_rec extends Bundle {
    val count = UInt(count_length.W)
    val start = UInt(LOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
    val end = UInt(LOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
  }

  class entry extends Bundle {
    val count = UInt(count_length.W)
    val cmd = new RoCCCommand 
  }

  val ld_cmd_que = Module(new Queue(new entry, loadQueDepth))
  val st_cmd_que = Module(new Queue(new entry, storeQueDepth))
  val ex_cmd_que = Module(new Queue(new entry, exeQueDepth))
  val cfg_counter_que = Module(new Queue(UInt(count_length.W), cfgQueDepth))
  val input_grp = Reg(Vec(loadQueDepth + loadQueDepth, Tool.UDValid(new Input_rec)))
  val output_grp = Reg(Vec(storeQueDepth, Tool.UDValid(new Output_rec)))
  val cfg_grp = Reg(Vec(cfgQueDepth, Tool.UDValid(new Config_rec)))
  val input_full = input_grp.map(_.valid).reduce(_ && _)
  val output_full = output_grp.map(_.valid).reduce(_ && _)
  val cfg_full = cfg_grp.map(_.valid).reduce(_ && _)//TODO

  val s_idle :: s_flush :: s_enq :: s_fence :: Nil = Enum(4)
  val state = RegInit(s_idle)
  val funct = io.cmd.bits.inst.funct
  val isLoadCmd = (funct === LOAD_CMD)
  val isReloadCmd = (funct === RELOAD_CMD)
  val isLoadsetCmd = (funct === LOAD_SET_CMD)
  val isCfgCmd = (funct === CFG_CMD)
  val isExeCmd = (funct === EXE_CMD)
  val isStoreCmd = (funct === STORE_CMD)
  val isStoresetCmd = (funct === STORE_SET_CMD)
  val isFlushCmd = (funct === FLUSH_CMD)
  val isFenceCmd = (funct === FENCE_CMD)
  val isMoveCmd = (funct === MOVE_CMD)
  val flush = RegInit(false.B)
  val req_rd = Reg(chiselTypeOf(io.resp.bits.rd))
  val req_xd = RegInit(false.B)
  val cmd_ready = RegInit(false.B)

//*************************************************************************************************************************************************
  //receive the order from the host
  io.flush := flush
  io.cmd.ready := cmd_ready
  io.busy := (state =/= s_idle) || io.cmd.valid
  switch(state) {
    is(s_idle) {
      when(io.cmd.valid){
        when(isFlushCmd) {
          state := s_flush
          req_rd := io.cmd.bits.inst.rd
          cmd_ready := true.B
          flush := true.B
        }.elsewhen(isFenceCmd && !ld_cmd_que.io.deq.valid && !ex_cmd_que.io.deq.valid && !st_cmd_que.io.deq.valid){ // previous commands have all been completed
          state := s_fence
          req_rd := io.cmd.bits.inst.rd
          req_xd := io.cmd.bits.inst.xd
          cmd_ready := true.B
        }.elsewhen(((isLoadCmd || isReloadCmd || isLoadsetCmd) && ld_cmd_que.io.enq.ready && !input_full && !cfg_full) || // push to load command queue
                   ((isMoveCmd && ld_cmd_que.io.enq.ready && st_cmd_que.io.enq.ready && !input_full && !output_full)) ||
                   ((isCfgCmd || isExeCmd) && ex_cmd_que.io.enq.ready) ||  // push to execute command queue
                   ((isStoreCmd || isStoresetCmd) && st_cmd_que.io.enq.ready)){ // push to store command queue
          state := s_enq
          cmd_ready := true.B
        }
      }
    }
    is(s_enq){
      cmd_ready := false.B
      state := s_idle
    }
    is(s_fence){
      cmd_ready := false.B
      when(!req_xd || io.resp.ready){
        state := s_idle
      }
    }
    is(s_flush){
      cmd_ready := false.B
      when(io.resp.ready){
        state := s_idle
        flush := false.B
      }
    }
  }

  io.resp.valid := (state === s_fence && req_xd)
  io.resp.bits.rd := req_rd
  io.resp.bits.data := 1.U
//**************************************************************************************************************************************
  val ld_count = ld_cmd_que.io.deq.bits.count
  val ex_count = ex_cmd_que.io.deq.bits.count
  val st_count = st_cmd_que.io.deq.bits.count
  val ld_addr = ld_cmd_que.io.deq.bits.cmd.rs2.asTypeOf(new LoadRs2).spad_addr
  val ld_end = ld_cmd_que.io.deq.bits.cmd.rs2.asTypeOf(new LoadRs2).spad_addr + ld_cmd_que.io.deq.bits.cmd.rs2.asTypeOf(new LoadRs2).total_len
  val isldCfg = ld_cmd_que.io.deq.bits.cmd.rs2.asTypeOf(new LoadRs2).is_cfg
  val iscfgReq = ex_cmd_que.io.deq.bits.cmd.inst.funct === CFG_CMD
  val iscfgCmd = io.cmd.bits.rs2.asTypeOf(new LoadRs2).is_cfg === true.B
  val isLoadsetReq = ld_cmd_que.io.deq.bits.cmd.inst.funct === LOAD_SET_CMD
  val isMoveReq = ld_cmd_que.io.deq.bits.cmd.inst.funct === MOVE_CMD
  val isLoadReq = ld_cmd_que.io.deq.bits.cmd.inst.funct === LOAD_CMD
  val isStoresetReq = st_cmd_que.io.deq.bits.cmd.inst.funct === STORE_SET_CMD
  val fused = ld_cmd_que.io.deq.bits.cmd.rs2.asTypeOf(new LoadRs2).fused

  // push into the group or queue
  ld_cmd_que.io.enq.valid := io.cmd.fire && (isLoadCmd || isLoadsetCmd || isMoveCmd)
  ld_cmd_que.io.enq.bits.cmd := io.cmd.bits
  ld_cmd_que.io.enq.bits.count := count
  ex_cmd_que.io.enq.valid := io.cmd.fire && (isCfgCmd || isExeCmd)
  ex_cmd_que.io.enq.bits.cmd := io.cmd.bits
  ex_cmd_que.io.enq.bits.count := count
  st_cmd_que.io.enq.valid := io.cmd.fire && (isStoreCmd || isStoresetCmd)
  st_cmd_que.io.enq.bits.cmd := io.cmd.bits
  st_cmd_que.io.enq.bits.count := count
  cfg_counter_que.io.enq.valid := io.cmd.fire && isLoadCmd && iscfgCmd
  cfg_counter_que.io.enq.bits := count
  cfg_counter_que.io.deq.ready := io.ex.resp.fire && iscfgReq
  val cfg_count = cfg_counter_que.io.deq.bits

//**********************************************************************************************************************************
  // set the working principle of the input group
  val input_location = MuxCase((loadQueDepth + loadQueDepth - 1).U, input_grp.zipWithIndex.map { case (e, i) => !e.valid -> i.U })
  when(io.cmd.fire && ((isLoadCmd && !iscfgCmd) || isReloadCmd || isMoveCmd)) {
    input_grp(input_location).valid := true.B
    input_grp(input_location).bits.count := count
    input_grp(input_location).bits.addr := Mux(isMoveCmd, io.cmd.bits.rs2.asTypeOf(new MoveRs2).spad_addr_to, io.cmd.bits.rs2.asTypeOf(new LoadRs2).spad_addr)
  }

  when(io.ex.resp.fire && !iscfgReq) {
    input_grp.foreach{ case (e) =>
      e.valid := e.valid && (e.bits.count > ex_count)
    }
  }

  // set the working principle of the config queue
  val cfg_location = MuxCase(2.U, cfg_grp.zipWithIndex.map { case (e, i) => !e.valid -> i.U })
  when(io.cmd.fire && isLoadCmd && iscfgCmd) {
    cfg_grp(cfg_location).valid := true.B
    cfg_grp(input_location).bits.start := io.cmd.bits.rs2.asTypeOf(new LoadRs2).spad_addr
    cfg_grp(input_location).bits.count := count
  }
  when(io.ex.resp.fire && iscfgReq) {
    cfg_grp.foreach{ case (e) =>
      e.valid := e.valid && (e.bits.count > ex_count)
    }
  }

  // set the working principle of the output group
  val output_location = MuxCase((storeQueDepth - 1).U, output_grp.zipWithIndex.map { case (e, i) => !e.valid -> i.U })
  when(io.cmd.fire && (isStoreCmd || isMoveCmd)) {
    output_grp(input_location).valid := true.B
    output_grp(input_location).bits.addr := Mux(isMoveCmd, io.cmd.bits.rs1.asTypeOf(new MoveRs1).spad_addr_from, io.cmd.bits.rs2.asTypeOf(new StoreRs2).spad_addr)
    output_grp(input_location).bits.count := count
  }  

  when(io.st.resp.fire) {
    output_grp.foreach{ case(e) =>
      e.valid := e.valid && (e.bits.count > st_count)
    }
  }

  when(io.mv.resp.fire) {
    output_grp.foreach{ case(e) =>
      e.valid := e.valid && (e.bits.count > ld_count)
    }    
  }
//***************************************************************************************************************************************
  //issue the command
  val st_permitted = RegInit(true.B)
  val ld_permitted = RegInit(true.B)
  val ex_permitted = RegInit(true.B)
  val ld_cfg_wait = cfg_grp.map(e => e.valid && e.bits.count < ld_count &&
                                          (e.bits.start <= ld_end && ld_end <= e.bits.end) || (e.bits.start <= ld_addr && ld_addr <= e.bits.end)).reduce(_ || _)
  val ld_data_wait1 = input_grp.map(e => e.valid && e.bits.addr === ld_addr && e.bits.count < ld_count).reduce(_ || _)
  val ld_data_wait2 = output_grp.map(e => e.valid && e.bits.addr === ld_addr && e.bits.count < ld_count).reduce(_ || _)
  val mv_wait =  ld_count > ex_count && ex_cmd_que.io.deq.valid

  val s_load_idle :: s_load_set :: s_load_cfg :: s_load_data :: s_move :: s_wait :: Nil = Enum(6)
  val ld_state = RegInit(s_load_idle)
  val move = RegInit(false.B)
  val store_fence = RegInit(false.B)
  switch(ld_state) {
    is(s_load_idle) {
      when(ld_cmd_que.io.deq.valid){
        when(isLoadReq && isldCfg && !ld_cfg_wait){
          ld_state := s_load_cfg
        }.elsewhen(isLoadReq && !fused && !isldCfg && !ld_data_wait1 && !ld_data_wait2){
          ld_state := s_load_data
        }.elsewhen(isLoadsetReq || (isLoadReq && fused)){
          ld_state := s_load_set
        }.elsewhen(isMoveReq){
          when(!mv_wait && !ld_data_wait1 && !ld_data_wait2){
            store_fence := true.B
            when (st_permitted){
              ld_state := s_move
              move := true.B
            }
          }
        }
      }
    }
    is(s_load_cfg){
      when(io.ld.req.fire){
        ld_state := s_wait
      }
    }
    is(s_load_data){
      when(io.ld.req.fire){
        ld_state := s_wait
      }
    }
    is(s_load_set){
      when(io.ld.req.fire){
        ld_state := s_idle
      }
    }
    is(s_move){
      when(io.mv.req.fire){
        ld_state := s_wait
      }
    }
    is(s_wait){
      when(io.ld.resp.fire){
        ld_state := s_idle;
      }.elsewhen(io.mv.resp.fire){
        ld_state := s_idle
        move := false.B
        store_fence := false.B
      }
    }
  }

  io.ld.req.valid := (ld_state === s_load_cfg) || (ld_state === s_load_data) || (ld_state === s_load_set)
  io.ld.req.bits.cmd := ld_cmd_que.io.deq.bits.cmd
  io.ld.resp.ready := true.B
  ld_cmd_que.io.deq.ready := io.ld.resp.fire || (io.ld.req.fire && fused) || (io.ld.req.fire && isLoadsetReq) || io.mv.resp.fire
  io.mv.req.valid := (ld_state === s_move)
  io.mv.req.bits.cmd := ld_cmd_que.io.deq.bits.cmd
  io.mv.resp.ready := true.B
  io.move := move
  
  val cfg_wait = iscfgReq && ex_count > cfg_count && ld_count <= cfg_count && ld_cmd_que.io.deq.valid
  val exe_wait1 = !iscfgReq && ex_count > ld_count && ld_cmd_que.io.deq.valid
  val exe_wait2 = !iscfgReq && output_grp.map( out_e => 
                                                out_e.valid && out_e.bits.count > ex_count && output_grp.map( e => 
                                                  e.valid && e.bits.addr === out_e.bits.addr && e.bits.count < ex_count
                                                ).reduce(_ || _) //TODO
                                            ).reduce(_ || _)
  io.ex.req.valid := ex_cmd_que.io.deq.valid && !cfg_wait && !exe_wait1 && !exe_wait2 && ex_permitted
  io.ex.req.bits.cmd := ex_cmd_que.io.deq.bits.cmd
  io.ex.resp.ready := true.B
  ex_cmd_que.io.deq.ready := io.ex.resp.fire
  when(io.ex.req.fire){
    ex_permitted := false.B
  }
  when(io.ex.resp.fire){
    ex_permitted := true.B
  }

  val st_wait =  st_count > ex_count && ex_cmd_que.io.deq.valid
  io.st.req.valid := st_cmd_que.io.deq.valid && !st_wait && st_permitted && !store_fence
  io.st.req.bits.cmd := st_cmd_que.io.deq.bits.cmd
  io.st.resp.ready := true.B
  st_cmd_que.io.deq.ready := io.st.resp.fire || (io.st.req.fire && isStoresetReq)
  when(io.st.req.fire){
    st_permitted := false.B || isStoresetReq
  }
  when(io.st.resp.fire){
    st_permitted := true.B
  }
}