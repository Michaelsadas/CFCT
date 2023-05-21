#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif

//Define compute data type
#define TYPE float

//Specify row/column sizes
#define row_size 32
#define col_size 32
#define N (row_size*col_size)

TYPE m1[N] __attribute__((aligned(8)));
TYPE m2[N] __attribute__((aligned(8)));
TYPE prod0[N] __attribute__((aligned(8)));
TYPE prod[N] __attribute__((aligned(8)));

__attribute__((noinline))
void gemm(){
    int i, j, k;
    int k_col, i_col;
    TYPE mult;

    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<col_size;j++) {
            i_col = i * col_size;
            TYPE sum = 0;
            inner:for(k=0;k<row_size;k++) {
                k_col = k * col_size;
                mult = m1[i_col + k] * m2[k_col + j];
                sum += mult;
            }
            prod0[i_col + j]  = sum;
        }
    }
}

#include "include/ISA.h"
#include "include/encoding.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[59][3] __attribute__((aligned(8))) = {
		{0x1820, 0x2010, 0x0004},
		{0x2040, 0x8107, 0x0005},
		{0x0e30, 0x0102, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x9001, 0x2000, 0x0008},
		{0xf900, 0x0107, 0x0009},
		{0x0002, 0x0102, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x0800, 0x2010, 0x000c},
		{0x2040, 0x8107, 0x000d},
		{0x0e30, 0x0102, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x8000, 0x2000, 0x0010},
		{0xf900, 0x0107, 0x0011},
		{0x0002, 0x0102, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x0400, 0x0000, 0x002c},
		{0x0100, 0x0000, 0x0030},
		{0x0000, 0x0000, 0x0034},
		{0x2403, 0x0000, 0x0051},
		{0x4403, 0x0000, 0x0055},
		{0x0400, 0x0000, 0x0078},
		{0x2401, 0x0000, 0x009d},
		{0x0000, 0x0000, 0x00c4},
		{0x3011, 0x0000, 0x00e9},
		{0xc000, 0x0000, 0x0108},
		{0x0000, 0x0000, 0x010c},
		{0x0002, 0x0000, 0x0110},
		{0x0000, 0x0001, 0x0114},
		{0x9003, 0x0000, 0x0125},
		{0x4c21, 0x0000, 0x0129},
		{0x0c0d, 0x0000, 0x012d},
		{0x0000, 0x7001, 0x012e},
		{0x0020, 0x0100, 0x012f},
		{0x8c03, 0x0000, 0x0139},
		{0x0004, 0x0000, 0x014c},
		{0x0100, 0x0000, 0x0150},
		{0x0010, 0x0000, 0x015c},
		{0x0000, 0x0000, 0x0160},
		{0x8003, 0x2000, 0x016c},
		{0xf900, 0x0107, 0x016d},
		{0x0002, 0x0102, 0x016e},
		{0x0000, 0x0000, 0x016f},
		{0x0860, 0x2010, 0x0170},
		{0x2040, 0x8107, 0x0171},
		{0x0e30, 0x0102, 0x0172},
		{0x0000, 0x0000, 0x0173},
		{0x1000, 0x2000, 0x0174},
		{0x0040, 0x8100, 0x0175},
		{0x0000, 0x9102, 0x0176},
		{0x0000, 0x0000, 0x0177},
		{0x0840, 0x2010, 0x017c},
		{0x2040, 0x8107, 0x017d},
		{0x0e30, 0x0102, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x8002, 0x2000, 0x0184},
		{0xf900, 0x0107, 0x0185},
		{0x0002, 0x0102, 0x0186},
		{0x0000, 0x0000, 0x0187},
	};

	load_cfg((void*)cin, 0x20000, 354, 0, 0);
	load_data(din_addr[0], 0x0, 4096, 1, 0, 0);
	load_data(din_addr[1], 0x4000, 4096, 1, 0, 0);
	load_data(din_addr[2], 0x18000, 4096, 1, 0, 0);
	load_data(din_addr[3], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[4], 0x2000, 4096, 1, 0, 0);
	load_data(din_addr[5], 0x6000, 4096, 1, 0, 0);
	load_data(din_addr[6], 0x1a000, 4096, 1, 0, 0);
	load_data(din_addr[7], 0x12000, 4096, 0, 0, 0);
	config(0x0, 59, 0, 0);
	execute(0x570f, 0, 0);
	store(dout_addr[0], 0x14000, 4096, 0, 0);
}


int main(){
//#ifndef BAREMETAL
//    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
//      perror("mlockall failed");
//      exit(1);
//    }
//#endif
    long long unsigned start;
    long long unsigned end;

    for (int i=0; i<N; i++){
      m1[i] = i;
      m2[i] = i+1;
    }

    start = rdcycle();
    gemm();
    end = rdcycle();
    printf("It takes %d cycles for CPU to finish the task.\n", end - start);

//    void* cgra_din_addr[2] = {m1, m2};
	void* cgra_din_addr[8] = {m1, m1, m1, m1, m2, m2, m2, m2};
	void* cgra_dout_addr[1] = {prod};
    start = rdcycle();
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    volatile int result = fence(1);
    end = rdcycle();
    printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

//    printf("%x, %x, %x, %x\n", m1, m2, prod0, prod);
//    printf("Execution time (CPU, CGRA): %ul, %ul\n", time0, time1);
    printf("Compare the result:\n");
    for (int i=0; i<N; i++){
        if((int)prod[i] != (int)prod0[i]){
            printf("%x, %x\n", (int)prod0[i], (int)prod[i]);
        }
    }
    printf("Succeed!(%d)\n", result);

    return 0;
}