package cdnn.dsa

import chisel3._
import chisel3.util._
import scala.collection.mutable.ListBuffer
import cdnn.op._


/** reconfigurable arithmetic unit
 * 
 * @param width   data width
 */
class ALU(accType: SInt, width: Int, ops: ListBuffer[OPC.OPC]) extends Module {
  val op_info = OpInfo(width)
  val maxNumOperands = ops.map(OpInfo(width).getALUOperandNum(_)).max
  val cfgDataWidth = log2Ceil(OPC.numOPC)
  val io = IO(new Bundle {
    val config = Input(UInt(cfgDataWidth.W))
    val in = Input(Vec(maxNumOperands, UInt(width.W)))
    val out = Output(UInt(width.W)) 
  })

  val op_func_map = op_info.SInt_OpFuncs(accType, io.in.toSeq, io.config, ops)
  val op2res = ops.map{op => 
    (op.id.U -> op_func_map(op.toString)(0))
  }

  io.out := MuxLookup(io.config, 0.U, op2res)

}

class FPU(accType: Float, width: Int, ops: ListBuffer[OPC.OPC]) extends Module {
  val op_info = OpInfo(width)
  val maxNumOperands = ops.map(OpInfo(width).getALUOperandNum(_)).max
  val cfgDataWidth = log2Ceil(OPC.numOPC)
  val io = IO(new Bundle {
    val config = Input(UInt(cfgDataWidth.W))
    val in = Input(Vec(maxNumOperands, UInt(width.W)))
    val out = Output(UInt(width.W)) 
  })
//"PASS", "FADD", "FMUL", "FACC", "FSUB", "FMAC", "FMAX", "FAMAX", "FMIN", "FSHL", "TOINT"
  val op0 = io.in(0)
	val op1 = {
		if(maxNumOperands > 1) io.in(1)
		else 0.U(1.W)
	}
	val op2 = {
		if(maxNumOperands > 2) io.in(2)
		else 0.U(1.W)
	}

		// shift number
	val shn = Mux(op1.asSInt > 0.S, op1, (-(op1.asSInt)).asUInt)
	val shn0 = op0(log2Ceil(width)-1, 0)
	val opa = op0.asTypeOf(accType)
	val opb = op1.asTypeOf(accType)
	val opd = op2.asTypeOf(accType)

	val opa_rec = RegInit(0.U((width + 1).W))
	val opb_rec = RegInit(0.U((width + 1).W))
	val opd_rec = RegInit(0.U((width + 1).W))

  opa_rec := recFNFromFN(opa.expWidth, opa.sigWidth, opa.bits)
  opb_rec := recFNFromFN(opb.expWidth, opb.sigWidth, opb.bits)
  opd_rec := recFNFromFN(opd.expWidth, opd.sigWidth, opd.bits)

  val shift_exp = Wire(UInt(opa.expWidth.W))
	shift_exp := (opa.bias.S + op1(4, 0).asSInt).asUInt
	val shift_fn = Cat(0.U(1.W), shift_exp, 0.U((opa.sigWidth-1).W))
	val shift_rec = RegInit(0.U((width + 1).W))
  shift_rec := recFNFromFN(opa.expWidth, opa.sigWidth, shift_fn)

  val in_to_rec_fn = Module(new INToRecFN(1, opa.expWidth, opa.sigWidth))
  in_to_rec_fn.io.signedIn := false.B
  in_to_rec_fn.io.in := 1.U
  in_to_rec_fn.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
  in_to_rec_fn.io.detectTininess := consts.tininess_afterRounding
        
  val one_rec = in_to_rec_fn.io.out

