package cdnn

import chisel3._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.subsystem.SystemBusKey


class Withcdnn extends Config((site, here, up) => {
  case BuildRoCC => up(BuildRoCC) ++ Seq( 
    (p: Parameters) => {
      val cdnn = LazyModule(new cdnn(OpcodeSet.custom0)(p))
      cdnn
    }
  )
  case SystemBusKey => up(SystemBusKey).copy(beatBytes = 8)
})

class WithDualcdnn extends Config((site, here, up) => {
  case BuildRoCC => List(
    (p: Parameters) => {
      val cdnn0 = LazyModule(new cdnn(OpcodeSet.custom0)(p))
      cdnn0
    },
    (p: Parameters) => {
      val cdnn1 = LazyModule(new cdnn(OpcodeSet.custom1)(p))
      cdnn1
    })
  case SystemBusKey => up(SystemBusKey).copy(beatBytes = 8)
})