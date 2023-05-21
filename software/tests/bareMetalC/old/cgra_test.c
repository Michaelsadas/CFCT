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

#define sizeofBank 4096
#define in_port0 4  // INPUT
#define in_port1 5  // LOAD 0
#define in_port2 6  // LOAD 1
#define out_port 7
#define cfg_port 8
#define config_num 32

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
        {0x0001, 0x0000, 0x80}, //
        {0x1003, 0x0000, 0x81}, //
        {0x0001, 0x0000, 0x84}, //
        {0x0c03, 0x0000, 0x85},
        {0x7001, 0x0000, 0x89},
        {0x1000, 0x0000, 0x94},
        {0x0031, 0x0000, 0x98},
        {0x0000, 0x0200, 0x9c},
        {0x0000, 0x0000, 0xa0},
        {0x0001, 0x0000, 0xa4},
        {0x0803, 0x0000, 0xa5},
        {0x0000, 0x0000, 0xa8},
        {0x0c01, 0x0000, 0xa9},
        {0x0001, 0x0000, 0xac},
        {0x0c03, 0x0000, 0xad},
        {0x0001, 0x0000, 0xb0},
        {0x0c03, 0x0000, 0xb1},
        {0x0010, 0x0000, 0xbc},
        {0x0100, 0x00c8, 0xc0},
        {0x0b10, 0x0000, 0xc4},
        {0x0800, 0x8200, 0xcc},
        {0x0408, 0x0000, 0xcd},
        {0x0080, 0x0000, 0xce},
        {0x0400, 0x0400, 0xd0},
        {0x0000, 0x0000, 0xd1},
        {0x2880, 0x0010, 0xd2},
        {0x0000, 0x0400, 0xd4},
        {0x0000, 0x0000, 0xd5},
        {0x2880, 0x0010, 0xd6},
        {0x0400, 0x0400, 0xd8},
        {0x0000, 0x0000, 0xd9},
        {0x5080, 0x0418, 0xda}  //
    };
    struct config cout[config_num] __attribute__((aligned(8)));
//    unsigned short cout[config_num*3] __attribute__((aligned(8)));

    int in_addr0 = in_port0 * sizeofBank;
    int in_addr1 = in_port1 * sizeofBank;
    int in_addr2 = in_port2 * sizeofBank;
    int out_addr = out_port * sizeofBank;
    int config_addr = cfg_port * sizeofBank;
//    int spad_addr = 0;
    
    int NUM = 64;
    int len = 4*NUM;
    int config_len = config_num * sizeof(struct config);
    int result;
    unsigned int din0[NUM] __attribute__((aligned(8)));
    unsigned int din1[NUM] __attribute__((aligned(8)));
    unsigned int din2[NUM] __attribute__((aligned(8)));
    unsigned int dout[NUM] __attribute__((aligned(8)));
    
    for(int i = 0; i < NUM; i++){
        din0[i] = 4*i;
        din1[i] = i+1;
        din2[i] = 2*i;
    }
    uint64_t iob_ens = (1 << in_port0) | (1 << in_port1) | (1 << in_port2) | (1 << out_port);

    for(int i = 0; i < 1; i++){
//        printf("Load the config information.\n");
        load_cfg(cin, config_addr, config_len, i, 2);
//        store(cout, config_addr, config_len, i);
//        printf("Load the input data.\n");
        load_data(din0, in_addr0, len, i, 2);
        load_data(din1, in_addr1, len, i, 0);
        load_data(din2, in_addr2, len, i, 0);
//        fence(0);
//        printf("Config the CGRA.\n");
//        fence(0);
        config(0, config_num, i, 0);
//        printf("Load the input data.\n");
//        load(din, in_addr, len, i);
//        printf("Perform computation.\n");
        execute(iob_ens, i, 1);
//        fence(0);
//        printf("Store the output data.\n");
        store(dout, out_addr, len, i, 0);
//        int res = fence(1);
//        fence(0);
//        printf("Here comes the result: (output)\n");
//        for(int i = 0; i < NUM; i++){
//            printf("%d\n", dout[i]);
//        }
    }
    printf("Here comes the result: (output)\n");
    for(int i = 0; i < NUM; i++){
        printf("%d\n", dout[i]); // 3*i+1
    }
//    //load the data
//    printf("Load the data.\n");
//    load(din, in_addr, len);
//    fence(0);
//    //*************************************check the load***********************************************//
////    store(dout, in_addr, len);
////    fence(0);
////    result = fence(1);
////    printf("Fence succeed? : %d\n", result);
////    printf("Dout : \n");
////    for(int i = 0; i < NUM; i++){
////        printf("0x%x  ", dout[i]);
////    }
////    printf("\n");
//    //****************************************finished**************************************************//
//    printf("Loading data completed.\n---------------------------\n");
//
////    printf("cin address 0x%x\n", (void*)cin);
////    printf("cout address 0x%x\n", (void*)cout);
//    //load the config information
//    printf("Load the config information.\n");
//    load((void*)cin, config_addr, config_len);
//    fence(0);
//    //************************************check the load*************************************************///
//    store((void*)cout, config_addr, config_len);
//    fence(0);
//    //    printf("The size of the config structure is %d.\n", sizeof(struct config));
//    printf("Config data: \n");
//    for (int i = 0; i < config_num; i++){
//    	printf("0x%x, 0x%x, 0x%x\n", cout[i].cfg_data1, cout[i].cfg_data2, cout[i].cfg_base_addr);
////    	printf("(0x%x)\n", ((unsigned short *)cout)[i]);
//    }
//    //***************************************finished***************************************************///
//    printf("Loading config information completed.\n---------------------------\n");
//
//    //config the CGRA
//    printf("Config the CGRA.\n");
//    unsigned int iob_ens = (1 << in_port) | (1 << out_port1) | (1 << out_port2);
//    config(iob_ens, NUM, config_num, 0);
//    fence(0);
//    printf("Configuration completed.\n---------------------------\n");
//
//    //execute
//    printf("Execute the data.\n");
//    execute();
//    result = fence(1);
//    printf("Execution completed.\n---------------------------\n");
//
//    //store the data
//    store(dout1, out_addr1, len);
//    fence(0);
//    store(dout2, out_addr2, len);
//
//    printf("Here comes the result: (output1, output2)\n");
//    for(int i = 0; i < NUM; i++){
//        printf("%d , %d\n", dout1[i], dout2[i]);
//    }
//
//    fence(0);
//    printf("Fence\n");

    return 0;
}
