CDNN Framework
=======================

Template-based Reconfigurable Architecture Modeling Framework, including:

1. hardware-generator: CGRA modeling and generation based on Chisel. Design a flexible CGRA template, and generate architecture IR and Verilog. 

2. cgra-mapper: CGRA mapper, mapping DFGs to CGRA in batches. The mapping flow includes placement and routing, data synchronization, optimization, visualization, and configuration generation.

3. fronted-compiler: automatically extracting the computing kernels in C language, and output a DFG file that can be recognized by cgra-mapper. 

4. Benchmarks: DFGs of multiple DNN computing kernels in Json format derived from dot files.


## Getting Started


### Dependencies

##### JDK 8 or newer (for hardware-generator)

##### SBT (for hardware-generator)

##### CMake  (for cgra-mapper)

##### C++-11 (for cgra-mapper)

#### Build your hardware and run

Using the script run.sh
```sh
cd hardware-generator
./run.sh
```

Or using sbt command:
```sh
cd hardware-generator
sbt "runMain mg.CGRAMG -td ./test_run_dir"
```

### cgra-mapper

#### Build

Using the script build.sh
```sh
cd cgra-mapper
./build.sh
```

#### Run

Using the script run.sh
```sh
./run.sh
```

Change the benchmark file path as you need.
The generated result files are in the same directory as the benchmark.

#### How to design your own hardware?

CDNN provides a large amount of parameters that can be changed in the design-time to meet the demand of different accelerators.
The parameters are specified in the file CDNN/hardware-generator/src/main/scala/spec/CDNNSpec.scala. 
Some of the important parameters are explained as following:

cgra_num_row: The row number of the PEs
cgra_num_column: The column number of the PEs

cgra_data_width: The data width
cgra_cfg_data_width: The width of the configuration data

cgra_fpe_operations: The operations supported in the fpe. 
cgra_spe_operations: The operations supported in the spe. 
(currently we designs two kinds of PEs, fpe can handle data type of int and float while spe can only solve data type of int, users can specify their own PEs. The supported operations library is listed in CDNN/hardware-generator/src/main/scala/op/Operations.scala)

cgra_gpe_in_from_dir: High-level description of CGRA topology
cgra_gpe_out_to_dir: High-level description of CGRA topology
cgra_gib_num_track: num of the tracks of GIB
cgra_gib_connect_flexibility: High-level description of CGRA topology

#### How to add your own logic to the PEs?
step1: Add your logic name in ```object OPC``` in operations.scala

step2: Give out your logic parameters in ```object OpInfo``` in operations.scala

step3: Add your logic in ```object Float_OpFuncs``` in operations.scala

step4: Choose your own logic name in 'cgra_fpe_operations' or 'cgra_spe_operations' parameter in CDNNSpec.scala

### How to generate DFG and call functions for target DNN computing kernels?
Design the DNN computing kernels in C language and put it to CDNN/fronted-compiler/applications and generate the DFG files according to the README.md in CDNN/fronted-compiler/

Then, run the cgra-mapper. The corresponding configuration data and call function will be generated in the same directory of DFG. The call function is composed of series of instructions including configuration, data transmission and execution.

