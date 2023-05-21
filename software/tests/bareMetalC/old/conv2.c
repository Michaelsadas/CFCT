#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"
#define TYPE int
#define N 1024
TYPE a[N];
TYPE b[N];
TYPE c[N];

//void cgra_execute(void** din_addr, void** dout_addr)
//{
//	volatile unsigned short cin[23][3] __attribute__((aligned(8))) = {
//		{0x2000, 0xfc00, 0x0004},
//		{0x000f, 0x0000, 0x0005},
//		{0x0000, 0x0100, 0x0006},
//		{0x0000, 0x0000, 0x0007},
//		{0x0000, 0x4120, 0x004c},
//		{0x1008, 0x0000, 0x004d},
//		{0x0000, 0x0000, 0x0070},
//		{0x0002, 0x0000, 0x00b8},
//		{0xc046, 0x0000, 0x00dd},
//		{0x0000, 0x0000, 0x0100},
//		{0x0001, 0x0000, 0x0104},
//		{0x0000, 0x41a0, 0x0124},
//		{0x4008, 0x0000, 0x0125},
//		{0x0001, 0x0000, 0x0148},
//		{0x0000, 0x0000, 0x014c},
//		{0x2800, 0xfc00, 0x016c},
//		{0x000f, 0x0000, 0x016d},
//		{0x0000, 0x8f00, 0x016e},
//		{0x0000, 0x0000, 0x016f},
//		{0x2001, 0xfc00, 0x0170},
//		{0x000f, 0x0000, 0x0171},
//		{0x0000, 0x0100, 0x0172},
//		{0x0000, 0x0000, 0x0173},
//	};
//
//	load_cfg((void*)cin, 0x20000, 138, 0, 0);
//	load_data(din_addr[0], 0x0, 4096, 1, 0, 0);
//	load_data(din_addr[1], 0x10000, 4096, 0, 0, 0);
//	config(0x0, 23, 0, 0);
//	execute(0x301, 0, 0);
//	store(dout_addr[0], 0x12000, 4092, 0, 0);
//}

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[20][3] __attribute__((aligned(8))) = {
		{0x2801, 0xfc00, 0x0018},
		{0x000f, 0x0000, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x2000, 0xfc00, 0x001c},
		{0x000f, 0x0000, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x3000, 0xfc00, 0x0020},
		{0x000f, 0x0000, 0x0021},
		{0x0000, 0x8b00, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0000, 0x0000, 0x0040},
		{0x0030, 0x0000, 0x0044},
		{0x0014, 0x0000, 0x0064},
		{0x1003, 0x0000, 0x0065},
		{0x000a, 0x0000, 0x0068},
		{0x1003, 0x0000, 0x0069},
		{0x0400, 0x0000, 0x008c},
		{0x9001, 0x0000, 0x00b1},
	};

	load_cfg((void*)cin, 0x20000, 120, 1);
	load_data(din_addr[0], 0x8000, 4096, 1);
	load_data(din_addr[1], 0xa000, 4096, 1);
	config(0x0, 20, 1);
	execute(0xe0, 1);
	store(dout_addr[0], 0xc000, 4092, 1);
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
void conv2(){
  int i;
  for (i = 0; i < N-1; i++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif 
    b[i] = a[i] * 10 + a[i + 1] * 20; //+ a[i+ 2] * 3; 
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
  conv2();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[2] = {a, a};
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