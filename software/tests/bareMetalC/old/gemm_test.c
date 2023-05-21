#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif

//Define compute data type
#define TYPE unsigned int

//Specify row/column sizes
#define row_size 32
#define col_size 32
#define N (row_size*col_size)

TYPE m1[N] __attribute__((aligned(8)));
TYPE m2[N] __attribute__((aligned(8)));
TYPE prod0[N] __attribute__((aligned(8)));
TYPE prod[N] __attribute__((aligned(8)));

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

#include "include/rocc.h"
#include "include/ISA.h"
#include "include/encoding.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	unsigned short int cin[22][3] __attribute__((aligned(8))) = {
		{0x0000, 0x0004, 0x0030},
		{0x8402, 0x41f0, 0x0031},
		{0x0080, 0x083e, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x1000, 0x0000, 0x0038},
		{0x0402, 0x4000, 0x0039},
		{0x0080, 0x0800, 0x003a},
		{0x0710, 0x0101, 0x003b},
		{0x2000, 0x0000, 0x0040},
		{0x8402, 0x41ff, 0x0041},
		{0x0080, 0x0800, 0x0042},
		{0x0010, 0x0000, 0x0043},
		{0x0000, 0x0000, 0x0080},
		{0x0003, 0x0000, 0x0088},
		{0x8803, 0x0000, 0x00c9},
		{0x0000, 0x0020, 0x0118},
		{0x0000, 0x0620, 0x01a8},
		{0x0300, 0x0000, 0x0238},
		{0x080f, 0x0000, 0x0281},
		{0x0000, 0x8002, 0x0282},
		{0x1002, 0x8000, 0x0283},
		{0x0000, 0x0000, 0x0284},
	};

	load_cfg(cin, 0x40000, 132, 0, 0);
	load_data(din_addr[0], 0x18000, 4096, 0, 0);
	load_data(din_addr[1], 0x10000, 4096, 0, 0);
	config(0x0, 22, 0, 0);
	execute(0xe0, 0, 0);
	store(dout_addr[0], 0x1c000, 4096, 0, 0);
	int res = fence(1);
}

int main(){
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

    unsigned long long time0, time1;

    for (int i=0; i<N; i++){
      m1[i] = i;
      m2[i] = i+1;
    }

    time0 = rdcycle();
    gemm();
    time0 = rdcycle() - time0;

    time1 = rdcycle();
    void* cgra_din_addr[2] = {m1, m2};
    void* cgra_dout_addr[1] = {prod};
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    time1 = rdcycle() - time1;

//    printf("%x, %x, %x, %x\n", m1, m2, prod0, prod);
    printf("Execution time (CPU, CGRA): %ul, %ul\n", time0, time1);
    printf("Compare the result:\n");
    for (int i=0; i<N; i++){
        if(prod[i] != prod0[i]){
            printf("%x, %x\n", prod0[i], prod[i]);
        }
    }
    printf("Succeed!\n");

    return 0;
}