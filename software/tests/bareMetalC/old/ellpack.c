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

#define TYPE float
#define N 494
#define L 4
TYPE nzval[N*L];
int cols[N*L];
TYPE vec[N];
TYPE out[N];
TYPE res[N];

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[31][3] __attribute__((aligned(8))) = {
		{0x2000, 0x1000, 0x0004},
		{0x0040, 0x0f70, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x2800, 0x1000, 0x0008},
		{0x0040, 0x0f70, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x1000, 0xe000, 0x000c},
		{0x001e, 0x0000, 0x000d},
		{0x0000, 0x0900, 0x000e},
		{0x0081, 0x0000, 0x000f},
		{0x1800, 0x1000, 0x0010},
		{0x0040, 0x0f70, 0x0011},
		{0x0000, 0x0700, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x0000, 0x1000, 0x0014},
		{0x0040, 0x0f70, 0x0015},
		{0x0000, 0x9700, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x1000, 0x0004, 0x002c},
		{0x0000, 0x0008, 0x0030},
		{0x0901, 0x0000, 0x0034},
		{0x0010, 0x0000, 0x0038},
		{0x080d, 0x0000, 0x004d},
		{0x0000, 0x8001, 0x004e},
		{0x8010, 0x007b, 0x004f},
		{0x2a03, 0x0000, 0x0051},
		{0x0004, 0x0000, 0x0054},
		{0x0403, 0x0000, 0x0055},
		{0x2441, 0x0000, 0x0059},
	};

	load_cfg((void*)cin, 0x20000, 186, 0, 0);
	load_data(din_addr[0], 0x2000, 7904, 0, 0, 0);
	load_data(din_addr[1], 0x0, 7904, 0, 0, 0);
	load_data(din_addr[2], 0x6000, 1976, 0, 0, 0);
	load_data(din_addr[3], 0x4000, 1976, 0, 0, 0);
	config(0x0, 31, 0, 0);
	execute(0x1f, 0, 0);
	store(dout_addr[0], 0x8000, 1976, 0, 0);
}


/* Array initialization. */
void init_array()
{
	int j, cur_indx, i;
	for (i = 0; i < N; i++){
        vec[i] = i;
        cur_indx = 0;
        for(j=0; j < L; j++){
            cur_indx = (TYPE)(((double) i / N) * ((L-1) - cur_indx) + cur_indx);
//            printf("idx %d \n",cur_indx);
            if(cur_indx < L){
		        nzval[i*L + j] = i;
                cols[i*L +j] = cur_indx;
            }
        }
	}
	for (i=0; i<N; i++){
        res[i] = i;
        out[i] = i;
    }
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void ellpack()
{
    int i, j;
    TYPE Si;

    ellpack_1 : for (i=0; i<N; i++) {
        TYPE sum = 0;
        ellpack_2 : for (j=0; j<L; j++) {
//        please_map_me();
                Si = nzval[j + i*L] * vec[cols[j + i*L]];
                sum += Si;
        }
        res[i] += sum;
    }
}

void result_check()
{
  int i, j;

  for (i = 0; i < N; i++)
  {
    if (res[i] != out[i])
        printf("There is an error in location (%d)[%d, %d]\n", i, res[i], out[i]);
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
  ellpack();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[4] = {cols, nzval, out, vec};
  void* cgra_dout_addr[1] = {out};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}