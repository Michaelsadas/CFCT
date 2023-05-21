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
	volatile unsigned short cin[17][3] __attribute__((aligned(8))) = {
		{0x0000, 0x001f, 0x0438},
		{0x0004, 0x0000, 0x0439},
		{0x0000, 0x0000, 0x04a0},
		{0x0001, 0x0000, 0x04a8},
		{0x4000, 0x4000, 0x0500},
		{0x001f, 0x0000, 0x0501},
		{0x0000, 0x0000, 0x0502},
		{0x0000, 0x0000, 0x0503},
		{0x4800, 0x0004, 0x0504},
		{0x0000, 0x0000, 0x0505},
		{0x2000, 0x0000, 0x0506},
		{0x4000, 0x4000, 0x0508},
		{0x001f, 0x0000, 0x0509},
		{0x0000, 0x0000, 0x050a},
		{0x0000, 0x0000, 0x050b},
		{0x0800, 0x0000, 0x050c},
		{0x0000, 0x0000, 0x050d},
	};

	load_cfg((void*)cin, 0x30000, 102);
	load_data(din_addr[0], 0x20000, 2000, 0);
	config(0x0, 17);
	execute(0x18000);
	store(dout_addr[0], 0x18000, 2000);
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
  for(int i = 0; i < 20; i ++){
    printf("The number in location %d is %d\n", i, (int)(1000 * from_half_to_float(C[i + 490])));
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