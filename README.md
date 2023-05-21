CDNN Framework
=======================

Template-based Reconfigurable Architecture Modeling Framework, including:

1. hardware-generator: CGRA modeling and generation based on Chisel. Design a flexible CGRA template, and generate architecture IR and Verilog. 

2. cgra-mapper: CGRA mapper, mapping DFGs to CGRA in batches. The mapping flow includes placement and routing, data synchronization, optimization, visualization, and configuration generation.

3. Benchmarks: DFGs in Json format derived from dot files.


## Getting Started


### Dependencies

##### JDK 8 or newer (for hardware-generator)

##### SBT (for hardware-generator)

##### CMake  (for cgra-mapper)

##### C++-11 (for cgra-mapper)


### hardware-generator

#### Build and run

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




