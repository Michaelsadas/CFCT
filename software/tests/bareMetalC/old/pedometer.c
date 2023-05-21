#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
//#include "include/polybench.h"

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"
#define TYPE int
#define SIZE 1024

//typedef struct Acceleration Acceleration;
//struct Acceleration {
//    int x;
//    int y;
//    int z;
//};

//unsigned char second = 0;
//unsigned char minute = 0;
//unsigned char hour = 0;
//unsigned char state = 0;
//unsigned char I1_flag = 0;
//unsigned char I2_flag = 0;
//unsigned char timerFlag1 = 0;
//unsigned char timerFlag2 = 0;
//unsigned char aux=0;
//char Int_SourceSystem =0;
//char Int_SourceTrans=0;
//unsigned char length;
//char buffer[14];
//int step[SIZE]={};
//int km[SIZE]={};
TYPE acc_vector[SIZE] __attribute__((aligned(8)));
TYPE res_vector[SIZE] __attribute__((aligned(8)));

//int i;
//int sub_x;
//int sub_y;
//int sub_z;
TYPE acceleration_x[SIZE] __attribute__((aligned(8)));
TYPE acceleration_y[SIZE] __attribute__((aligned(8)));
TYPE acceleration_z[SIZE] __attribute__((aligned(8)));
TYPE acc_avg_x[SIZE] __attribute__((aligned(8)));
TYPE acc_avg_y[SIZE] __attribute__((aligned(8)));
TYPE acc_avg_z[SIZE] __attribute__((aligned(8)));

//long long unsigned start;
//long long unsigned end;
//int result;
void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[45][3] __attribute__((aligned(8))) = {
		{0x2000, 0x0000, 0x0004},
		{0x0010, 0x0000, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x2800, 0x0000, 0x0008},
		{0x0010, 0x0000, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x0400, 0x0000, 0x002c},
		{0x9002, 0x0000, 0x0051},
		{0x0000, 0x0000, 0x0078},
		{0x9003, 0x0000, 0x009d},
		{0x0100, 0x0000, 0x00c0},
		{0x4003, 0x0002, 0x00e1},
		{0x9001, 0x0001, 0x00e5},
		{0x0000, 0x0040, 0x0108},
		{0x0800, 0x0000, 0x010c},
		{0x4002, 0x0002, 0x0125},
		{0xb003, 0x0001, 0x0129},
		{0x3002, 0x0002, 0x012d},
		{0x9001, 0x0000, 0x0131},
		{0x0009, 0x0000, 0x014c},
		{0x0010, 0x0001, 0x0150},
		{0x0000, 0x0000, 0x0154},
		{0x0000, 0x0000, 0x0158},
		{0x3000, 0x0000, 0x016c},
		{0x0010, 0x0000, 0x016d},
		{0x0000, 0x0100, 0x016e},
		{0x0000, 0x0000, 0x016f},
		{0x2000, 0x0000, 0x0170},
		{0x0010, 0x0000, 0x0171},
		{0x0000, 0x0100, 0x0172},
		{0x0000, 0x0000, 0x0173},
		{0x3800, 0x0000, 0x0174},
		{0x0010, 0x0000, 0x0175},
		{0x0000, 0x0100, 0x0176},
		{0x0000, 0x0000, 0x0177},
		{0x2800, 0x0000, 0x0178},
		{0x0010, 0x0000, 0x0179},
		{0x0000, 0x0100, 0x017a},
		{0x0000, 0x0000, 0x017b},
		{0x2000, 0x0000, 0x017c},
		{0x0010, 0x0000, 0x017d},
		{0x0000, 0x8f00, 0x017e},
		{0x0000, 0x0000, 0x017f},
	};

	load_cfg((void*)cin, 0x20000, 270, 1);
	load_data(din_addr[0], 0x12000, 4096, 1);
	load_data(din_addr[1], 0x2000, 4096, 1);
	load_data(din_addr[2], 0x16000, 4096, 1);
	load_data(din_addr[3], 0x10000, 4096, 1);
	load_data(din_addr[4], 0x0, 4096, 1);
	load_data(din_addr[5], 0x14000, 4096, 1);
	config(0x0, 45, 1);
	execute(0x1f03, 1);
	store(dout_addr[0], 0x18000, 4096, 1);
}

/* Array initialization. */
static
void init_array()
{
	for(int i=0;i<SIZE;i++){    
//    	sub_x = 0;
//    	sub_y = 0;
//    	sub_z = 0;

    	acceleration_x[i] = 3*i;
    	acceleration_y[i] = 3*i+1;
    	acceleration_z[i] = 3*i+2;
    
    	acc_avg_x[i] = i;
    	acc_avg_y[i] = i;
    	acc_avg_z[i] = i;
 	}
}


__attribute__((noinline))
void pedometer(){//(int input[], int output[], int coefficients[])
	for(int i=0;i<SIZE;i++){
		res_vector[i] = (acceleration_x[i]- acc_avg_x[i]) * (acceleration_x[i]- acc_avg_x[i])+  (acceleration_y[i]- acc_avg_y[i]) * (acceleration_y[i]- acc_avg_y[i])+ (acceleration_z[i]-acc_avg_z[i]) * (acceleration_z[i]-acc_avg_z[i]) ;
	}
}

static
void result_check()
{
  int i, j;

  for (i = 0; i < SIZE; i++)
  {
    if (acc_vector[i] != res_vector[i]) printf("There is an error in location (%d)[%d, %d]\n", i, (int)acc_vector[i], (int)res_vector[i]);
  }
}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  init_array();
  printf("Initialization finished!\n");

  start = rdcycle();
  /* Run kernel. */
  pedometer();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[6] = {acc_avg_x, acc_avg_y, acc_avg_z, acceleration_x, acceleration_y, acceleration_z};
  void* cgra_dout_addr[1] = {acc_vector};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}