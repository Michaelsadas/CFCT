package cdnn.dsa
//This file is actually not used in this design
import chisel3._
import chisel3.experimental._
import chisel3.util._

case class Float(expWidth: Int, sigWidth: Int) extends Bundle {
  val bits = UInt((expWidth + sigWidth).W)

  val bias: Int = (1 << (expWidth-1)) - 1
}

object Float {
    def apply(expWidth: Int, sigWidth: Int, bits: UInt): Float = {
        val result = Wire(new Float(expWidth, sigWidth))
        result.bits := bits
        result
    }
}

class Complex(val w: Int) extends Bundle {
  val real = SInt(w.W)
  val imag = SInt(w.W)
}

object Complex {
  def apply(w: Int, real: SInt, imag: SInt): Complex = {
    val result = Wire(new Complex(w))
    result.real := real
    result.imag := imag
    result
  }
}

abstract class Arithmetic[T <: Data] {
    implicit def cast(t: T): ArithmeticOps[T]
}
abstract class ArithmeticOps[T <: Data](self: T) {
    def *(t: T): T
    def +(t: T): T
    def -(t: T): T
    def /(t: T): T
    def %(t: T): T
    def <(t: T): Bool
    def >(t: T): Bool
    def <<(t: UInt): T
    def >>(t: UInt): T
    def asUnsigned: T
    def ===(t: T): Bool
    def =/=(t: T): Bool
    def <=(t: T): Bool
    def >=(t: T): Bool
    def asUInt: UInt
    def relu: T
    def relu6(shift: UInt): T
    def mac(m1: T, m2: T): T
    def toInt: UInt
    def toFloat: Float
}

