// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/rocc.h"

#define FENCE_CMD 0
#define LOAD_CMD 1
#define STORE_CMD 2
#define CFG_CMD 3
#define EXE_CMD 4
//#define CGRA_CFG_CMD 3
//#define CGRA_EXE_CMD 4
#define FLUSH_CMD 5
#define RELOAD_CMD 6
#define LOAD_SET_CMD 7
#define STORE_SET_CMD 8

static inline int load_cfg(void *dram_ptr, int spad_ptr, int len, int target_tram)
{
//	uint64_t value;
	uint64_t rs2 = ((uint64_t) 1 << 55) | ((uint64_t) len << 32) | (uint64_t) spad_ptr;
	if (target_tram == 0)
		ROCC_INSTRUCTION_SS(0, (uintptr_t) dram_ptr, rs2, LOAD_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_SS(1, (uintptr_t) dram_ptr, rs2, LOAD_CMD);
	return 0;
}

static inline int load_data(void *dram_ptr, int spad_ptr, int len, int fused, int target_tram)
{
//	uint64_t value;
	uint64_t rs2 = ((uint64_t) fused << 52) | ((uint64_t) len << 32) | (uint64_t) spad_ptr;
	if (target_tram == 0)
		ROCC_INSTRUCTION_SS(0, (uintptr_t) dram_ptr, rs2, LOAD_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_SS(1, (uintptr_t) dram_ptr, rs2, LOAD_CMD);
	return 0;
}

static inline int reload_data(int spad_ptr, int target_tram)
{
	uint64_t rs2 = (uint64_t) spad_ptr;
	if (target_tram == 0)
		ROCC_INSTRUCTION_SS(0, 0, rs2, RELOAD_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_SS(1, 0, rs2, RELOAD_CMD);
	return 0;
}

static inline int load_set(int loop, int spad_stride, int remote_stride, int target_tram)
{
	uint64_t rs2 = ((uint64_t) loop << 44) | ((uint64_t) spad_stride << 22) | (uint64_t) remote_stride;
	if (target_tram == 0)
		ROCC_INSTRUCTION_SS(0, 0, rs2, LOAD_SET_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_SS(1, 0, rs2, LOAD_SET_CMD);
	return 0;
}

static inline int store(void *dram_ptr, int spad_ptr, int len, int target_tram)
{
//	uint64_t value;
	uint64_t rs2 = ((uint64_t) len << 32) | (uint64_t) spad_ptr;
	if (target_tram == 0)
		ROCC_INSTRUCTION_SS(0, (uintptr_t) dram_ptr, rs2, STORE_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_SS(1, (uintptr_t) dram_ptr, rs2, STORE_CMD);
	return 0;
}

static inline int store_set(int loop, int spad_stride, int remote_stride, int target_tram)
{
	uint64_t rs2 = ((uint64_t) loop << 44) | ((uint64_t) spad_stride << 22) | (uint64_t) remote_stride;
	if (target_tram == 0)
		ROCC_INSTRUCTION_SS(0, 0, rs2, STORE_SET_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_SS(1, 0, rs2, STORE_SET_CMD);
	return 0;
}

static inline int config(int cfg_base_addr, int cfg_num, int target_tram)
{
	uint64_t rs1 = 0;
	uint64_t rs2 = ((uint64_t) cfg_num << 32) | (uint64_t)cfg_base_addr;
	if (target_tram == 0)
		ROCC_INSTRUCTION_SS(0, rs1, rs2, CFG_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_SS(1, rs1, rs2, CFG_CMD);
	return 0;
}

static inline int execute(uint64_t iob_ens, int target_tram)
{
	uint64_t rs1 = (uint64_t) iob_ens;
	uint64_t rs2 = 0;
	if (target_tram == 0)
		ROCC_INSTRUCTION_SS(0, rs1, rs2, EXE_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_SS(1, rs1, rs2, EXE_CMD);
	return 0;
}

//static inline int config(int iob_ens, int loop_cycles, int cfg_num, int cfg_base_addr, int task_id)
//{
//	uint64_t rs1 = (uint64_t) iob_ens;
//	uint64_t rs2 =((uint64_t) task_id << 56) | ((uint64_t) 1 << 48) | ((uint64_t) loop_cycles << 32) | ((uint64_t) cfg_num << 16) | (uint64_t)cfg_base_addr;
//	ROCC_INSTRUCTION_SS(0, rs1, rs2, CGRA_CFG_CMD);
//	return 0;
//}

//static inline int execute(int task_id)
//{
//	uint64_t rs1 = 0; // (uint64_t) exe_cycles;
//	uint64_t rs2 = (uint64_t) task_id << 56;
//	ROCC_INSTRUCTION_SS(0, rs1, rs2, CGRA_EXE_CMD);
//	return 0;
//}

static inline int fence(int xd, int target_tram)
{
    if(xd){ // has return value
        unsigned long value;
		if (target_tram == 0)
	    	ROCC_INSTRUCTION_D(0, value, FENCE_CMD);
		if (target_tram == 1)
	    	ROCC_INSTRUCTION_D(1, value, FENCE_CMD);
    	return (value == 1);
    }else{
		if (target_tram == 0)
	        ROCC_INSTRUCTION(0, FENCE_CMD);
		if (target_tram == 1)
	        ROCC_INSTRUCTION(1, FENCE_CMD);
    	return 0;
    }
}

static inline int flush(unsigned target_tram)
{
	unsigned long value;
	if (target_tram == 0)
		ROCC_INSTRUCTION_D(0, value, FLUSH_CMD);
	if (target_tram == 1)
		ROCC_INSTRUCTION_D(1, value, FLUSH_CMD);
	return (value == 1);
}



