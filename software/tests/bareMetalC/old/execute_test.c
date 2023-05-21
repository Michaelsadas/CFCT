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

#define sizeofBank 1024
#define in_port1 5
#define in_port2 5
#define in_port3 4
#define in_port4 5
#define in_port5 6
#define out_port1 4
#define out_port2 0
#define out_port3 6
#define out_port4 7
#define cfg_port 8
#define config_num1 36
#define config_num2 11
#define config_num3 25

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

    struct config cin1[config_num1] __attribute__((aligned(8))) = {
        {0x0c00, 0x0018, 0x04}, //
        {0x0000, 0x0000, 0x1c},
        {0x0100, 0x0000, 0x20},
        {0x0001, 0x0000, 0x30},
        {0x8401, 0x0000, 0x31},
        {0x0001, 0x0000, 0x34},
        {0x8c01, 0x0000, 0x35},
        {0x0000, 0x0000, 0x4c},
        {0x0001, 0x0000, 0x58},
        {0x8c01, 0x0000, 0x59},
        {0x4c01, 0x0000, 0x5d},
        {0x0001, 0x0000, 0x60},
        {0x8801, 0x0000, 0x61},
        {0x0000, 0x0000, 0x70},
        {0x0030, 0x0000, 0x74},
        {0x6501, 0x0000, 0x81},
        {0x0001, 0x0000, 0x84},
        {0x8c01, 0x0000, 0x85},
        {0x0001, 0x0000, 0x88},
        {0x8c01, 0x0000, 0x89},
        {0x4000, 0x0000, 0x94},
        {0x1004, 0x0000, 0x98},
        {0x0001, 0x0008, 0x9c},
        {0x0000, 0x0000, 0xa0},
        {0x2581, 0x0000, 0xa5},
        {0x0001, 0x0000, 0xa8},
        {0x8401, 0x0000, 0xa9},
        {0x0001, 0x0000, 0xac},
        {0x8801, 0x0000, 0xad},
        {0x0001, 0x0000, 0xb0},
        {0x8801, 0x0000, 0xb1},
        {0x0000, 0x0000, 0xb8},
        {0x0010, 0x0000, 0xc0},
        {0x0000, 0x0000, 0xc4},
        {0x1000, 0x0008, 0xcc}, //
        {0x0100, 0x0000, 0xd0}  //
    };

    struct config cin2[config_num2] __attribute__((aligned(8))) = {
        {0x0000, 0x0000, 0x7c},  //
        {0x8c03, 0x0000, 0x7d},
        {0x0001, 0x0000, 0x94},
        {0x0000, 0x0000, 0xa4},
        {0x8c01, 0x0000, 0xa5},
        {0x4101, 0x0000, 0xa9},
        {0x0040, 0x0000, 0xbc},
        {0x0000, 0x0000, 0xc0},
        {0x0000, 0x0000, 0xcc},
        {0x0100, 0x0000, 0xd0},
        {0x0a00, 0x0008, 0xd4}
    };

    struct config cin3[config_num3] __attribute__((aligned(8))) = {
        {0x0000, 0x0000, 0x54}, 
        {0x8c03, 0x0000, 0x55},
        {0x4881, 0x0000, 0x59},
        {0x0041, 0x0000, 0x6c},
        {0x0000, 0x0000, 0x70},
        {0x0000, 0x0000, 0x7c},
        {0x8c01, 0x0000, 0x7d},
        {0x0000, 0x0000, 0x80},
        {0x8801, 0x0000, 0x81},
        {0x0c03, 0x0000, 0x85},
        {0x0111, 0x0000, 0x94},
        {0x0000, 0x0000, 0x98},
        {0x0001, 0x0000, 0x9c},
        {0x0000, 0x0000, 0xa4},
        {0x8c01, 0x0000, 0xa5},
        {0x0000, 0x0000, 0xa8},
        {0x8003, 0x0000, 0xa9},
        {0x4011, 0x0000, 0xad},
        {0x4201, 0x0000, 0xb1},
        {0x0000, 0x0000, 0xbc},
        {0x0000, 0x0000, 0xc0},
        {0x0140, 0x0000, 0xc4},
        {0x0100, 0x0000, 0xd0},
        {0x0200, 0x0000, 0xd4}, 
        {0x1100, 0x0008, 0xd8}
    };
//    unsigned short cout[config_num*3] __attribute__((aligned(8)));

    int in_addr1 = in_port1 * sizeofBank;
    int in_addr2 = in_port2 * sizeofBank;
    int in_addr3 = in_port3 * sizeofBank;
    int in_addr4 = in_port4 * sizeofBank;
    int in_addr5 = in_port5 * sizeofBank;
    int out_addr1 = out_port1 * sizeofBank;
    int out_addr2 = out_port2 * sizeofBank;
    int out_addr3 = out_port3 * sizeofBank;
    int out_addr4 = out_port4 * sizeofBank;
    int config_addr = cfg_port * sizeofBank;
//    int spad_addr = 0;
    
    int NUM = 31;
    int len = 4*NUM;
    int config_len1 = config_num1 * sizeof(struct config);
    int config_len2 = config_num2 * sizeof(struct config);
    int config_len3 = config_num3 * sizeof(struct config);
    int result;
    unsigned int din1[NUM] __attribute__((aligned(8)));
    unsigned int din2[NUM] __attribute__((aligned(8)));
    unsigned int dout1[NUM] __attribute__((aligned(8)));
    unsigned int dout2[NUM] __attribute__((aligned(8)));
    unsigned int dout3[NUM] __attribute__((aligned(8)));
    unsigned int dout4[NUM] __attribute__((aligned(8)));
    long long unsigned int start1, end1, start2, end2, start3, end3;
    unsigned int iob_ens;

    for(int i = 0; i < NUM; i++){
        din1[i] = i + 1;
        din2[i] = 2 * i;
    }

