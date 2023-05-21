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

#define sizeofBank 1024
#define in_port 4
#define out_port1 7
#define out_port2 1
#define cfg_port 8
#define config_num 35

struct config
{
    unsigned short int cfg_data1;
    unsigned short int cfg_data2;
    unsigned short int cfg_base_addr;
};

int main(){
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

    struct config cin[config_num] __attribute__((aligned(8))) = {
        {0x0900, 0x0018, 0x08}, //
        {0x0000, 0x0000, 0x20},
        {0x0001, 0x0000, 0x34},
        {0x8c01, 0x0000, 0x35},
        {0x0000, 0x0000, 0x4c},
        {0x0001, 0x0000, 0x58},
        {0x8801, 0x0000, 0x59},
        {0x4801, 0x0000, 0x5d},
        {0x0001, 0x0000, 0x60},
        {0x8801, 0x0000, 0x61},
        {0x0010, 0x0000, 0x6c},
        {0x0010, 0x0000, 0x70},
        {0x0010, 0x0000, 0x74},
        {0x0001, 0x0000, 0x7c},
        {0x8c01, 0x0000, 0x7d},
        {0x0001, 0x0000, 0x80},
        {0x8801, 0x0000, 0x81},
        {0x0001, 0x0000, 0x84},
        {0x8801, 0x0000, 0x85},
        {0x4101, 0x0000, 0x89},
        {0x1010, 0x0000, 0x94},
        {0x0010, 0x0000, 0x98},
        {0x0140, 0x0000, 0x9c},
        {0x0001, 0x0000, 0xa4},
        {0x8401, 0x0000, 0xa5},
        {0x0001, 0x0000, 0xa8},
        {0x8801, 0x0000, 0xa9},
        {0x0001, 0x0000, 0xac},
        {0x8801, 0x0000, 0xad},
        {0x4181, 0x0000, 0xb1},
        {0x0010, 0x0000, 0xbc},
        {0x0000, 0x0000, 0xc0},
        {0x0100, 0x0000, 0xc4},
        {0x0000, 0x0000, 0xcc},
        {0x0d00, 0x0008, 0xd8}
    };
    struct config cout[config_num] __attribute__((aligned(8)));
//    unsigned short cout[config_num*3] __attribute__((aligned(8)));

    int in_addr = in_port * sizeofBank;
    int out_addr1 = out_port1 * sizeofBank;
    int out_addr2 = out_port2 * sizeofBank;
    int config_addr = cfg_port * sizeofBank;
//    int spad_addr = 0;
    
    int NUM = 31;
    int len = 4*NUM;
    int config_len = config_num * sizeof(struct config);
    int result;
    unsigned int din[NUM] __attribute__((aligned(8)));
//    unsigned int dout[NUM] __attribute__((aligned(8)));
    unsigned int dout1[NUM] __attribute__((aligned(8)));
    unsigned int dout2[NUM] __attribute__((aligned(8)));
    
    for(int i = 0; i < NUM; i++){
        din[i] = i+1;
    }

    //load the data
    printf("Load the data.\n");
    load(din, in_addr, len);
    fence(0);
    //*************************************check the load***********************************************//
//    store(dout, in_addr, len);
//    fence(0);
//    result = fence(1);
//    printf("Fence succeed? : %d\n", result);
//    printf("Dout : \n");
//    for(int i = 0; i < NUM; i++){
//        printf("0x%x  ", dout[i]);
//    }
//    printf("\n");
    //****************************************finished**************************************************//
    printf("Loading data completed.\n---------------------------\n");

//    printf("cin address 0x%x\n", (void*)cin);
//    printf("cout address 0x%x\n", (void*)cout);
    //load the config information
    printf("Load the config information.\n");
    load((void*)cin, config_addr, config_len);
    fence(0);
    //************************************check the load*************************************************///
    store((void*)cout, config_addr, config_len);
    fence(0);
    //    printf("The size of the config structure is %d.\n", sizeof(struct config));
    printf("Config data: \n");
    for (int i = 0; i < config_num; i++){
    	printf("0x%x, 0x%x, 0x%x\n", cout[i].cfg_data1, cout[i].cfg_data2, cout[i].cfg_base_addr);
//    	printf("(0x%x)\n", ((unsigned short *)cout)[i]);
    }
    //***************************************finished***************************************************///
    printf("Loading config information completed.\n---------------------------\n");

    //config the CGRA
    printf("Config the CGRA.\n");
    unsigned int iob_ens = (1 << in_port) | (1 << out_port1) | (1 << out_port2);
    config(iob_ens, NUM, config_num, 0);
    fence(0);
    printf("Configuration completed.\n---------------------------\n");

    //execute
    printf("Execute the data.\n");
    execute();
    result = fence(1);
    printf("Execution completed.\n---------------------------\n");
 
    //store the data
    store(dout1, out_addr1, len);
    fence(0);
    store(dout2, out_addr2, len);

    printf("Here comes the result: (output1, output2)\n");
    for(int i = 0; i < NUM; i++){
        printf("%d , %d\n", dout1[i], dout2[i]);
    }
    
    fence(0);
    printf("Fence\n");

    return 0;
}
