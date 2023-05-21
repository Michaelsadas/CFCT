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
#include "include/ISA.h"

#define FENCE_CMD 0
#define LAOD_CMD 1
#define STORE_CMD 2
#define FLUSH_CMD 5



int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif
    int NUM = 31;
    unsigned long din[NUM] __attribute__((aligned(8)));
    unsigned long dout[NUM] __attribute__((aligned(8)));
    for(int i = 0; i < NUM; i++){
        din[i] = i;
    }
    int spad_addr = 0;
    int len = 8*NUM;
    int result = 0;
    printf("din address 0x%x\n", (void*)din);
    printf("dout address 0x%x\n", (void*)dout);
    printf("Load data of 0x%x bytes\n", len);
    load_data(din, spad_addr, len, 0, 0, 0);
    printf("Fence\n");
    fence(0);
    printf("Store data of 0x%x bytes\n", len);
    store(dout, spad_addr, len, 0, 0);
    result = fence(1);
    printf("Fence succeed? : %d\n", result);
    printf("Dout : \n");
    for(int i = 0; i < NUM; i++){
        printf("0x%x  ", dout[i]);
    }
    printf("\n");
    for(int i = 0; i < NUM; i++){
        din[i] = i+1;
    }
    len = 8*(NUM-10);
    printf("Load data of 0x%x bytes\n", len);
    load_data(din, spad_addr, len, 0, 0, 0);
    printf("Fence\n");
    fence(0);
    printf("Store data of 0x%x bytes\n", len);
    store(dout, spad_addr, len, 0 ,0);
    result = fence(1);
    printf("Fence succeed? : %d\n", result);
    printf("Dout : \n");
    for(int i = 0; i < NUM; i++){
        printf("0x%x  ", dout[i]);
    }
    printf("\n");
}

