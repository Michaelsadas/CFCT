#include <stdio.h>
#include "include/rocc.h"
#include "include/ISA.h"
#include "include/encoding.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	unsigned short int cin[20][3] __attribute__((aligned(8))) = {
		{0x2000, 0x8000, 0x0030},
		{0x0000, 0x0000, 0x0031},
		{0x0000, 0x0000, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x2001, 0x8000, 0x0038},
		{0x0000, 0x0000, 0x0039},
		{0x0000, 0x0000, 0x003a},
		{0x0010, 0x0000, 0x003b},
		{0x3001, 0x8000, 0x0040},
		{0x0000, 0x0000, 0x0041},
		{0x0000, 0x0000, 0x0042},
		{0x0510, 0x0001, 0x0043},
		{0x0000, 0x0000, 0x0080},
		{0x0030, 0x0000, 0x0088},
		{0x0014, 0x0000, 0x00c8},
		{0x0803, 0x0000, 0x00c9},
		{0x000a, 0x0000, 0x00d0},
		{0x0803, 0x0000, 0x00d1},
		{0x0400, 0x0000, 0x0118},
		{0x4801, 0x0000, 0x0161},
	};

	load_cfg(cin, 0x40000, 120, 0, 0);
	load_data(din_addr[0], 0x18000, 36, 0, 0);
	load_data(din_addr[1], 0x10000, 32, 0, 0);
	config(0x0, 20, 0, 0);
	execute(0xe0, 0, 0);
	store(dout_addr[0], 0x1c000, 36, 0, 0);
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
        b0[i] = a[i] * 10 + a[i + 1] * 20; //+ a[i+ 2] * 3;
    }
    time0 = rdcycle() - time0;

    time1 = rdcycle();
    void* cgra_din_addr[2] = {a, (void*)a+8};
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
