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

#define NTAPS 32

int input[NTAPS] __attribute__((aligned(8)));
int output[NTAPS] __attribute__((aligned(8)));
int res[NTAPS] __attribute__((aligned(8)));
int coefficients[NTAPS] __attribute__((aligned(8)))= {025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125};


void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[64][3] __attribute__((aligned(8))) = {
		{0x9803, 0x2000, 0x0004},
		{0xf900, 0x0107, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x9003, 0x2000, 0x0008},
		{0xf900, 0x0107, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x8802, 0x2000, 0x000c},
		{0xf900, 0x0107, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x8002, 0x2000, 0x0010},
		{0xf900, 0x0107, 0x0011},
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x0400, 0x0000, 0x002c},
		{0x0100, 0x0000, 0x0030},
		{0x0000, 0x0000, 0x0034},
		{0x2403, 0x0000, 0x0051},
		{0x4403, 0x0000, 0x0055},
		{0x0100, 0x0000, 0x0074},
		{0x4100, 0x0000, 0x0078},
		{0x4421, 0x0000, 0x0099},
		{0x8411, 0x0000, 0x009d},
		{0x0000, 0x0000, 0x00c0},
		{0x0000, 0x0000, 0x00c4},
		{0x040d, 0x0000, 0x00e5},
		{0x0000, 0x7001, 0x00e6},
		{0x0020, 0x0008, 0x00e7},
		{0x9001, 0x0000, 0x00e9},
		{0x0000, 0x0000, 0x0108},
		{0x0001, 0x0000, 0x0110},
		{0x4c41, 0x0000, 0x0129},
		{0x8c03, 0x0000, 0x0131},
		{0x8c03, 0x0000, 0x0135},
		{0x0110, 0x0000, 0x014c},
		{0x0010, 0x0000, 0x0154},
		{0x0010, 0x0000, 0x0158},
		{0x0000, 0x0000, 0x015c},
		{0x1000, 0x2000, 0x016c},
		{0x0040, 0x0100, 0x016d},
		{0x0000, 0x0300, 0x016e},
		{0x0000, 0x0000, 0x016f},
		{0x1800, 0x2000, 0x0170},
		{0x0040, 0x0100, 0x0171},
		{0x0000, 0x9300, 0x0172},
		{0x0000, 0x0000, 0x0173},
		{0x8801, 0x2000, 0x0174},
		{0xf900, 0x0107, 0x0175},
		{0x0000, 0x0100, 0x0176},
		{0x0000, 0x0000, 0x0177},
		{0x8000, 0x2000, 0x0178},
		{0xf900, 0x0107, 0x0179},
		{0x0000, 0x0100, 0x017a},
		{0x0000, 0x0000, 0x017b},
		{0x8801, 0x2000, 0x017c},
		{0xf900, 0x0107, 0x017d},
		{0x0000, 0x0100, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x8000, 0x2000, 0x0180},
		{0xf900, 0x0107, 0x0181},
		{0x0000, 0x0100, 0x0182},
		{0x0000, 0x0000, 0x0183},
	};

	load_cfg((void*)cin, 0x20000, 384, 0, 0);
	load_data(din_addr[0], 0x10000, 128, 1, 0, 0);
	load_data(din_addr[1], 0x12000, 128, 1, 0, 0);
	load_data(din_addr[2], 0x2000, 128, 1, 0, 0);
	load_data(din_addr[3], 0x6000, 128, 0, 0, 0);
	load_data(din_addr[4], 0x18000, 128, 1, 0, 0);
	load_data(din_addr[5], 0x1a000, 128, 1, 0, 0);
	load_data(din_addr[6], 0x0, 128, 1, 0, 0);
	load_data(din_addr[7], 0x4000, 128, 0, 0, 0);
	load_data(din_addr[8], 0x14000, 128, 0, 0, 0);
	config(0x0, 64, 0, 0);
	execute(0x3f0f, 0, 0);
	store(dout_addr[0], 0x16000, 128, 0, 0);
}

/* Array initialization. */
void init_array()
{
  int i, j;

  for (i = 0; i < NTAPS; i++)
    {
      input[i] = i;
      output[i] = 0;
      res[i] = 0;
    }
}


__attribute__((noinline))
void fir()//(int input[], int output[], int coefficients[])
/*   input :           input sample array */
/*   output:           output sample array */
/*   coefficient:      coefficient array */
{
  int i;
  int j = 0;

 for(j=0; j< NTAPS; ++j) {
 	  int sum = 0;
//    #pragma unroll 4
    for (i = 0; i < NTAPS; ++i) {
      #ifdef CGRA_COMPILER
      please_map_me();
      #endif
      sum += input[i] * coefficients[i];
    }
    res[j] += sum;
 }
}


void result_check()
{
  int i, j;

  for (i = 0; i < NTAPS; i++)
  {
    if (res[i] != output[i]) printf("There is an error in location (%d)[%d, %d]\n", i, res[i], output[i]);
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
  fir();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[9] = {coefficients, coefficients, coefficients, coefficients, input, input, input, input, output};
  void* cgra_dout_addr[1] = {output};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}