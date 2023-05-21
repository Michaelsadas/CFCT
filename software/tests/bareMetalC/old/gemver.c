/**
 * gemver.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>


/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
//#include "include/gemver.h"
#include "include/encoding.h"
#include "include/ISA.h"

#define DATA_TYPE float
#define N 32
#define _PB_N 32

int n = N;
//DATA_TYPE alpha;
//DATA_TYPE beta;

DATA_TYPE A[N][N] __attribute__((aligned(8)));
DATA_TYPE O[N][N] __attribute__((aligned(8)));
//DATA_TYPE R[N][N];
DATA_TYPE u1[N] __attribute__((aligned(8)));
DATA_TYPE v1[N] __attribute__((aligned(8)));
DATA_TYPE u2[N] __attribute__((aligned(8)));
DATA_TYPE v2[N] __attribute__((aligned(8)));

/*POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);
POLYBENCH_2D_ARRAY_DECL(O, DATA_TYPE, N, N, n, n);
POLYBENCH_2D_ARRAY_DECL(R, DATA_TYPE, N, N, n, n);
POLYBENCH_1D_ARRAY_DECL(u1, DATA_TYPE, N, n);
POLYBENCH_1D_ARRAY_DECL(v1, DATA_TYPE, N, n);
POLYBENCH_1D_ARRAY_DECL(u2, DATA_TYPE, N, n);
POLYBENCH_1D_ARRAY_DECL(v2, DATA_TYPE, N, n);
*/
void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[33][3] __attribute__((aligned(8))) = {
		{0x3000, 0x8000, 0x0004},
		{0xf840, 0x0107, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x0800, 0x8000, 0x0008},
		{0x0040, 0x0100, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x3800, 0x8000, 0x000c},
		{0x0040, 0x0100, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x2000, 0x8000, 0x0010},
		{0xf840, 0x0107, 0x0011},
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x0000, 0x8000, 0x0014},
		{0x0040, 0x0100, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x2800, 0x8000, 0x0018},
		{0x0040, 0x0100, 0x0019},
		{0x0000, 0x8f00, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x0400, 0x0000, 0x002c},
		{0x0400, 0x0004, 0x0030},
		{0x1200, 0x0004, 0x0034},
		{0x4400, 0x0008, 0x0038},
		{0x0020, 0x0000, 0x003c},
		{0x2403, 0x0000, 0x0051},
		{0x2901, 0x0000, 0x0055},
		{0x4401, 0x0000, 0x0059},
		{0x2403, 0x0000, 0x005d},
	};

	load_cfg((void*)cin, 0x20000, 198, 0, 0);
	load_data(din_addr[0], 0x6000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x8000, 128, 0, 0, 0);
	load_data(din_addr[2], 0x2000, 128, 0, 0, 0);
	load_data(din_addr[3], 0x0, 128, 0, 0, 0);
	load_data(din_addr[4], 0x4000, 128, 0, 0, 0);
	config(0x0, 33, 0, 0);
	execute(0x3f, 0, 0);
	store(dout_addr[0], 0xa000, 4096, 0, 0);
}


/* Array initialization. */
void init_array ()
{
  int i, j;

//  alpha = 43532;
//  beta = 12313;

  for (i = 0; i < n; i++)
    {
      u1[i] = i;
      u2[i] = (i+1)/n/2;
      v1[i] = (i+1)/n/4;
      v2[i] = (i+1)/n/6;
      for (j = 0; j < n; j++)
      {
	       A[i][j] = ((DATA_TYPE) i*j) / n;
	       O[i][j] = ((DATA_TYPE) i*j) / n;
      }
    }
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void kernel_gemver()
{
  int i, j;

  for (i = 0; i < _PB_N; i++)
  {
    for (j = 0; j < _PB_N; j++)
    {
      O[i][j] += u1[i] * v1[j] + u2[i] * v2[j];
    }
  }
}

void result_check()
{
  int i, j;

  for (i = 0; i < _PB_N; i++)
  {
    for (j = 0; j < _PB_N; j++)
    {
      if (O[i][j] != A[i][j]) printf("There is an error in location (%d, %d)\n", i, j);
    }
  }
}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
//  printf("The number of N is %d", N);
  init_array();
  printf("Initialization finished!\n");
  start = rdcycle();
  /* Run kernel. */
  kernel_gemver();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[5] = {A, u1, u2, v1, v2};
  void* cgra_dout_addr[1] = {A};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();

  result_check();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

/*  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(O);
  POLYBENCH_FREE_ARRAY(R);
  POLYBENCH_FREE_ARRAY(u1);
  POLYBENCH_FREE_ARRAY(v1);
  POLYBENCH_FREE_ARRAY(u2);
  POLYBENCH_FREE_ARRAY(v2);
  */
  return 0;
}