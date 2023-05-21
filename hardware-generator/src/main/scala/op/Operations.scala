package cdnn.op
// ALU/IOB operations

import chisel3._
import chisel3.util._
import scala.collection.mutable
import scala.collection.mutable.ListBuffer
import cdnn.ir._
import cdnn.dsa._

/**
 * Operation Code
 */
object OPC extends Enumeration {
	type OPC = Value
	// Operation Code
	val PASS,    // passthrough from in to out
		ADD,     // add
	  	SUB,     // substrate
	  	MUL,     // multiply
	  	DIV,     // divide
		RELU,
		RELU6,
	  	// MOD,     // modulo
	  	// MIN,
		MAX,
	  	// NOT,
		//	  AND,
		//	  OR,
		//	  XOR,
		//
	  	SHL,     // shift left
	  	LSHR,    // logic shift right
		ASHR,    // arithmetic shift right
		CSHL,    // cyclic shift left
		CSHR,    // cyclic shift right
	  	EQ,      // equal to
	  	NE,      // not equal to
	  	ULT,      // less than
	  	ULE,      // less than or equal to
		SEL,
		ACC,	 // +=
		ASUB,  // -=
		AMIN,
		AMAX,
		MAC,
		TOFLOAT,
		//	  AMUL,  // *=
		//		ADIV,  // /=
		//		AMOD,  // %=
		//		AAND,  // &=
		//		AOR,   // |=
		//		AXOR,  // ^=
		//		ASHL,  // <<=
		//		ALSHR, // >>=
		//		AASHR, // >>>=
		FPASS,
		FADD,
		FSUB,
		FMUL,
		FDIV,
		FMOD,
		FMIN,
		FMAX,
		FSHL,
		FLSHR,
		FEQ, 
		FNE, 
		FULT,
		FULE,
		FSLT,
		FSLE,
		FSEL,
		FACC,
		FASUB,
		FAMUL,
		FADIV,
		FAMOD,
		FAMIN,
		FAMAX,
		FASHL,
		FALSHR,
		FRELU,
		FRELU6,
		FMAC,
		TOINT,
		//
		INPUT,
		OUTPUT,
		LOAD,
		STORE,
		SSTORE = Value

	val numOPC = this.values.size - 4 // not including INPUT/OUTPUT/LOAD/STORE, since ALU OPC config width is determined by numOPC

	def printOPC = {
		this.values.foreach{ op => println(s"$op\t: ${op.id}")}
	}
}

/**
 *  Operation Information
 */
