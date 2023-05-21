#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/rocc.h"
#include "include/ISA.h"
#include "include/encoding.h"

void cgra_execute_conv3(void** din_addr, void** dout_addr)
{
	unsigned short int cin[39][3] __attribute__((aligned(8))) = {
		{0x0180, 0x007c, 0x0004},
		{0x0000, 0x0000, 0x0005},
		{0x0910, 0x0101, 0x0006},
		{0x0100, 0x007c, 0x0008},
		{0x0000, 0x0000, 0x0009},
		{0x0010, 0x0000, 0x000a},
		{0x4001, 0x0000, 0x001c},
		{0x1000, 0x0000, 0x0020},
		{0x5181, 0x0000, 0x002d},
		{0x0001, 0x0000, 0x0030},
		{0x0801, 0x0000, 0x0031},
		{0x0001, 0x0000, 0x0034},
		{0x0401, 0x0000, 0x0035},
		{0x0001, 0x0000, 0x0038},
		{0x0c01, 0x0000, 0x0039},
		{0x0400, 0x0000, 0x0044},
		{0x0000, 0x0200, 0x0048},
		{0x0000, 0x0000, 0x004c},
		{0x3101, 0x0000, 0x0059},
		{0x0001, 0x0000, 0x005c},
		{0x0801, 0x0000, 0x005d},
		{0x0001, 0x0000, 0x0060},
		{0x0401, 0x0000, 0x0061},
		{0x2400, 0x0000, 0x0070},
		{0x1100, 0x0000, 0x0074},
		{0x0001, 0x0000, 0x0080},
		{0x0801, 0x0000, 0x0081},
		{0x2801, 0x0000, 0x0085},
		{0x0001, 0x0000, 0x0088},
		{0x0401, 0x0000, 0x0089},
		{0x0000, 0x0000, 0x0094},
		{0x0001, 0x0000, 0x00a4},
		{0x0801, 0x0000, 0x00a5},
		{0x0001, 0x0000, 0x00a8},
		{0x0c01, 0x0000, 0x00a9},
		{0x0100, 0x0000, 0x00bc},
		{0x0100, 0x007c, 0x00d0},
		{0x0000, 0x0000, 0x00d1},
		{0x0810, 0x0001, 0x00d2},
	};

	load_cfg(cin, 0x1000, 234, 0, 0);
	load_data(din_addr[0], 0x0, 124, 0, 0);
	config(0x0, 39, 0, 0);
	execute(0x23, 0, 0);
	store(dout_addr[0], 0x200, 124, 0, 0);
	store(dout_addr[1], 0x800, 124, 0, 0);
}


void cgra_execute_matrix(void** din_addr, void** dout_addr)
{
	unsigned short int cin[30][3] __attribute__((aligned(8))) = {
		{0x0100, 0x00a0, 0x0008},
		{0x0000, 0x0000, 0x0009},
		{0x0010, 0x0000, 0x000a},
		{0x1000, 0x0000, 0x0020},
		{0x0000, 0x0000, 0x0030},
		{0x0801, 0x0000, 0x0031},
		{0x0000, 0x0000, 0x0034},
		{0x0401, 0x0000, 0x0035},
		{0x0000, 0x0000, 0x0038},
		{0x0c01, 0x0000, 0x0039},
		{0x0400, 0x0000, 0x0044},
		{0x4100, 0x0000, 0x004c},
		{0x0000, 0x0000, 0x0054},
		{0x0803, 0x0000, 0x0055},
		{0x2c81, 0x0000, 0x0059},
		{0x4811, 0x0000, 0x005d},
		{0x0000, 0x0000, 0x0060},
		{0x0403, 0x0000, 0x0061},
		{0x0000, 0x0000, 0x006c},
		{0x0100, 0x0000, 0x0070},
		{0x2403, 0x0000, 0x0085},
		{0x0100, 0x0000, 0x0098},
		{0x6601, 0x0000, 0x00ad},
		{0x0140, 0x0000, 0x00c0},
		{0x01b2, 0x00a0, 0x00d0},
		{0x0000, 0x0000, 0x00d1},
		{0x0110, 0x0000, 0x00d2},
		{0x0132, 0x00a0, 0x00d4},
		{0x0000, 0x0000, 0x00d5},
		{0x0910, 0x0001, 0x00d6},
	};

	load_cfg(cin, 0x1000, 180, 2, 0);
	load_data(din_addr[0], 0x0, 160, 2, 0);
	load_data(din_addr[1], 0xac8, 160, 2, 0);
	config(0x0, 30, 2, 0);
	execute(0x62, 2, 0);
	store(dout_addr[0], 0xcc8, 160, 2, 0);
}