	//val muladder = Module(new MulAddRecFNPipe(opa.expWidth, opa.sigWidth))
	//muladder.io.op := Mux(io.config === OPC.FSUB.id.U, 1.U, 0.U)
  //muladder.io.a := opa_rec
  //muladder.io.b := Mux(io.config === OPC.FADD.id.U || io.config === OPC.FSUB.id.U || io.config === OPC.FACC.id.U, one_rec, Mux(io.config === OPC.FMUL.id.U || io.config === OPC.FMAC.id.U, opb_rec, shift_rec))
  //muladder.io.c := Mux(io.config === OPC.FADD.id.U || io.config === OPC.FSUB.id.U || io.config === OPC.FACC.id.U, opb_rec, Mux(io.config === OPC.FMAC.id.U, opd_rec, 0.U))
	//muladder.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
  //muladder.io.detectTininess := consts.tininess_afterRounding
	//val muladder_out = fNFromRecFN(opa.expWidth, opa.sigWidth, muladder.io.out)

	val muladder = Module(new MulAddRecFNPipe(2, opa.expWidth, opa.sigWidth))
	muladder.io.op := Mux(io.config === OPC.FSUB.id.U, 1.U, 0.U)
  muladder.io.a := opa_rec
  muladder.io.b := Mux(io.config === OPC.FADD.id.U || io.config === OPC.FSUB.id.U || io.config === OPC.FACC.id.U, one_rec, Mux(io.config === OPC.FMUL.id.U || io.config === OPC.FMAC.id.U, opb_rec, shift_rec))
  muladder.io.c := Mux(io.config === OPC.FADD.id.U || io.config === OPC.FSUB.id.U || io.config === OPC.FACC.id.U, opb_rec, Mux(io.config === OPC.FMAC.id.U, opd_rec, 0.U))
	muladder.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
  muladder.io.detectTininess := consts.tininess_afterRounding
	val muladder_out = fNFromRecFN(opa.expWidth, opa.sigWidth, muladder.io.out)

	val comparator = Module(new CompareRecFN(opa.expWidth, opa.sigWidth))
  comparator.io.a := opa_rec
  comparator.io.b := opb_rec
  comparator.io.signaling := false.B

	val recFNtoIn = Module(new RecFNToIN(opa.expWidth, opa.sigWidth, opa.expWidth + opa.sigWidth))
  recFNtoIn.io.in := opa_rec
  recFNtoIn.io.roundingMode := consts.round_min
  recFNtoIn.io.signedOut := true.B

  val intorecFN = Module(new INToRecFN(opa.expWidth + opa.sigWidth, opa.expWidth, opa.sigWidth))
  intorecFN.io.in := opa.bits
  intorecFN.io.signedIn := true.B
  intorecFN.io.roundingMode := consts.round_min
  intorecFN.io.detectTininess := consts.tininess_afterRounding
	val intorecFN_out = fNFromRecFN(opa.expWidth, opa.sigWidth, intorecFN.io.out)

  io.out := Mux(io.config === OPC.FMIN.id.U, Mux(comparator.io.gt, opb.bits, opa.bits), Mux(io.config === OPC.FMAX.id.U || io.config === OPC.FAMAX.id.U, Mux(comparator.io.gt, opa.bits, opb.bits), Mux(io.config === OPC.TOINT.id.U, recFNtoIn.io.out, Mux(io.config === OPC.TOFLOAT.id.U, intorecFN_out, muladder_out))))
}
//class FPU(accType: Float, width: Int, ops: ListBuffer[OPC.OPC]) extends Module {
//  val op_info = OpInfo(width)
//  val maxNumOperands = ops.map(OpInfo(width).getALUOperandNum(_)).max
//  val cfgDataWidth = log2Ceil(OPC.numOPC)
//  val io = IO(new Bundle {
//    val config = Input(UInt(cfgDataWidth.W))
//    val in = Input(Vec(maxNumOperands, UInt(width.W)))
//    val out = Output(UInt(width.W)) 
//  })
//  val op_func_map = op_info.Float_OpFuncs(accType, io.in.toSeq, io.config, ops)
//  val op2res = ops.map{op => 
//    (op.id.U -> op_func_map(op.toString)(0))
//  }
//
//  io.out := MuxLookup(io.config, 0.U, op2res)
//


//object VerilogGen extends App {
//   (new chisel3.stage.ChiselStage).emitVerilog(new ALU(Float(expWidth = 8, sigWidth = 24), 64, ListBuffer(OPC.ADD, OPC.SUB, OPC.SEL)),args)
//}
