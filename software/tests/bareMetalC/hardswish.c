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
	volatile unsigned short cin[36][3] __attribute__((aligned(8))) = {
		{0x4000, 0x4000, 0x0028},
		{0x001f, 0x0000, 0x0029},
		{0x0000, 0x0000, 0x002a},
		{0x0000, 0x0000, 0x002b},
		{0x0800, 0x0000, 0x002c},
		{0x0000, 0x0000, 0x002d},
		{0x5000, 0x4000, 0x0030},
		{0x001f, 0x0000, 0x0031},
		{0x0000, 0x0000, 0x0032},
		{0x0000, 0x0000, 0x0033},
		{0x0800, 0x0005, 0x0034},
		{0x0000, 0x0000, 0x0035},
		{0x0000, 0x0000, 0x0036},
		{0x1800, 0x0000, 0x0098},
		{0x0000, 0x0000, 0x0099},
		{0x0008, 0x0000, 0x00a1},
		{0x8000, 0x0000, 0x00a9},
		{0xc200, 0x001f, 0x0100},
		{0x0001, 0x0000, 0x0101},
		{0x0000, 0x1800, 0x0168},
		{0xc000, 0x0000, 0x0170},
		{0x0000, 0x0000, 0x0178},
		{0x0000, 0x301b, 0x01d0},
		{0x0014, 0x0000, 0x01d1},
		{0x4200, 0x001e, 0x01d8},
		{0x0001, 0x0000, 0x01d9},
		{0x0000, 0x1800, 0x0238},
		{0x0000, 0x0000, 0x0240},
		{0x0000, 0x0000, 0x0241},
		{0x1000, 0x0000, 0x0248},
		{0x3155, 0x001b, 0x02a8},
		{0x0002, 0x0000, 0x02a9},
		{0x4200, 0x0019, 0x02b0},
		{0x0001, 0x0000, 0x02b1},
		{0x0000, 0x1000, 0x0308},
		{0x0000, 0x0000, 0x0310},
	};

	load_cfg((void*)cin, 0x30000, 216);
	load_data(din_addr[0], 0x8000, 2000, 0);
	config(0x0, 36);
	execute(0x30);
	store(dout_addr[0], 0xa000, 2000);
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
  for(int i = 0; i < 600; i ++){
    printf("The number in location %d is %d\n", i, (int)(1000 * from_half_to_float(C[i + 200])));
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