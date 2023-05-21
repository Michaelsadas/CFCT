#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"
#define TYPE float
#define N 1024
TYPE a[N];
TYPE b[N];
TYPE c[N];

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[30][3] __attribute__((aligned(8))) = {
		{0x2000, 0xf800, 0x000c},
		{0x000f, 0x0000, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x0000, 0x0000, 0x0034},
		{0x0000, 0x4120, 0x0058},
		{0x0403, 0x0000, 0x0059},
		{0x0400, 0x0000, 0x007c},
		{0x2c01, 0x0000, 0x00a1},
		{0x0130, 0x0000, 0x00c4},
		{0x6481, 0x0000, 0x00e9},
		{0x0040, 0x0200, 0x010c},
		{0x0000, 0x4040, 0x012c},
		{0x0c03, 0x0000, 0x012d},
		{0x0000, 0x41a0, 0x0130},
		{0x0c03, 0x0000, 0x0131},
		{0x0010, 0x0000, 0x0150},
		{0x0310, 0x0000, 0x0154},
		{0x2802, 0xf800, 0x0170},
		{0x000f, 0x0000, 0x0171},
		{0x0000, 0x0100, 0x0172},
		{0x0000, 0x0000, 0x0173},
		{0x2001, 0xf800, 0x0174},
		{0x000f, 0x0000, 0x0175},
		{0x0000, 0x0100, 0x0176},
		{0x0000, 0x0000, 0x0177},
		{0x3000, 0xf800, 0x0178},
		{0x000f, 0x0000, 0x0179},
		{0x0000, 0x8d00, 0x017a},
		{0x0000, 0x0000, 0x017b},
	};

	load_cfg((void*)cin, 0x20000, 180, 0, 0);
	load_data(din_addr[0], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[2], 0x12000, 4096, 0, 0, 0);
	config(0x0, 30, 0, 0);
	execute(0xe04, 0, 0);
	store(dout_addr[0], 0x14000, 4088, 0, 0);
}

/* Array initialization. */
static
void init_array()
{
    int i;
    for (i = 0; i < N; i++) {
        a[i] = i;
    }    
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void conv3(){
    int i;
    for (i = 0; i < N-2; i++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif 
        b[i] = a[i] * 10 + a[i + 1] * 20 + a[i+ 2] * 3;
    }
}

static
void result_check()
{
  int i, j;

  for (i = 0; i < N; i++)
  {
    if (c[i] != b[i]) printf("There is an error in location (%d)[%d, %d]\n", i, (int)c[i], (int)b[i]);
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
  conv3();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[3] = {a, a, a};
  void* cgra_dout_addr[1] = {c};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}