//    start1 = rdcycle();
//    load(din1, in_addr1, len, 1);
//    load(cin1, config_addr, config_len1, 1);
//    iob_ens = (1 << in_port1) | (1 << out_port1) | (1 << out_port2);
//    config(iob_ens, NUM, config_num1, 0, 1);
//    execute(1);
//    store(dout1, out_addr1, len, 1);
//    store(dout2, out_addr2, len, 1);
//    fence(1);
//    end1 = rdcycle();
//    printf("Here comes the result of conv3(%d cycles): (output1, output2)\n----------------------------\n", end1 - start1);
//    for(int i = 0; i < NUM; i++){
//        printf("%d , %d\n", dout1[i], dout2[i]);
//    }
//
//    start3 = rdcycle();
//    load(din1, in_addr4, len, 3);
//    load(din2, in_addr5, len, 3);
//    load(cin3, config_addr, config_len3, 3);
//    iob_ens = (1 << in_port4) | (1 << in_port5) | (1 << out_port4);
//    config(iob_ens, NUM, config_num3, 0, 3);
//    execute(3);
//    store(dout4, out_addr4, len, 3);
//    fence(1);
//    end3 = rdcycle();
//    printf("Here comes the result of matrixmul(%d cycles): (output4)\n----------------------------\n", end3 - start3);
//    for(int i = 0; i < NUM; i++){
//        printf("%d\n", dout4[i]);
//    }

    start2 = rdcycle();
    load(din1, in_addr2, len, 2);
    load(din2, in_addr3, len, 2);
    load(cin2, config_addr, config_len2, 2);
    iob_ens = (1 << in_port2) | (1 << in_port3) | (1 << out_port3);
    config(iob_ens, NUM, config_num2, 0, 2);
    execute(2);
    fence(0);
//    for(int i = 0; i < 1000; i++){ fence(0); }
    store(dout3, out_addr3, len, 2);
    fence(1);
    end2 = rdcycle();
    printf("Here comes the result of simple(%d cycles): (output3)\n----------------------------\n", end2 - start2);
    for(int i = 0; i < NUM; i++){
        printf("%d\n", dout3[i]);
    }

    start1 = rdcycle();
    load(din1, in_addr1, len, 1);
    load(cin1, config_addr, config_len1, 1);    
    iob_ens = (1 << in_port1) | (1 << out_port1) | (1 << out_port2);
    config(iob_ens, NUM, config_num1, 0, 1);
    execute(1);
    store(dout1, out_addr1, len, 1);
    store(dout2, out_addr2, len, 1);
    fence(1);
    end1 = rdcycle();

    start2 = rdcycle();
    load(din1, in_addr2, len, 2);
    load(din2, in_addr3, len, 2);
    load(cin2, config_addr, config_len2, 2);
    iob_ens = (1 << in_port2) | (1 << in_port3) | (1 << out_port3);
    config(iob_ens, NUM, config_num2, 0, 2);
    execute(2);
//    for(int i = 0; i < 1000; i++){ fence(0); }
    store(dout3, out_addr3, len, 2);
    fence(1);
    end2 = rdcycle();

    start3 = rdcycle();
    load(din1, in_addr4, len, 3);
    load(din2, in_addr5, len, 3);
    load(cin3, config_addr, config_len3, 3);
    iob_ens = (1 << in_port4) | (1 << in_port5) | (1 << out_port4);
    config(iob_ens, NUM, config_num3, 0, 3);
    execute(3);
    store(dout4, out_addr4, len, 3);
    fence(1);
    end3 = rdcycle();

    printf("Here comes the result of conv3(%d cycles): (output1, output2)\n----------------------------\n", end1 - start1);
    for(int i = 0; i < NUM; i++){
        printf("%d , %d\n", dout1[i], dout2[i]);
    }
    
    printf("Here comes the result of simple(%d cycles): (output3)\n----------------------------\n", end2 - start2);
    for(int i = 0; i < NUM; i++){
        printf("%d\n", dout3[i]);
    }

    printf("Here comes the result of matrixmul(%d cycles): (output4)\n----------------------------\n", end3 - start3);
    for(int i = 0; i < NUM; i++){
        printf("%d\n", dout4[i]);
    }

//    start1 = rdcycle();
//    for(int i = 0; i < NUM; i++){
//        dout1[i] = din1[i] * 4 + 10;
//        dout2[i] = din1[i] + 4;
//    }
//    end1 = rdcycle();
//
//    start2 = rdcycle();
//    for(int i = 0; i < NUM; i++){
//        dout3[i] = din2[i] + din1[i];
//    }
//    end2 = rdcycle();
//
//    start3 = rdcycle();
//    for(int i = 0; i < NUM; i++){
//        dout4[i] = din1[i] * din1[i] + din2[i];
//    }
//    end3 = rdcycle();
//
//    printf("It takes %d cycles for Rocket CPU to finish the task of conv3.\nIt takes %d cycles for Rocket CPU to finish the task of simple.\nIt takes %d cycles for Rocket CPU to finish the task of matrixmul.\n", end1 - start1, end2 - start2, end3 - start3);
    return 0;
}