object OpInfo {
	import OPC._
	val OpInfoMap: Map[String, ListBuffer[Int]] = Map(
		// OPC -> List(NumOperands, NumRes, Latency, Operands-Commutative, Accumulative-operation)
		// latency including the register outside ALU
		"PASS" -> ListBuffer(1, 1, 1, 0, 0),
		"ADD" -> ListBuffer(2, 1, 1, 1, 0),
		"SUB" -> ListBuffer(2, 1, 1, 0, 0),
		"MUL" -> ListBuffer(2, 1, 1, 1, 0),
		"DIV" -> ListBuffer(2, 1, 1, 0, 0),
		"MOD" -> ListBuffer(2, 1, 1, 0, 0),
		"MIN" -> ListBuffer(2, 1, 1, 1, 0),
		"MAX" -> ListBuffer(2, 1, 1, 1, 0),
		"AND" -> ListBuffer(2, 1, 1, 1, 0),
		"OR"  -> ListBuffer(2, 1, 1, 1, 0),
		"XOR" -> ListBuffer(2, 1, 1, 1, 0),
		"SHL" -> ListBuffer(2, 1, 1, 0, 0),
		"LSHR" -> ListBuffer(2, 1, 1, 0, 0),
		"ASHR" -> ListBuffer(2, 1, 1, 0, 0),
		"CSHL" -> ListBuffer(2, 1, 1, 0, 0),
		"CSHR" -> ListBuffer(2, 1, 1, 0, 0),
		"EQ"  -> ListBuffer(2, 1, 1, 1, 0),
		"NE"  -> ListBuffer(2, 1, 1, 1, 0),
		"ULT" -> ListBuffer(2, 1, 1, 0, 0),
		"ULE" -> ListBuffer(2, 1, 1, 0, 0),
		"SLT" -> ListBuffer(2, 1, 1, 0, 0),
		"SLE" -> ListBuffer(2, 1, 1, 0, 0),
		"SEL" -> ListBuffer(3, 1, 1, 0, 0),
		"RELU" -> ListBuffer(1, 1, 1, 0, 0),
		"RELU6" -> ListBuffer(2, 1, 1, 0, 0),
		"TOFLOAT" -> ListBuffer(1, 1, 1, 0, 0),
//---------------------------------------------------------------------------------------------------------------------------------//
		"FPASS" -> ListBuffer(1, 1, 1, 0, 0),
		"FADD" -> ListBuffer(2, 1, 4, 1, 0),
		"FSUB" -> ListBuffer(2, 1, 4, 0, 0),
		"FMUL" -> ListBuffer(2, 1, 4, 1, 0),
		"FDIV" -> ListBuffer(2, 1, 1, 0, 0), //The latency of divide is based on the length of the float
		"FMOD" -> ListBuffer(2, 1, 1, 0, 0),
		"FMIN" -> ListBuffer(2, 1, 2, 1, 0),
		"FMAX" -> ListBuffer(2, 1, 2, 1, 0),
		"FSHL" -> ListBuffer(2, 1, 4, 0, 0),
		"FLSHR" -> ListBuffer(2, 1, 1, 0, 0),
		"FEQ"  -> ListBuffer(2, 1, 1, 1, 0),
		"FNE"  -> ListBuffer(2, 1, 1, 1, 0),
		"FULT" -> ListBuffer(2, 1, 1, 0, 0),
		"FULE" -> ListBuffer(2, 1, 1, 0, 0),
		"FSLT" -> ListBuffer(2, 1, 1, 0, 0),
		"FSLE" -> ListBuffer(2, 1, 1, 0, 0),
		"FSEL" -> ListBuffer(3, 1, 1, 0, 0),
		"FRELU" -> ListBuffer(1, 1, 1, 0, 0),
		"FRELU6" -> ListBuffer(2, 1, 1, 0, 0),
		"TOINT" -> ListBuffer(1, 1, 2, 0, 0),
//--------------------------------------------------------------------------------------------------------------------------------//
		"ACC"  -> ListBuffer(1, 1, 1, 0, 1),  // +=
		"ASUB" -> ListBuffer(1, 1, 1, 0, 1), // -=
		"AMUL" -> ListBuffer(1, 1, 1, 0, 1), // *=
		"ADIV" -> ListBuffer(1, 1, 1, 0, 1), // /=
		"AMOD" -> ListBuffer(1, 1, 1, 0, 1), // %=
		"AAND" -> ListBuffer(1, 1, 1, 0, 1), // &=
		"AMIN" -> ListBuffer(1, 1, 1, 0, 1), // 
		"AMAX" -> ListBuffer(1, 1, 1, 0, 1), // 
		"AOR"  -> ListBuffer(1, 1, 1, 0, 1),  // |=
		"AXOR" -> ListBuffer(1, 1, 1, 0, 1), // ^=
		"ASHL" -> ListBuffer(1, 1, 1, 0, 1), // <<=
		"ALSHR" -> ListBuffer(1, 1, 1, 0, 1),// >>=
		"AASHR" -> ListBuffer(1, 1, 1, 0, 1),// >>>=
		"MAC" -> ListBuffer(2, 1, 1, 1, 1), // += a * b
//--------------------------------------------------------------------------------------------------------------------------------//
		"FACC"  -> ListBuffer(1, 1, 4, 0, 1),  // +=
		"FASUB" -> ListBuffer(1, 1, 3, 0, 1), // -=
		"FAMUL" -> ListBuffer(1, 1, 3, 0, 1), // *=
		"FADIV" -> ListBuffer(1, 1, 1, 0, 1), // /=
		"FAMOD" -> ListBuffer(1, 1, 1, 0, 1), // %=
		"FAMIN" -> ListBuffer(1, 1, 1, 0, 1), //
		"FAMAX" -> ListBuffer(1, 1, 2, 0, 1), //
		"FASHL" -> ListBuffer(1, 1, 3, 0, 1), // <<=
		"FALSHR" -> ListBuffer(1, 1, 1, 0, 1),// >>=
		"FMAC" -> ListBuffer(2, 1, 4, 1, 1),
//-------------------------------------------------------------------------------------------------------------------------------//
		"INPUT"  -> ListBuffer(0, 1, 2, 0, 0),
		"OUTPUT" -> ListBuffer(1, 0, 1, 0, 0),
		"LOAD"   -> ListBuffer(1, 1, 2, 0, 0),
		"STORE"  -> ListBuffer(2, 0, 1, 0, 0),
		"SSTORE" -> ListBuffer(7, 0, 1, 0, 0)
	)