void cgra_execute_simple(void** din_addr, void** dout_addr)
{
	unsigned short int cin[17][3] __attribute__((aligned(8))) = {
		{0x0000, 0x0000, 0x00a4},
		{0x1001, 0x0000, 0x00a5},
		{0x0000, 0x0000, 0x00a8},
		{0x0c03, 0x0000, 0x00a9},
		{0x8d01, 0x0000, 0x00ad},
		{0x0000, 0x0000, 0x00bc},
		{0x0100, 0x0000, 0x00c0},
		{0x0000, 0x0000, 0x00c4},
		{0x0180, 0x00c8, 0x00d0},
		{0x0000, 0x0000, 0x00d1},
		{0x0010, 0x0000, 0x00d2},
		{0x0100, 0x00c8, 0x00d4},
		{0x0000, 0x0000, 0x00d5},
		{0x0610, 0x0001, 0x00d6},
		{0x0180, 0x00c8, 0x00d8},
		{0x0000, 0x0000, 0x00d9},
		{0x0010, 0x0000, 0x00da},
	};

	load_cfg(cin, 0x10f0, 102, 1, 0);
	load_data(din_addr[0], 0xa00, 200, 1, 0);
	load_data(din_addr[1], 0xe00, 200, 1, 0);
	config(0x1e, 17, 1, 0);
	execute(0xe0, 1, 0);
	store(dout_addr[0], 0xc00, 200, 1, 0);
}


int main(){
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

    int NUM1 = 31;
    int NUM2 = 50;
    int NUM3 = 40;
    int NUM = NUM2;
    unsigned int din1[NUM] __attribute__((aligned(8)));
    unsigned int din2[NUM] __attribute__((aligned(8)));
    unsigned int dout1_0[NUM1] __attribute__((aligned(8)));
    unsigned int dout1_1[NUM1] __attribute__((aligned(8)));
    unsigned int dout2_0[NUM2] __attribute__((aligned(8)));
    unsigned int dout3_0[NUM3] __attribute__((aligned(8)));
    long long unsigned int start1, end1, start2, end2, start3, end3;

    for(int i = 0; i < NUM; i++){
        din1[i] = i + 1;
        din2[i] = 2 * i;
    }

    // conv3
    start1 = rdcycle();
    void* din_addr_conv3[1] = {din1};
    void* dout_addr_conv3[2] = {dout1_0, dout1_1};
    cgra_execute_conv3(din_addr_conv3, dout_addr_conv3);
    end1 = rdcycle();

    // simple
    start2 = rdcycle();
    void* din_addr_simple[2] = {din1, din2};
    void* dout_addr_simple[1] = {dout2_0};
    cgra_execute_simple(din_addr_simple, dout_addr_simple);
    end2 = rdcycle();

    start3 = rdcycle();
    void* din_addr_matrix[2] = {din1, din2};
    void* dout_addr_matrix[1] = {dout3_0};
    cgra_execute_matrix(din_addr_matrix, dout_addr_matrix);
    end3 = rdcycle();

    printf("Here comes the result of conv3(%d cycles): (output1, output2)\n----------------------------\n", end1 - start1);
    for(int i = 0; i < NUM1; i++){
        printf("%d , %d\n", dout1_0[i], dout1_1[i]); // 4x+10, x+4; x = 1, 2, 3...
    }

    printf("Here comes the result of simple(%d cycles): (output3)\n----------------------------\n", end2 - start2);
    for(int i = 0; i < NUM2; i++){
        printf("%d\n", dout2_0[i]); // x2; 0, 2, 4...
    }

    printf("Here comes the result of matrixmul(%d cycles): (output4)\n----------------------------\n", end3 - start3);
    for(int i = 0; i < NUM3; i++){
        printf("%d\n", dout3_0[i]); // x1*x1+x2 = (i+2)*(i+2)-3
    }

    return 0;
}
