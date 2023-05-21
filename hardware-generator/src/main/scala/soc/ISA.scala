package cdnn

import chisel3._

object cdnnISA{
  // funct7 valued for different commands
  val FENCE_CMD = 0.U
  val LOAD_CMD = 1.U
  val STORE_CMD = 2.U
  val CFG_CMD = 3.U
  val EXE_CMD = 4.U
//  val CGRA_CFG_CMD = 3.U
//  val CGRA_EXE_CMD = 4.U
  val FLUSH_CMD = 5.U
  val RELOAD_CMD = 6.U
  val LOAD_SET_CMD = 7.U
  val STORE_SET_CMD = 8.U
  val MOVE_CMD = 9.U


  // Task ID for dependency analysis
  // One task may consist of load, cgra_cfg, cgra_exe, and store commands
  val TASK_ID_WIDTH = 8
  val DEP_TYPE_WIDTH = 3 // command dependence type
  val LD_DEP_ST_LAST_TASK = 1.U     // this load command depends on the store command of last task
  val LD_DEP_EX_LAST_TASK = 2.U     // this load command depends on the execute command of last task
  val LD_DEP_ST_LAST_SEC_TASK = 3.U // this load command depends on the store command of last second task
  val EX_DEP_ST_LAST_TASK = 1.U     // this EXECUTE command depends on the store command of last task

  // LOAD_CMD RS1
  val LOAD_CMD_RS1_REMOTE_ADDR_WIDTH = 64
  class LoadRs1 extends Bundle {
    val remote_addr = UInt(LOAD_CMD_RS1_REMOTE_ADDR_WIDTH.W)
  }

  // LOAD_CMD RS2
  val LOAD_CMD_RS2_SPAD_ADDR_WIDTH = 32
  val LOAD_CMD_RS2_TOTAL_LEN_WIDTH = 20
  val LOAD_CMD_RS2_UNUSE0_WIDTH = 2
  class LoadRs2 extends Bundle {
    val task_id = UInt(TASK_ID_WIDTH.W)
    val is_cfg = Bool()
    val _unuse0 = UInt(LOAD_CMD_RS2_UNUSE0_WIDTH.W)
    val fused = Bool()
    val total_len = UInt(LOAD_CMD_RS2_TOTAL_LEN_WIDTH.W)
    val spad_addr = UInt(LOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
  }

  // RELOAD_CMD RS1
  val RELOAD_CMD_RS1_REMOTE_ADDR_WIDTH = 64
  class ReLoadRs1 extends Bundle {
    val remote_addr = UInt(RELOAD_CMD_RS1_REMOTE_ADDR_WIDTH.W)
  }

  // RELOAD_CMD RS2
  val RELOAD_CMD_RS2_SPAD_ADDR_WIDTH = 32
  val RELOAD_CMD_RS2_UNUSE0_WIDTH = 23
  class ReLoadRs2 extends Bundle {
    val task_id = UInt(TASK_ID_WIDTH.W)
    val _unuse0 = UInt(RELOAD_CMD_RS2_UNUSE0_WIDTH.W)
    val spad_addr = UInt(RELOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
  }

  // LOAD_SET_CMD RS1
  val LOAD_SET_CMD_RS1_UNUSE0_WIDTH = 64 //TODO
  class LoadsetRs1 extends Bundle {
    val _unuse0 = UInt(LOAD_SET_CMD_RS1_UNUSE0_WIDTH.W)
  }

  // LOAD_SET_CMD RS2
  val LOAD_SET_CMD_RS2_LOOP_WIDTH = 20
  val LOAD_SET_CMD_RS2_SPAD_ADDR_STRIDE_WIDTH = 22
  val LOAD_SET_CMD_RS2_REMOTE_ADDR_STRIDE_WIDTH = 22
  class LoadsetRs2 extends Bundle {
    val loop = UInt(LOAD_SET_CMD_RS2_LOOP_WIDTH.W)
    val spad_stride = UInt(LOAD_SET_CMD_RS2_SPAD_ADDR_STRIDE_WIDTH.W)
    val remote_stride = UInt(LOAD_SET_CMD_RS2_REMOTE_ADDR_STRIDE_WIDTH.W)
  }