	def getOperandNum(op: OPC): Int = {
		OpInfoMap(op.toString)(0)
	}

	def getALUOperandNum(op: OPC): Int = {
		val name = op.toString
		if(isAccumulative(op) > 0){
			OpInfoMap(name)(0) + 1
		}else{
			OpInfoMap(name)(0)
		}
	}

	def getResNum(op: OPC): Int = {
		OpInfoMap(op.toString)(1)
	}

	def getLatency(op: OPC): Int = {
		OpInfoMap(op.toString)(2)
	}

	def setLatency(op: String, lat: Int): Unit = {
		OpInfoMap(op)(2) = lat
	}

	def isCommutative(op: OPC.OPC): Int = {
		OpInfoMap(op.toString)(3)
	}

	def isAccumulative(op: OPC.OPC): Int = {
		OpInfoMap(op.toString)(4)
	}

	def dumpOpInfo(filename: String): Unit = {
		val infos = ListBuffer[Map[String, Any]]();
		OPC.values.foreach{ op =>
			val info = Map(
				"name" -> op.toString,
				"OPC" -> op.id,
				"numOperands" -> getOperandNum(op),
				"numRes" -> getResNum(op),
				"latency" -> getLatency(op),
				"commutative" -> isCommutative(op),
				"accumulative" ->isAccumulative(op)
			)
			infos += info
		}
		val ops: mutable.Map[String,Any] = mutable.Map("Operations" -> infos)
		IRHandler.dumpIR(ops, filename)
	}

	private var width = 32
	private var high = width - 1

	def apply(opWidth: Int) = {
		width = opWidth
		high = width - 1
		this
	}

	def isAccOp(op: UInt) : Bool = {
		val table = OPC.values.map{ op =>
			op.id.U -> (isAccumulative(op) > 0).asBool
		}.toSeq
		MuxLookup(op, false.B, table)
	}

	def getCurrentOperandNum(op: UInt) : UInt = {
		val table = OPC.values.map{ op =>
			op.id.U -> getOperandNum(op).asUInt
		}.toSeq
		MuxLookup(op, 0.U, table)
	}

