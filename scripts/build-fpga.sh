#!/bin/bash

cd ../../fpga/
make SUB_PROJECT=vcu118 bitstream CONFIG=cdnnVCU118Config EXTRA_FIRRTL_OPTIONS=--emission-options=disableMemRandomization,disableRegisterRandomization
#make SUB_PROJECT=vcu118 verilog CONFIG=cdnnVCU118Config EXTRA_FIRRTL_OPTIONS="--emission-options=disableMemRandomization,disableRegisterRandomization"

