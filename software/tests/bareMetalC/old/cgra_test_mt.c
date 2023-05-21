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

#define sizeofBank 4096
// conv3
#define in_port1_0 0
#define out_port1_0 2
#define out_port1_1 7
// simple
#define in_port2_0 5
#define in_port2_1 4
#define out_port2_0 6
// matrix
#define in_port3_0 0
#define in_port3_1 5
#define out_port3_0 4
#define cfg_port 8
#define config_num1 42
#define config_num2 17
#define config_num3 33

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
        {0x0800, 0x03e0, 0x04}, //
        {0x0000, 0x0000, 0x05},
        {0x0080, 0x0000, 0x06},
        {0x0800, 0x03e0, 0x0c},
        {0x0000, 0x0000, 0x0d},
        {0x4080, 0x0808, 0x0e},
        {0x1000, 0x0000, 0x1c},
        {0x0000, 0x0000, 0x24},
        {0x0001, 0x0000, 0x2c},
        {0x8401, 0x0000, 0x2d},
        {0x0001, 0x0000, 0x30},
        {0x8001, 0x0000, 0x31},
        {0x0001, 0x0000, 0x34},
        {0x8801, 0x0000, 0x35},
        {0x4d81, 0x0000, 0x39},
        {0x0000, 0x0000, 0x44},
        {0x0000, 0x0000, 0x48},
        {0x0000, 0x0000, 0x4c},
        {0x0000, 0x0000, 0x50},
        {0x0001, 0x0000, 0x54},
        {0x8401, 0x0000, 0x55},
        {0x0001, 0x0000, 0x58},
        {0x8001, 0x0000, 0x59},
        {0x0001, 0x0000, 0x5c},
        {0x8001, 0x0000, 0x5d},
        {0x0901, 0x0000, 0x61},
        {0x0000, 0x0000, 0x6c},
        {0x0000, 0x0000, 0x70},
        {0x0010, 0x0000, 0x74},
        {0x0001, 0x0000, 0x80},
        {0x8001, 0x0000, 0x81},
        {0x0801, 0x0000, 0x85},
        {0x0001, 0x0000, 0x88},
        {0x8001, 0x0000, 0x89},
        {0x0000, 0x0000, 0x98},
        {0x0100, 0x0000, 0x9c},
        {0x0001, 0x0000, 0xb0},
        {0x8001, 0x0000, 0xb1},
        {0x0100, 0x0000, 0xc4},
        {0x0c00, 0x03e0, 0xd8},
        {0x0000, 0x0000, 0xd9}, //
        {0x3080, 0x0008, 0xda}  //
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
        {0x0800, 0x0640, 0xcc},
        {0x0000, 0x0000, 0xcd},
        {0x0080, 0x0000, 0xce},
        {0x0c00, 0x0640, 0xd0},
        {0x0000, 0x0000, 0xd1},
        {0x0080, 0x0000, 0xd2},
        {0x0800, 0x0640, 0xd4},
        {0x0000, 0x0000, 0xd5},
        {0x2880, 0x0008, 0xd6}
    };

    struct config cin3[config_num3] __attribute__((aligned(8))) = {
        {0x0800, 0x0500, 0x04},
        {0x0000, 0x0000, 0x05},
        {0x0080, 0x0000, 0x06},
        {0x0000, 0x0000, 0x1c},
        {0x0000, 0x0000, 0x30},
        {0x8001, 0x0000, 0x31},
        {0x0000, 0x0000, 0x34},
        {0x8801, 0x0000, 0x35},
        {0x0000, 0x0000, 0x38},
        {0x8803, 0x0000, 0x39},
        {0x0000, 0x0000, 0x44},
        {0x0100, 0x0000, 0x48},
        {0x0000, 0x0000, 0x4c},
        {0x0000, 0x0000, 0x54},
        {0x8401, 0x0000, 0x55},
        {0x0000, 0x0000, 0x58},
        {0x8803, 0x0000, 0x59},
        {0x2011, 0x0000, 0x5d},
        {0x0000, 0x0000, 0x68},
        {0x0000, 0x0000, 0x6c},
        {0x0000, 0x0000, 0x70},
        {0x0481, 0x0000, 0x7d},
        {0x4403, 0x0000, 0x81},
        {0x0000, 0x0000, 0x94},
        {0x6601, 0x0000, 0xa5},
        {0x0000, 0x0000, 0xb8},
        {0x0000, 0x0000, 0xbc},
        {0x0800, 0x0500, 0xcc},
        {0x0000, 0x0000, 0xcd},
        {0x4080, 0x0008, 0xce},
        {0x0c00, 0x0500, 0xd0},
        {0x0000, 0x0000, 0xd1},
        {0x0880, 0x0000, 0xd2}
    };
