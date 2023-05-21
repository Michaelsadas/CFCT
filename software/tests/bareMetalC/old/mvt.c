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

//#define POLYBENCH_USE_SCALAR_LB
#define N 32
#define _PB_N 32
//#define int_IS_INT
#define TYPE int
int n = N;
int alpha;
int beta;
//long long unsigned start;
//long long unsigned end;
//int result;

TYPE A[N][N] __attribute__((aligned(8)));
TYPE x1[N] __attribute__((aligned(8)));
TYPE x2[N] __attribute__((aligned(8)));
TYPE y_1[N] __attribute__((aligned(8)));
TYPE y_2[N] __attribute__((aligned(8)));

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[23][3] __attribute__((aligned(8))) = {
		{0x3013, 0x0002, 0x0135},
		{0x0000, 0xc004, 0x0136},
		{0x0200, 0x0020, 0x0137},
		{0xb041, 0x0001, 0x0139},
		{0x0010, 0x0000, 0x0158},
		{0x0010, 0x0000, 0x015c},
		{0x0000, 0x0000, 0x0160},
		{0x2000, 0x8000, 0x0178},
		{0xf840, 0x0107, 0x0179},
		{0x0000, 0x0100, 0x017a},
		{0x0000, 0x0000, 0x017b},
		{0x0800, 0x8000, 0x017c},
		{0x0040, 0x0100, 0x017d},
		{0x0000, 0x0100, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x2000, 0x8000, 0x0180},
		{0x0040, 0x0100, 0x0181},
		{0x0000, 0x0100, 0x0182},
		{0x0000, 0x0000, 0x0183},
		{0x1000, 0x8000, 0x0184},
		{0x0040, 0x0100, 0x0185},
		{0x0000, 0x8b00, 0x0186},
		{0x0000, 0x0000, 0x0187},
	};

	load_cfg((void*)cin, 0x20000, 138, 1);
	load_data(din_addr[0], 0x18000, 4096, 1);
	load_data(din_addr[1], 0x1a000, 128, 1);
	load_data(din_addr[2], 0x10000, 128, 1);
	config(0x0, 23, 1);
	execute(0x7800, 1);
	store(dout_addr[0], 0x1c000, 128, 1);
}

/* Array initialization. */
void init_array()
{
  int i, j;

  for (i = 0; i < n; i++)
    {
      x1[i] = (int) (i % n); // / n;
      x2[i] = (int) (i % n); // / n;
      y_1[i] = (int) ((i + 3) % n); // / n;
      y_2[i] = (int) ((i + 4) % n); // / n;
      for (j = 0; j < n; j++)
	    A[i][j] = (int) (i*j % n); // / n;
      }
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void kernel_mvt()
{
  int i, j;
  TYPE s;

  for (i = 0; i < _PB_N; i++)
  {
    s = 0;
    for (j = 0; j < _PB_N; j++)
    {
      s += A[i][j] * y_1[j];
    }
    x2[i] += s;
  }
}


void result_check()
{
  int i, j;

  for (i = 0; i < _PB_N; i++)
  {
    if ((int)x1[i] != (int)x2[i]) printf("There is an error in location (%d) [%d, %d]\n", i, x1[i], x2[i]);
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
  kernel_mvt();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

//  void* cgra_din_addr[9] = {A, y_1, A, y_1, (void*)A+8, (void*)y_1+8, (void*)A+8, (void*)y_1+8, x1};
//  void* cgra_dout_addr[1] = {x1};
  void* cgra_din_addr[3] = {A, x1, y_1};
  void* cgra_dout_addr[1] = {x1};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}