	def Float_OpFuncs(accType: Float, ops: Seq[UInt], opc: UInt, opset: ListBuffer[OPC]): Map[String, Seq[UInt]] = {
		val op0 = ops.head(high, 0)
		val op1 = {
			if(ops.size > 1) ops(1)(high, 0)
			else 0.U(1.W)
		}
		val op2 = {
			if(ops.size > 2) ops(2)
			else 0.U(1.W)
		}

		// shift number
		val shn = Mux(op1.asSInt > 0.S, op1, (-(op1.asSInt)).asUInt)
		val shn0 = op0(log2Ceil(width)-1, 0)
		val opa = op0.asTypeOf(accType)
		val opb = op1.asTypeOf(accType)
		val opd = op2.asTypeOf(accType)

		val muladder = Module(new MulAddRecFN(opa.expWidth, opa.sigWidth))
		muladder.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
        muladder.io.detectTininess := consts.tininess_afterRounding
		val muladder_out = fNFromRecFN(opa.expWidth, opa.sigWidth, muladder.io.out)

		val comparator = Module(new CompareRecFN(opa.expWidth, opa.sigWidth))
        comparator.io.signaling := false.B

		val opa_rec = recFNFromFN(opa.expWidth, opa.sigWidth, opa.bits)
		val opb_rec = recFNFromFN(opb.expWidth, opb.sigWidth, opb.bits)
		val opd_rec = recFNFromFN(opd.expWidth, opd.sigWidth, opd.bits)

        val in_to_rec_fn = Module(new INToRecFN(1, opa.expWidth, opa.sigWidth))
        in_to_rec_fn.io.signedIn := false.B
        in_to_rec_fn.io.in := 1.U
        in_to_rec_fn.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
        in_to_rec_fn.io.detectTininess := consts.tininess_afterRounding
        
        val one_rec = in_to_rec_fn.io.out

		val recFNtoIn = Module(new RecFNToIN(opa.expWidth, opa.sigWidth, opa.expWidth + opa.sigWidth))
        recFNtoIn.io.roundingMode := consts.round_min
        recFNtoIn.io.signedOut := true.B

        val intorecFN = Module(new INToRecFN(opa.expWidth + opa.sigWidth, opa.expWidth, opa.sigWidth))
        intorecFN.io.signedIn := true.B
        intorecFN.io.roundingMode := consts.round_min
        intorecFN.io.detectTininess := consts.tininess_afterRounding
		val intorecFN_out = fNFromRecFN(opa.expWidth, opa.sigWidth, intorecFN.io.out)

		Map(
			"PASS" -> Seq(op0),
			"FADD" -> {
				muladder.io.op := 0.U
                muladder.io.a := opa_rec
                muladder.io.b := one_rec
                muladder.io.c := opb_rec
				Seq(muladder_out)},
			
			"FSUB" -> {
				muladder.io.op := 1.U
                muladder.io.a := opa_rec
                muladder.io.b := one_rec
                muladder.io.c := opb_rec
				Seq(muladder_out)},

			"FMUL" -> {
				muladder.io.op := 0.U
                muladder.io.a := opa_rec
                muladder.io.b := opb_rec
                muladder.io.c := 0.U
				Seq(muladder_out)},

//			"FDIV" -> Seq((opa / opb).asUInt),
//			"FMOD"  -> Seq((opa % opb).asUInt),
//			"FMIN"  -> Seq(Mux(opa < opb, opa.asUInt, opb.asUInt)),
			"FMAX" -> {
				comparator.io.a := opa_rec
                comparator.io.b := opb_rec
				Seq(Mux(comparator.io.gt, opa.bits, opb.bits))
			},

			"FSHL" -> {
				val shift_exp = Wire(UInt(opa.expWidth.W))
				shift_exp := (opa.bias.S + op1(log2Ceil(width)-1, 0).asSInt).asUInt
				val shift_fn = Cat(0.U(1.W), shift_exp, 0.U((opa.sigWidth-1).W))
				val shift_rec = recFNFromFN(opa.expWidth, opa.sigWidth, shift_fn)
				muladder.io.op := 0.U
                muladder.io.a := opa_rec
                muladder.io.b := shift_rec
				muladder.io.c := 0.U
				Seq(muladder_out)}, 
//			"FLSHR" -> Seq((opa >> shn).asUInt),
//			"FEQ"   -> {
//				val res = opa === opb
//				Seq(res, res)},
//			"FNE"   -> {
//				val res = opa =/= opb
//				Seq(res, res)},
//			"FULT"  -> { // unsigned less than
//				val res = opa.asUnsigned < opb.asUnsigned
//				Seq(res, res)},
//			"FULE"  -> { // unsigned less than or equal
//				val res = opa.asUnsigned <= opb.asUnsigned
//				Seq(res, res)},
//			"FSLT"  -> { // signed less than
//				val res = opa < opb
//				Seq(res, res)},
//			"FSLE"  -> { // signed less than or equal
//				val res = opa <= opb
//				Seq(res, res)},
//			"FSEL"  -> {
//				Seq(Mux(op2(0).asBool, op1, op0)) },
//			"FRELU" -> {
//				Seq(opa.relu.asUInt)},
//			"FRELU6" -> {
//				Seq(opa.relu6(shn).asUInt)},
			"FACC"   -> {
				muladder.io.op := 0.U
                muladder.io.a := opa_rec
                muladder.io.b := one_rec
                muladder.io.c := opb_rec
				Seq(muladder_out)}, // the SECOND operand is the register value. the following is similar
			
//			"FASUB"  -> Seq((opb - opa).asUInt),
//			"FAMUL"  -> Seq((opb * opa).asUInt),
//			"FADIV"  -> Seq((opb / opa).asUInt),
//			"FAMOD"  -> Seq((opb % opa).asUInt),
//			"FAMIN"  -> Seq(Mux(opa < opb, opa.asUInt, opb.asUInt)),
			"FAMAX"	-> {
				comparator.io.a := opa_rec
                comparator.io.b := opb_rec
				Seq(Mux(comparator.io.gt, opa.bits, opb.bits))
			},
//			"FASHL"  -> Seq((opb << shn0).asUInt),
//			"FALSHR" -> Seq((opb >> shn0).asUInt),
			"FMAC"	 -> {
				muladder.io.op := 0.U
                muladder.io.a := opa_rec
                muladder.io.b := one_rec
                muladder.io.c := opb_rec
				Seq(muladder_out)},
			
			"TOINT"	 -> {
				recFNtoIn.io.in := opa_rec
				Seq(recFNtoIn.io.out)},

			"TOFLOAT"-> {
				intorecFN.io.in := opa.bits
				Seq(intorecFN_out)}
		)
	}

