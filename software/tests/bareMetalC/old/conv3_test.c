#include <stdio.h>
#include "include/rocc.h"
#include "include/ISA.h"
#include "include/encoding.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	unsigned short int cin[31][3] __attribute__((aligned(8))) = {
		{0x3000, 0x8000, 0x0008},
		{0x0000, 0x0000, 0x0009},
		{0x0000, 0x0000, 0x000a},
		{0x0010, 0x0000, 0x000b},
		{0x2001, 0x8000, 0x0010},
		{0x0000, 0x0000, 0x0011},
		{0x0000, 0x0000, 0x0012},
		{0x0010, 0x0000, 0x0013},
		{0x2001, 0x8000, 0x0018},
		{0x0000, 0x0000, 0x0019},
		{0x0000, 0x0000, 0x001a},
		{0x0010, 0x0000, 0x001b},
		{0x3001, 0x8000, 0x0020},
		{0x0000, 0x0000, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0610, 0x0001, 0x0023},
		{0x0000, 0x0000, 0x0058},
		{0x0100, 0x0000, 0x0060},
		{0x0030, 0x0000, 0x0068},
		{0x000a, 0x0000, 0x0098},
		{0x1003, 0x0000, 0x0099},
		{0x0014, 0x0000, 0x00a0},
		{0x0803, 0x0000, 0x00a1},
		{0x0003, 0x0000, 0x00a8},
		{0x0803, 0x0000, 0x00a9},
		{0x0000, 0x0000, 0x00e0},
		{0x0000, 0x0040, 0x00e8},
		{0x0600, 0x0000, 0x00f0},
		{0x0000, 0x0008, 0x00f8},
		{0x5001, 0x0000, 0x0129},
		{0x4901, 0x0000, 0x0139},
	};

	load_cfg(cin, 0x40000, 186, 0, 0);
	load_data(din_addr[0], 0x0, 36, 0, 0);
	load_data(din_addr[1], 0x4000, 32, 0, 0);
	load_data(din_addr[2], 0x8000, 36, 0, 0);
	config(0x0, 31, 0, 0);
	execute(0xf, 0, 0);
	store(dout_addr[0], 0xc000, 36, 0, 0);
}



int a[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
int b[20];// = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
int b0[20];


int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

    unsigned long long time0, time1;

    int i, j;
    int n = 10;//*N;

    int sum = 0;
    time0 = rdcycle();
    for (i = 1; i < n-1; i++) {
        b0[i] = a[i] * 10 + a[i + 1] * 20 + a[i+ 2] * 3;
    }
    time0 = rdcycle() - time0;

    time1 = rdcycle();
    void* cgra_din_addr[3] = {a, (void*)a+8, (void*)a+8};
    void* cgra_dout_addr[1] = {b};
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    time1 = rdcycle() - time1;

    printf("Execution time (CPU, CGRA): %ul, %ul\n", time0, time1);
    printf("Compare the result:\n");
    for (i = 1; i < n-1; i++) {
        if(b[i] != b0[i]){
            printf("%x, %x\n", b0[i], b[i]);
        }
    }
    printf("Succeed!\n");

    return 0;
}
