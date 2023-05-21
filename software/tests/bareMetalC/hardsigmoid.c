#include <stdio.h>
#include <string.h>

/* Include polybench common header. */
//#include "include/polybench.h"

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"
#include "include/half.h"

#define SIZE 1000
float A_orig[SIZE] __attribute__((aligned(8)));
float B_orig[SIZE] __attribute__((aligned(8)));
short int A[SIZE] __attribute__((aligned(8)));
short int B[SIZE] __attribute__((aligned(8)));
short int C[SIZE] __attribute__((aligned(8)));

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[28][3] __attribute__((aligned(8))) = {
		{0x3155, 0x001b, 0x0290},
		{0x0003, 0x0000, 0x0291},
		{0x0010, 0x0000, 0x02f8},
		{0x0000, 0x0000, 0x02f9},
		{0x4200, 0x0019, 0x0358},
		{0x0004, 0x0000, 0x0359},
		{0x0000, 0x0000, 0x03c8},
		{0x0000, 0x0001, 0x03c9},
		{0xc200, 0x001f, 0x0428},
		{0x0018, 0x0000, 0x0429},
		{0x4200, 0x001e, 0x0430},
		{0x0003, 0x0000, 0x0431},
		{0x0000, 0x0000, 0x0489},
		{0x0080, 0x0000, 0x0490},
		{0x0000, 0x0008, 0x0498},
		{0x4000, 0x4000, 0x04e8},
		{0x001f, 0x0000, 0x04e9},
		{0x0000, 0x0000, 0x04ea},
		{0x0000, 0x0000, 0x04eb},
		{0x0800, 0x0000, 0x04ec},
		{0x0000, 0x0000, 0x04ed},
		{0x5000, 0x4000, 0x04f8},
		{0x001f, 0x0000, 0x04f9},
		{0x0000, 0x0000, 0x04fa},
		{0x0000, 0x0000, 0x04fb},
		{0xc800, 0x0004, 0x04fc},
		{0x0000, 0x0000, 0x04fd},
		{0x2000, 0x0000, 0x04fe},
	};

	load_cfg((void*)cin, 0x30000, 168);
	load_data(din_addr[0], 0x18000, 2000, 0);
	config(0x0, 28);
	execute(0x5000);
	store(dout_addr[0], 0x1a000, 2000);
}

void init_array()
{
int i,j;
  for(i = 0; i < SIZE; i ++){
    A_orig[i] = 0.01 * i - 5.00;
    A[i] = from_float_to_half(A_orig[i]);
  }
}

void result_check()
{
  for(int i = 0; i < SIZE; i ++){
    printf("The number in location %d is %d\n", i, (int)(1000 * from_half_to_float(C[i])));
  }
}

int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  init_array();
  printf("Initialization finished!\n");

  start = rdcycle();
  void* cgra_din_addr[2] = {A};
  void* cgra_dout_addr[1] = {C};
  volatile int result;
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done(%d)!\n", result);

  return 0;
}