object Arithmetic {
    implicit object FloatArithmetic extends Arithmetic[Float]{
        override implicit def cast(self: Float): ArithmeticOps[Float] = new ArithmeticOps(self) {
            override def *(t: Float): Float = {
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val multiplier = Module(new MulRecFN(self.expWidth, self.sigWidth))

                multiplier.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
                multiplier.io.detectTininess := consts.tininess_afterRounding

                multiplier.io.a := self_rec
                multiplier.io.b := t_rec

                val out = Wire(new Float(self.expWidth, self.sigWidth))
                out.bits := fNFromRecFN(self.expWidth, self.sigWidth, multiplier.io.out)
                out
            }

            override def /(t: Float): Float = {
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val divider = Module(new DivSqrtRecFN_small(self.expWidth, self.sigWidth, 0))

                divider.io.inValid := divider.io.inReady
                divider.io.sqrtOp := false.B
                divider.io.a := self_rec
                divider.io.b := t_rec
                divider.io.roundingMode := consts.tininess_afterRounding
                divider.io.detectTininess := consts.tininess_afterRounding

                val out = Wire(new Float(self.expWidth, self.sigWidth))
                out.bits := fNFromRecFN(self.expWidth, self.sigWidth, divider.io.out)
                out                
            }

            override def +(t: Float): Float = {
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val adder = Module(new AddRecFN(self.expWidth, self.sigWidth))

                adder.io.subOp := 0.U
                adder.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
                adder.io.detectTininess := consts.tininess_afterRounding

                adder.io.a := self_rec
                adder.io.b := t_rec

                val out = Wire(new Float(self.expWidth, self.sigWidth))
                out.bits := fNFromRecFN(self.expWidth, self.sigWidth, adder.io.out)
                out
            }

            override def -(t: Float): Float = {
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val adder = Module(new AddRecFN(self.expWidth, self.sigWidth))

                adder.io.subOp := 1.U
                adder.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
                adder.io.detectTininess := consts.tininess_afterRounding

                adder.io.a := self_rec
                adder.io.b := t_rec

                val out = Wire(new Float(self.expWidth, self.sigWidth))
                out.bits := fNFromRecFN(self.expWidth, self.sigWidth, adder.io.out)
                out
            }

            override def %(t: Float): Float = self

            override def >(t: Float): Bool = {
                // Recode all operands
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val comparator = Module(new CompareRecFN(self.expWidth, self.sigWidth))
                comparator.io.a := self_rec
                comparator.io.b := t_rec
                comparator.io.signaling := false.B

                comparator.io.gt
            }

            override def <(t: Float): Bool = {
                // Recode all operands
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val comparator = Module(new CompareRecFN(self.expWidth, self.sigWidth))
                comparator.io.a := self_rec
                comparator.io.b := t_rec
                comparator.io.signaling := false.B

                comparator.io.lt
            }

            override def ===(t: Float): Bool = {
                // Recode all operands
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val comparator = Module(new CompareRecFN(self.expWidth, self.sigWidth))
                comparator.io.a := self_rec
                comparator.io.b := t_rec
                comparator.io.signaling := false.B

                comparator.io.eq
            }

            override def =/=(t: Float): Bool = {
                // Recode all operands
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val comparator = Module(new CompareRecFN(self.expWidth, self.sigWidth))
                comparator.io.a := self_rec
                comparator.io.b := t_rec
                comparator.io.signaling := false.B

                !comparator.io.eq
            }

            override def <=(t: Float): Bool = {
                // Recode all operands
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val comparator = Module(new CompareRecFN(self.expWidth, self.sigWidth))
                comparator.io.a := self_rec
                comparator.io.b := t_rec
                comparator.io.signaling := false.B

                comparator.io.eq || comparator.io.lt
            }

            override def >=(t: Float): Bool = {
                // Recode all operands
                val t_rec = recFNFromFN(t.expWidth, t.sigWidth, t.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                val comparator = Module(new CompareRecFN(self.expWidth, self.sigWidth))
                comparator.io.a := self_rec
                comparator.io.b := t_rec
                comparator.io.signaling := false.B

                comparator.io.eq || comparator.io.gt
            }

            override def >>(u: UInt): Float = {
                // Recode self
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                // Get 2^(-u) as a recoded float
                val shift_exp = Wire(UInt(self.expWidth.W))
                //val bias: Int = (1 << (self.expWidth-1)) - 1
                shift_exp := self.bias.U - u
                val shift_fn = Cat(0.U(1.W), shift_exp, 0.U((self.sigWidth-1).W))
                val shift_rec = recFNFromFN(self.expWidth, self.sigWidth, shift_fn)

                // Multiply self and 2^(-u)
                val multiplier = Module(new MulRecFN(self.expWidth, self.sigWidth))

                multiplier.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
                multiplier.io.detectTininess := consts.tininess_afterRounding

                multiplier.io.a := self_rec
                multiplier.io.b := shift_rec

                val result = Wire(new Float(self.expWidth, self.sigWidth))
                result.bits := fNFromRecFN(self.expWidth, self.sigWidth, multiplier.io.out)
                result
            }

            override def <<(u: UInt): Float = {
                // Recode self
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                // Get 2^(+u) as a recoded float
                val shift_exp = Wire(UInt(self.expWidth.W))
                //val bias: Int = (1 << (self.expWidth-1)) - 1
                shift_exp := self.bias.U + u
                val shift_fn = Cat(0.U(1.W), shift_exp, 0.U((self.sigWidth-1).W))
                val shift_rec = recFNFromFN(self.expWidth, self.sigWidth, shift_fn)

                // Multiply self and 2^(-u)
                val multiplier = Module(new MulRecFN(self.expWidth, self.sigWidth))

                multiplier.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
                multiplier.io.detectTininess := consts.tininess_afterRounding

                multiplier.io.a := self_rec
                multiplier.io.b := shift_rec

                val result = Wire(new Float(self.expWidth, self.sigWidth))
                result.bits := fNFromRecFN(self.expWidth, self.sigWidth, multiplier.io.out)
                result
            }

            override def asUnsigned: Float = {
                val result = Wire(new Float(self.expWidth, self.sigWidth))
                result.bits := Cat(0.U, self.bits(self.expWidth + self.sigWidth - 2, 0))
                result
            }

            override def asUInt: UInt = {
                self.bits
            }

            override def relu: Float = {
                val raw = rawFloatFromFN(self.expWidth, self.sigWidth, self.bits)

                val result = Wire(Float(self.expWidth, self.sigWidth))
                result.bits := Mux(!raw.isZero && raw.sign, 0.U, self.bits)
                result
            }

            override def relu6(shift: UInt): Float = {
                // Get a constant 6 as a float
                val in_to_rec_fn = Module(new INToRecFN(log2Up(6+1), self.expWidth, self.sigWidth))
                in_to_rec_fn.io.signedIn := false.B
                in_to_rec_fn.io.in := 6.U
                in_to_rec_fn.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
                in_to_rec_fn.io.detectTininess := consts.tininess_afterRounding

                val six_rec = in_to_rec_fn.io.out

                // Get 2^shift as a float
                val shift_exp = self.bias.U(self.expWidth.W) + shift
                val shift_fn = Cat(0.U(1.W), shift_exp, 0.U((self.sigWidth-1).W))
                val shift_rec = recFNFromFN(self.expWidth, self.sigWidth, shift_fn)

                // Get 6*(2^shift) as a float
                val muladder = Module(new MulAddRecFN(self.expWidth, self.sigWidth))

                muladder.io.op := 0.U
                muladder.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
                muladder.io.detectTininess := consts.tininess_afterRounding

                muladder.io.a := six_rec
                muladder.io.b := shift_rec
                muladder.io.c := 0.U

                val shifted_rec = muladder.io.out

                // Now, compare self and 6*(2^shift) to calculate the activation function
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)
                val self_raw = rawFloatFromFN(self.expWidth, self.sigWidth, self.bits)

                val comparer = Module(new CompareRecFN(self.expWidth, self.sigWidth))
                comparer.io.a := self_rec
                comparer.io.b := shifted_rec
                comparer.io.signaling := false.B

                val larger_than_six = comparer.io.gt

                val result_rec = Mux(!self_raw.isZero && self_raw.sign, 0.U,
                Mux(larger_than_six, shifted_rec, self_rec))

                val result = Wire(Float(self.expWidth, self.sigWidth))
                result.bits := fNFromRecFN(self.expWidth, self.sigWidth, result_rec)
                result
            }

            override def mac(m1: Float, m2: Float): Float = {
                // Recode all operands
                val m1_rec = recFNFromFN(m1.expWidth, m1.sigWidth, m1.bits)
                val m2_rec = recFNFromFN(m2.expWidth, m2.sigWidth, m2.bits)
                val self_rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)

                // Perform multiply-add
//                val muladder = Module(new MulAddRecFNPipe(2, self.expWidth, self.sigWidth))
                val muladder = Module(new MulAddRecFN(self.expWidth, self.sigWidth))

//                muladder.io.validin := true.B
                muladder.io.op := 0.U
                muladder.io.roundingMode := consts.round_near_even // consts.round_near_maxMag
                muladder.io.detectTininess := consts.tininess_afterRounding

                muladder.io.a := m1_rec
                muladder.io.b := m2_rec
                muladder.io.c := self_rec

                val out = Wire(Float(self.expWidth, self.sigWidth))
                out.bits := fNFromRecFN(self.expWidth, self.sigWidth, muladder.io.out)
                out
            }

            override def toInt: UInt = {
                val rec = recFNFromFN(self.expWidth, self.sigWidth, self.bits)
                val recFNtoIn = Module(new RecFNToIN(self.expWidth, self.sigWidth, self.expWidth + self.sigWidth)) // Assuming float and SInt have the smae data width
                recFNtoIn.io.in := rec
                recFNtoIn.io.roundingMode := consts.round_min
                recFNtoIn.io.signedOut := true.B

                val out = Wire(UInt((self.expWidth + self.sigWidth).W))
                out := recFNtoIn.io.out
                out
            }

            override def toFloat: Float = { // In this case, we first regard the input as SInt and turns it to float.
                val intorecFN = Module(new INToRecFN(self.expWidth + self.sigWidth, self.expWidth, self.sigWidth))

                intorecFN.io.in := self.bits
                intorecFN.io.signedIn := true.B
                intorecFN.io.roundingMode := consts.round_min
                intorecFN.io.detectTininess := consts.tininess_afterRounding

                val out = Wire(Float(self.expWidth, self.sigWidth))
                out.bits := fNFromRecFN(self.expWidth, self.sigWidth, intorecFN.io.out)
                out
            }
        }
    }

    implicit object SIntArithmetic extends Arithmetic[SInt]{
        override implicit def cast(self: SInt): ArithmeticOps[SInt] = new ArithmeticOps(self) {
            override def *(t: SInt) = self * t
            override def +(t: SInt) = self + t
            override def -(t: SInt) = self - t
            override def /(t: SInt) = self / t 
            override def %(t: SInt) = self % t 
            override def <(t: SInt) = self < t
            override def >(t: SInt) = self > t
            override def <<(t: UInt) = self << t 
            override def >>(t: UInt) = self >> t
            override def asUnsigned = Mux(self > 0.S, self, -self)
            override def ===(t: SInt) = self === t
            override def =/=(t: SInt) = self =/= t
            override def <=(t: SInt) = self <= t
            override def >=(t: SInt) = self >= t
            override def asUInt = self.asUInt
            override def relu = Mux(self >= 0.S, self, 0.S)
            override def relu6(shift: UInt): SInt = {
                val max6 = (6.S << shift).asSInt()
                val maxwidth = ((1 << (self.getWidth-1))-1).S
                val max = Mux(max6 > maxwidth, maxwidth, max6)(self.getWidth-1, 0).asSInt()
                MuxCase(self, Seq((self < 0.S) -> 0.S, (self > max) -> max))
            }
            override def mac(m1: SInt, m2: SInt) = m1 * m2 + self
            override def toFloat: Float = {
                val intorecFN = Module(new INToRecFN(32, 8, 24)) //TODO 8, 24, 32 should be replaced with variables
                intorecFN.io.in := self.asUInt
                intorecFN.io.signedIn := true.B
                intorecFN.io.roundingMode := consts.round_min
                intorecFN.io.detectTininess := consts.tininess_afterRounding

                val out = Wire(Float(8, 24)) //TODO 8, 24, 32 should be replaced with variables
                out.bits := fNFromRecFN(8, 24, intorecFN.io.out) //TODO 8, 24, 32 should be replaced with variables
                out
            }
            override def toInt: UInt = self.asUInt
        }
    }

}