//    unsigned short cout[config_num*3] __attribute__((aligned(8)));

    int in_addr1_0 = in_port1_0 * sizeofBank;
    int in_addr2_0 = in_port2_0 * sizeofBank;
    int in_addr2_1 = in_port2_1 * sizeofBank;
    int in_addr3_0 = in_port3_0 * sizeofBank;
    int in_addr3_1 = in_port3_1 * sizeofBank;
    int out_addr1_0 = out_port1_0 * sizeofBank;
    int out_addr1_1 = out_port1_1 * sizeofBank;
    int out_addr2_0 = out_port2_0 * sizeofBank;
    int out_addr3_0 = out_port3_0 * sizeofBank;
    int config_addr1 = cfg_port * sizeofBank;
//    int config_offset1 = 0
    int config_offset2 = (config_num1*6/8+2)*8;
    int config_offset3 = ((config_num1+config_num2)*6/8+4)*8;
    int config_addr2 = config_addr1 + config_offset2;
    int config_addr3 = config_addr1 + config_offset3;

    int NUM1 = 31;
    int NUM2 = 50;
    int NUM3 = 40;
    int NUM = NUM2;
    int len1 = 4*NUM1;
    int len2 = 4*NUM2;
    int len3 = 4*NUM3;
    int config_len1 = config_num1 * sizeof(struct config);
    int config_len2 = config_num2 * sizeof(struct config);
    int config_len3 = config_num3 * sizeof(struct config);
    int result;
    unsigned int din1[NUM] __attribute__((aligned(8)));
    unsigned int din2[NUM] __attribute__((aligned(8)));
    unsigned int dout1_0[NUM1] __attribute__((aligned(8)));
    unsigned int dout1_1[NUM1] __attribute__((aligned(8)));
    unsigned int dout2_0[NUM2] __attribute__((aligned(8)));
    unsigned int dout3_0[NUM3] __attribute__((aligned(8)));
    long long unsigned int start1, end1, start2, end2, start3, end3;
    uint64_t iob_ens;
    int task_id = 0;

    for(int i = 0; i < NUM; i++){
        din1[i] = i + 1;
        din2[i] = 2 * i;
    }

    // conv3
    start1 = rdcycle();
    task_id = 0;
    load_cfg(cin1, config_addr1, config_len1, task_id, 0);
    load_data(din1, in_addr1_0, len1, task_id, 0);
    config(0, config_num1, task_id, 0);
    iob_ens = (1 << in_port1_0) | (1 << out_port1_0) | (1 << out_port1_1);
    execute(iob_ens, task_id, 0);
    store(dout1_0, out_addr1_0, len1, task_id, 0);
    store(dout1_1, out_addr1_1, len1, task_id, 0);
    end1 = rdcycle();

    // simple
    start2 = rdcycle();
    task_id = 1;
    load_cfg(cin2, config_addr2, config_len2, task_id, 0);
    load_data(din1, in_addr2_0, len2, task_id, 0);
    load_data(din2, in_addr2_1, len2, task_id, 0);
    config(config_offset2/8, config_num2, task_id, 0);
    iob_ens = (1 << in_port2_0) | (1 << in_port2_1) | (1 << out_port2_0);
    execute(iob_ens, task_id, 0);
    store(dout2_0, out_addr2_0, len2, task_id, 0);
    end2 = rdcycle();

    start3 = rdcycle();
    task_id = 2;
    load_cfg(cin3, config_addr3, config_len3, task_id, 0);
    load_data(din1, in_addr3_0, len3, task_id, 0);
    load_data(din2, in_addr3_1, len3, task_id, 2);
    config(config_offset3/8, config_num3, task_id, 0);
    iob_ens = (1 << in_port3_0) | (1 << in_port3_1) | (1 << out_port3_0);
    execute(iob_ens, task_id, 0);
    store(dout3_0, out_addr3_0, len3, task_id, 0);
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
