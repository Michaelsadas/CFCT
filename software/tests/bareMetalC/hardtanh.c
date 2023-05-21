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
	volatile unsigned short cin[21][3] __attribute__((aligned(8))) = {
		{0x3c00, 0x001e, 0x0370},
		{0x0003, 0x0000, 0x0371},
		{0x0010, 0x0000, 0x03d8},
		{0x0000, 0x0000, 0x03e1},
		{0xbc00, 0x001f, 0x0438},
		{0x0004, 0x0000, 0x0439},
		{0x0000, 0x0000, 0x04a8},
		{0x0200, 0x0000, 0x04b0},
		{0x4000, 0x4000, 0x0510},
		{0x001f, 0x0000, 0x0511},
		{0x0000, 0x0000, 0x0512},
		{0x0000, 0x0000, 0x0513},
		{0x0800, 0x0000, 0x0514},
		{0x0000, 0x0000, 0x0515},
		{0x5000, 0x4000, 0x0518},
		{0x001f, 0x0000, 0x0519},
		{0x0000, 0x0000, 0x051a},
		{0x0000, 0x0000, 0x051b},
		{0x5800, 0x0004, 0x051c},
		{0x0000, 0x0000, 0x051d},
		{0x0000, 0x0000, 0x051e},
	};

	load_cfg((void*)cin, 0x30000, 126);
	load_data(din_addr[0], 0x20000, 2000, 0);
	config(0x0, 21);
	execute(0x60000);
	store(dout_addr[0], 0x22000, 2000);
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
  for(int i = 0; i < 200; i ++){
    printf("The number in location %d is %d\n", i, (int)(1000 * from_half_to_float(C[i + 400])));
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