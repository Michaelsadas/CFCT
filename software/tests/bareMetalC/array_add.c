#include <stdio.h>
#include <string.h>

/* Include polybench common header. */
//#include "include/polybench.h"

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"
#include "include/half.h"

#define SIZE  100
float A_orig[SIZE] __attribute__((aligned(8)));
float B_orig[SIZE] __attribute__((aligned(8)));
short int A[SIZE] __attribute__((aligned(8)));
short int B[SIZE] __attribute__((aligned(8)));
short int C[SIZE] __attribute__((aligned(8)));

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[24][3] __attribute__((aligned(8))) = {
		{0x4000, 0x0000, 0x0008},
		{0x0020, 0x0000, 0x0009},
		{0x0000, 0x0000, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x0800, 0x0000, 0x000c},
		{0x0000, 0x0000, 0x000d},
		{0x0000, 0x0000, 0x0020},
		{0x0002, 0x0000, 0x0050},
		{0x0000, 0xc034, 0x0068},
		{0x0000, 0xc004, 0x0069},
		{0x0640, 0x0001, 0x006a},
		{0x0001, 0x0000, 0x0088},
		{0x5000, 0x0000, 0x0098},
		{0x0020, 0x0000, 0x0099},
		{0x0000, 0x0000, 0x009a},
		{0x0000, 0x0000, 0x009b},
		{0x0800, 0x0000, 0x009c},
		{0x0000, 0x0000, 0x009d},
		{0x6000, 0x0000, 0x00a0},
		{0x0020, 0x0000, 0x00a1},
		{0x0000, 0x0000, 0x00a2},
		{0x0000, 0x0000, 0x00a3},
		{0x7800, 0x0004, 0x00a4},
		{0x0000, 0x0000, 0x00a5},
	};

	load_cfg((void*)cin, 0x8000, 144);
	load_data(din_addr[0], 0x0, 200, 0);
	load_data(din_addr[1], 0x2000, 200, 0);
	config(0x0, 24);
	execute(0xd);
	store(dout_addr[0], 0x4000, 200);
}

/* Array initialization. */
void init_array()
{
int i,j;
  for(i = 0; i < SIZE; i ++){
    A_orig[i] = i;
    A[i] = from_float_to_half(A_orig[i]);
    B_orig[i] = i;
    B[i] = from_float_to_half(B_orig[i]);
  }
}

void result_check()
{
  for(int i = 0; i < SIZE; i ++){
    printf("The number in location %d is %d\n", i, (int)from_half_to_float(C[i]));
  }
}

int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  init_array();
  printf("Initialization finished!\n");

  start = rdcycle();
  void* cgra_din_addr[2] = {A, B};
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