  // STORE_CMD RS1
  val STORE_CMD_RS1_REMOTE_ADDR_WIDTH = 64
  class StoreRs1 extends Bundle {
    val remote_addr = UInt(LOAD_CMD_RS1_REMOTE_ADDR_WIDTH.W)
  }

  // STORE_CMD RS2
  val STORE_CMD_RS2_SPAD_ADDR_WIDTH = 32
  val STORE_CMD_RS2_TOTAL_LEN_WIDTH = 20
  val STORE_CMD_RS2_UNUSE0_WIDTH = 4
  class StoreRs2 extends Bundle {
    val task_id = UInt(TASK_ID_WIDTH.W)
    val _unuse0 = UInt(STORE_CMD_RS2_UNUSE0_WIDTH.W)
    val total_len = UInt(LOAD_CMD_RS2_TOTAL_LEN_WIDTH.W)
    val spad_addr = UInt(LOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
  }

  // STORE_SET_CMD RS1
  val STORE_SET_CMD_RS1_UNUSE0_WIDTH = 64
  class StoresetRs1 extends Bundle {
    val _unuse0 = UInt(STORE_SET_CMD_RS1_UNUSE0_WIDTH.W)
  }

  // STORE_SET_CMD RS2
  val STORE_SET_CMD_RS2_LOOP_WIDTH = 20
  val STORE_SET_CMD_RS2_SPAD_ADDR_STRIDE_WIDTH = 22
  val STORE_SET_CMD_RS2_REMOTE_ADDR_STRIDE_WIDTH = 22
  class StoresetRs2 extends Bundle {
    val loop = UInt(STORE_SET_CMD_RS2_LOOP_WIDTH.W)
    val spad_stride = UInt(STORE_SET_CMD_RS2_SPAD_ADDR_STRIDE_WIDTH.W)
    val remote_stride = UInt(STORE_SET_CMD_RS2_REMOTE_ADDR_STRIDE_WIDTH.W)
  }

  // MOVE_CMD RS1
  val MOVE_CMD_SPAD_ADDR = 32
  val MOVE_CMD_TOTAL_LEN = 20
  val MOVE_CMD_RS1_UNUSE = 32
  val MOVE_CMD_RS2_UNUSE = 12
  class MoveRs1 extends Bundle {
    val _unuse0 = UInt(MOVE_CMD_RS1_UNUSE.W)
    val spad_addr_from = UInt(MOVE_CMD_SPAD_ADDR.W)
  }

  class MoveRs2 extends Bundle {
    val _unuse0 = UInt(MOVE_CMD_RS2_UNUSE.W)
    val total_len = UInt(MOVE_CMD_TOTAL_LEN.W)
    val spad_addr_to = UInt(MOVE_CMD_SPAD_ADDR.W)
  }

  // CFG_CMD RS2
  val CFG_CMD_RS2_BASE_ADDR_WIDTH = 32
  val CFG_CMD_RS2_NUM_WIDTH = 16
  val CFG_CMD_RS2_UNUSE0_WIDTH = 8
  class CfgRs2 extends Bundle {
    val task_id = UInt(TASK_ID_WIDTH.W)
    val _unuse0 = UInt(CFG_CMD_RS2_UNUSE0_WIDTH.W)
    val cfg_num = UInt(CFG_CMD_RS2_NUM_WIDTH.W)
    val cfg_base_addr = UInt(CFG_CMD_RS2_BASE_ADDR_WIDTH.W)
  }

  // EXE_CMD RS1
  val EXE_CMD_RS1_IOB_ENS_WIDTH = 64
  class ExeRs1 extends Bundle {
    val iob_ens = UInt(EXE_CMD_RS1_IOB_ENS_WIDTH.W)
  }

  // EXE_CMD RS2
  val EXE_CMD_RS2_UNUSE0_WIDTH = 56
  class ExeRs2 extends Bundle {
    val task_id = UInt(TASK_ID_WIDTH.W)
    val _unuse0 = UInt(EXE_CMD_RS2_UNUSE0_WIDTH.W)
  }
}