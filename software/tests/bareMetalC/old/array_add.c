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

#define SIZE  1024
float A[SIZE] __attribute__((aligned(8)));
float B[SIZE] __attribute__((aligned(8)));
float C[SIZE] __attribute__((aligned(8)));
float D[SIZE] __attribute__((aligned(8)));


void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[15][3] __attribute__((aligned(8))) = {
		{0x2800, 0x0000, 0x0004},
		{0x0010, 0x0000, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x2000, 0x0000, 0x0008},
		{0x0010, 0x0000, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x3000, 0x0000, 0x000c},
		{0x0010, 0x0000, 0x000d},
		{0x0000, 0x8900, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x0400, 0x0000, 0x002c},
		{0x0010, 0x0000, 0x0030},
		{0x2401, 0x0000, 0x0051},
	};

	load_cfg((void*)cin, 0x20000, 90, 0, 0);
	load_data(din_addr[0], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x2000, 4096, 0, 0, 0);
	config(0x0, 15, 0, 0);
	execute(0x7, 0, 0);
	store(dout_addr[0], 0x4000, 4096, 0, 0);
}

/* Array initialization. */
void init_array()
{
int i,j;
for (i=0;i<SIZE; i++){
      A[i] = i * 2 + 5;
      B[i] = i * 3;
//      C[i] = 0;
//      D[i] = 0;
    }
    
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void array_add(){

   
   for (int i=0;i<SIZE; i++){
      #ifdef CGRA_COMPILER
      please_map_me();
      #endif

      D[i] = A[i]+B[i];
   }
}

void result_check()
{
  int i, j;

  for (i = 0; i < SIZE; i++)
  {
//    if ((int)C[i] != (int)D[i]) 
    printf("There is an error in location (%d)[%d, %d]\n", i, (int)C[i], (int)D[i]);
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
  array_add();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[2] = {A, B};
  void* cgra_dout_addr[1] = {C};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}