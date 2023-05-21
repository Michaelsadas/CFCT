package cdnn

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.rocket._
import cdnnISA._

class DefaultIO(spadAddrWidth: Int, spadAddrNum: Int, lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int)
                (implicit p: Parameters) extends CoreModule {
    val io = IO(new Bundle {
        val read = new SpadStreamReadIF(spadAddrWidth, lgMaxDataLen, dataWidth, hasMask, idWidth)
        val write = new SpadStreamWriteIF(spadAddrWidth, spadAddrNum, lgMaxDataLen, dataWidth, hasMask, idWidth)
    })
    //io.read.stream.bits.last = false.B
    //io.read.stream.bits.data = 0.U
    //io.read.stream.bits.id = 0.U
    //io.read.stream.bits.mask = false.B
    //io.read.stream.valid = false.B
    //io.read.req.ready = false.B
    //io.read.exp.ack = 0.U
//
    //io.write.req.ready = false.B
    //io.write.stream.ready = false.B
    //io.write.exp.ack = false.B
}