package cdnn.dsa

import chisel3._
import chisel3.util._
import scala.math.pow

class Serial(width: Int, num: Int) extends Module {
    val io = IO(new Bundle {
        val cgra_in = Input(Vec(num, UInt(width.W)))
        val sram_in = Input(UInt(width.W))
        val out = Output(UInt(width.W))
        val cgra_en = Input(Bool())
        val sram_en = Input(Bool())
        val out_en = Input(Bool())
        val add_en = Input(Bool())
    })

    val regs = RegInit(VecInit(Seq.fill(num){0.U(width.W)}))
    for(i <- 0 until num) {
        when(io.cgra_en) {
            when (io.add_en){
                regs(i) := regs(i) + io.cgra_in(i)
            }.otherwise{
                regs(i) := io.cgra_in(i)
            }
        }
    }

    val ptrWidth = log2Ceil(num)
    val wptr = RegInit(0.U(ptrWidth.W))
    //val wptr_delay = RegNext(wptr)
    val rptr = RegInit(0.U(ptrWidth.W))

    when(!io.cgra_en && io.out_en) {
        io.out := regs(wptr)
        when(wptr < (num - 1).U){
            wptr := wptr + 1.U
        }.otherwise {
            wptr := 0.U
        }
    }.otherwise {
        io.out := 0.U
        wptr := 0.U
    }

    when(!io.cgra_en && io.sram_en) {
        when (rptr < (num - 1).U){
            rptr := rptr + 1.U
        }.otherwise {
            rptr := 0.U
        }
        regs(rptr) := io.sram_in
    }.otherwise {
        rptr := 0.U
    }
}

//object VerilogGen extends App {
//  (new chisel3.stage.ChiselStage).emitVerilog(new serial(32, 6),args)
//}