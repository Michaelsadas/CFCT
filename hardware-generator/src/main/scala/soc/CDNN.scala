package cdnn

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.diplomacy._
import cdnn.spec._
import cdnn.dsa._
import java.io.File

object cdnnParam { 
  val dumpSpec : Boolean = false
  val loadSpec : Boolean = false
  val dumpOperationSet : Boolean = true
  val dumpADG : Boolean = true
  val rootDirPath = (new File("")).getAbsolutePath()
  val cdnn_spec_filename = rootDirPath + "/generators/cdnn/hardware-generator/src/main/resources/cdnn_spec.json"
  val operation_set_filename = rootDirPath + "/generators/cdnn/hardware-generator/src/main/resources/operations.json"
  val cgra_adg_filename = rootDirPath + "/generators/cdnn/hardware-generator/src/main/resources/cgra_adg.json"
}

class cdnn(opcodes: OpcodeSet)(implicit p: Parameters) extends LazyRoCC(opcodes, nPTWPorts = 1) {
  import cdnnParam._
  if(dumpSpec){ cdnnSpec.dumpSpec(cdnn_spec_filename) }
  if(loadSpec){ cdnnSpec.loadSpec(cdnn_spec_filename) }
  cdnnSpec.attrs("dumpOperationSet") = dumpOperationSet
  if(dumpOperationSet){ cdnnSpec.attrs("operation_set_filename") = operation_set_filename }
  cdnnSpec.attrs("dumpOperationSet") = dumpADG
  if(dumpADG){ cdnnSpec.attrs("cgra_adg_filename") = cgra_adg_filename }
  // scratchpad banks used for IOB
  val lgSizeSpadBank = cdnnSpec.attrs("spad_bank_lg_size").asInstanceOf[Int]
  val nSpadBanks = cdnnSpec.attrs("spad_num_banks").asInstanceOf[Int]
  // scratchpad block used for Config
  val lgSizeSpadCfg = cdnnSpec.attrs("spad_cfg_lg_size").asInstanceOf[Int]
  val cfgSpadBanks = {
    if(lgSizeSpadCfg <= lgSizeSpadBank) 1
    else 1 << (lgSizeSpadCfg - lgSizeSpadBank)
  }
  val spadAddrWidth = lgSizeSpadBank + log2Ceil(nSpadBanks+cfgSpadBanks) // in bytes
  val lgMaxDataLen = spadAddrWidth
  val spadAddrNum = cdnnSpec.attrs("spad_addr_num").asInstanceOf[Int]
  val spadDataWidth = cdnnSpec.attrs("spad_data_width").asInstanceOf[Int]
  val cgraDataWidth = cdnnSpec.attrs("cgra_data_width").asInstanceOf[Int]
  val hasMask = false // spadDataWidth != cgraDataWidth
  val idWidth = cdnnSpec.attrs("id_width").asInstanceOf[Int]
  val nReqInflight = cdnnSpec.attrs("dma_num_req_in_flight").asInstanceOf[Int]
  val maxLgSizeTL = cdnnSpec.attrs("dma_lg_max_burst_size").asInstanceOf[Int]
  val nWaysOfTLB = cdnnSpec.attrs("tlb_num_ways").asInstanceOf[Int]
  val useSharedTLB = cdnnSpec.attrs("tlb_is_shared").asInstanceOf[Boolean]
//  val cmdQueDepth = cdnnSpec.attrs("rs_cmd_queue_depth").asInstanceOf[Int]
  val loadQueDepth = cdnnSpec.attrs("rs_load_queue_depth").asInstanceOf[Int]
  val storeQueDepth = cdnnSpec.attrs("rs_store_queue_depth").asInstanceOf[Int]
  val exeQueDepth = cdnnSpec.attrs("rs_exe_queue_depth").asInstanceOf[Int]
  val streamQueDepth = cdnnSpec.attrs("ls_stream_queue_depth").asInstanceOf[Int]
  
  override lazy val module = new cdnnModuleImp(this)
  val dma_node = LazyModule(new DMAController(lgMaxDataLen, spadDataWidth, hasMask, idWidth, nReqInflight, maxLgSizeTL, nWaysOfTLB, useSharedTLB))
  override val tlNode = dma_node.id_node
//  tlNode := dma_node.id_node
}



class cdnnModuleImp(outer: cdnn)(implicit p: Parameters) extends LazyRoCCModuleImp(outer)
  with HasCoreParameters {
  import outer._
  val float_type = new Float(expWidth = 5, sigWidth = 11)
  val sint_type = SInt(16.W)
  
//  val reservation = Module(new ReservationStation(cmdQueDepth, loadQueDepth, storeQueDepth, exeQueDepth, nSpadBanks, lgSizeSpadBank, idWidth))
  val reservation = Module(new ReservationStation(loadQueDepth, storeQueDepth, exeQueDepth, idWidth))
  val loader = Module(new LoadController(spadAddrWidth, spadAddrNum, lgMaxDataLen, spadDataWidth, hasMask, idWidth, streamQueDepth, loadQueDepth))
  val storer = Module(new StoreController(spadAddrWidth, lgMaxDataLen, spadDataWidth, hasMask, idWidth, streamQueDepth))
  val mover = Module(new MoveController(spadAddrWidth, spadAddrNum, lgMaxDataLen, spadDataWidth, hasMask, idWidth))
  val default = Module(new DefaultIO(spadAddrWidth, spadAddrNum, lgMaxDataLen, spadDataWidth, hasMask, idWidth))
  val spad = Module(new Scratchpad(spadAddrWidth, spadAddrNum, lgMaxDataLen, spadDataWidth, hasMask, idWidth, nSpadBanks, lgSizeSpadBank, lgSizeSpadCfg, cgraDataWidth))
  val cgra = Module(new CGRAController(sint_type, float_type, attrs = cdnnSpec.attrs))
  val dmaCtrl = outer.dma_node.module

  io.busy := reservation.io.busy
  io.resp <> reservation.io.resp
  reservation.io.cmd <> io.cmd
  reservation.io.ld <> loader.io.core
  reservation.io.st <> storer.io.core
  reservation.io.ex <> cgra.io.core
  reservation.io.mv <> mover.io.core
  loader.io.dma <> dmaCtrl.io.read
  storer.io.dma <> dmaCtrl.io.write
//**************************************************************
//  spad.io.read <> storer.io.spad
//  spad.io.write <> loader.io.spad
//**************************************************************
  loader.io.spad <> spad.io.write
  when(reservation.io.move){
    spad.io.read <> mover.io.spadread
    spad.io.write <> mover.io.spadwrite
    default.io.read <> storer.io.spad
    default.io.write <> loader.io.spad
  }.otherwise{
    spad.io.read <> storer.io.spad
    spad.io.write <> loader.io.spad
    default.io.read <> mover.io.spadread
    default.io.write <> mover.io.spadwrite
  }
  cgra.io.srams_iob <> spad.io.srams
  cgra.io.sram_cfg <> spad.io.sram_last
  io.ptw <> dmaCtrl.io.ptw
  dmaCtrl.io.exp.foreach(_.flush := reservation.io.flush)
  io.interrupt := dmaCtrl.io.exp.map(_.interrupt).reduce(_ || _)
  io.mem.req.valid := false.B

}