	def SInt_OpFuncs(accType: SInt, ops: Seq[UInt], opc: UInt, opset: ListBuffer[OPC])(implicit ev: Arithmetic[SInt]): Map[String, Seq[UInt]] = {
		import ev._
		val op0 = ops.head(high, 0)
		val op1 = {
			if(ops.size > 1) ops(1)(high, 0)
			else 0.U(1.W)
		}
		val op2 = {
			if(ops.size > 2) ops(2)
			else 0.U(1.W)
		}

		// shift number
		val shn = Mux(op1.asSInt > 0.S, op1(log2Ceil(width)-1, 0), ((-(op1.asSInt)).asUInt)(log2Ceil(width)-1, 0))
		val shn0 = op0(log2Ceil(width)-1, 0)
		val opa = op0.asTypeOf(accType)
		val opb = op1.asTypeOf(accType)
		val opd = op2.asTypeOf(accType)
		Map(
			"PASS" -> Seq(op0),
			"ADD"  -> Seq((opa + opb).asUInt),
			"SUB"  -> Seq((opa - opb).asUInt),
			"MUL"  -> Seq((opa * opb).asUInt),
			"DIV" -> Seq((opa / opb).asUInt),
			"MOD"  -> Seq((opa % opb).asUInt),
			"MIN"  -> Seq(Mux(opa < opb, opa.asUInt, opb.asUInt)),
			"MAX"  -> Seq(Mux(opa > opb, opa.asUInt, opb.asUInt)),
			"AND"  -> Seq(op0 & op1),
			"OR"   -> Seq(op0 | op1),
			"XOR"  -> Seq(op0 ^ op1),
			"SHL"  -> Seq(Mux(op1.asSInt > 0.S, (opa << shn).asUInt, (opa >> shn).asUInt)),
			"LSHR" -> Seq((opa >> shn).asUInt),
			"CSHL" -> {
				val res1 = (op0 << shn).asUInt
				val res2 = (op0 >> (width.U - shn)).asUInt
				Seq(res1 | res2) },
			"CSHR" -> {
				val res1 = (op0 >> shn).asUInt
				val res2 = (op0 << (width.U - shn)).asUInt
				Seq(res1 | res2) },
			"EQ"   -> {
				val res = opa === opb
				Seq(res, res)},
			"NE"   -> {
				val res = opa =/= opb
				Seq(res, res)},
			"ULT"  -> { // unsigned less than
				val res = opa.asUnsigned < opb.asUnsigned
				Seq(res, res)},
			"ULE"  -> { // unsigned less than or equal
				val res = opa.asUnsigned <= opb.asUnsigned
				Seq(res, res)},
			"SLT"  -> { // signed less than
				val res = opa < opb
				Seq(res, res)},
			"SLE"  -> { // signed less than or equal
				val res = opa <= opb
				Seq(res, res)},
			"SEL"  -> {
				Seq(Mux(op2(0).asBool, op1, op0)) },
			"RELU" -> {
				Seq(opa.relu.asUInt)},
			"RELU6" -> {
				Seq(opa.relu6(shn).asUInt)},
			"ACC"   -> Seq((opb + opa).asUInt), // the SECOND operand is the register value. the following is similar
			"ASUB"  -> Seq((opb - opa).asUInt),
			"AMUL"  -> Seq((opb * opa).asUInt),
			"ADIV"  -> Seq((opb / opa).asUInt),
			"AMOD"  -> Seq((opb % opa).asUInt),
			"AMIN"  -> Seq(Mux(opa < opb, opa.asUInt, opb.asUInt)),
			"AMAX"	-> Seq(Mux(opa > opb, opa.asUInt, opb.asUInt)),
			"AAND"  -> Seq(op1 & op0),
			"AOR"   -> Seq(op1 | op0),
			"AXOR"  -> Seq(op1 ^ op0),
			"ASHL"  -> Seq((opa << shn0).asUInt),
			"ALSHR" -> Seq((opb >> shn0).asUInt),
			"MAC"	-> Seq(opd.mac(opa, opb).asUInt),
			"TOFLOAT" -> Seq(opa.toFloat.bits)
		)
	}
}


//object test extends App {
//	println(OPC.values)
//	println(s"OPC number: ${OPC.numOPC}")
//	println(OPC(0), OPC.withName("ADD"))
//	OPC.printOPC
//	val outFilename = "src/main/resources/operations.json"
//	OpInfo.dumpOpInfo(outFilename)
//	// println(OpInfo.OpFuncMap(OPC.ADD)(Seq(1.U, 2.U)))
//}