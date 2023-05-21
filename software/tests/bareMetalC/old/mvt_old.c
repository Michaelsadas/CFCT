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
#define TYPE float
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
	volatile unsigned short cin[24][3] __attribute__((aligned(8))) = {
		{0x0000, 0x8000, 0x000c},
		{0x0040, 0x0100, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x0800, 0x8000, 0x0010},
		{0x0040, 0x0100, 0x0011},
		{0x0000, 0x8d00, 0x0012},
		{0x0080, 0x0000, 0x0013},
		{0x2800, 0x8000, 0x0014},
		{0xf840, 0x0107, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x2000, 0x8000, 0x0018},
		{0x0040, 0x0100, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x0000, 0x0000, 0x0034},
		{0x4001, 0x0000, 0x0038},
		{0x1400, 0x0000, 0x003c},
		{0x4421, 0x0000, 0x0059},
		{0x080d, 0x0000, 0x005d},
		{0x0000, 0x4001, 0x005e},
		{0x0080, 0x0008, 0x005f},
		{0x2403, 0x0000, 0x0061},
	};

	load_cfg((void*)cin, 0x20000, 144, 0, 0);
	load_data(din_addr[0], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x0, 128, 0, 0, 0);
	load_data(din_addr[2], 0xa000, 128, 0, 0, 0);
	config(0x0, 24, 0, 0);
	execute(0x3c, 0, 0);
	store(dout_addr[0], 0x2000, 128, 0, 0);
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
    if ((int)x1[i] != (int)x2[i]) printf("There is an error in location (%d)\n", i);
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