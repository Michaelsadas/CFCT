//#include "convolution2d.h"#define
//typedef int DATA_TYPE;

/* Problem size */
#define NI 32
#define NJ 32
#define SIZE NI*NJ


int A[SIZE];
int B[SIZE];
int C[SIZE];
#define c11 2
#define c12 -3
#define c21 5
#define c22 6

/*For 3x3 filter*/
//#define c13 4
//#define c23 7
//#define c31 -8
//#define c32 -9
//#define c33 10

#include "include/encoding.h"
#include "include/ISA.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[37][3] __attribute__((aligned(8))) = {
		{0x2000, 0x7800, 0x0004},
		{0x00c0, 0x00f0, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x2820, 0x7800, 0x0008},
		{0x00c0, 0x00f0, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x3001, 0x7800, 0x0010},
		{0x00c0, 0x00f0, 0x0011},
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x2801, 0x7800, 0x0014},
		{0x00c0, 0x00f0, 0x0015},
		{0x0000, 0x8f00, 0x0016},
		{0x0080, 0x0000, 0x0017},
		{0x2021, 0x7800, 0x0018},
		{0x00c0, 0x00f0, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x1000, 0x0004, 0x0030},
		{0x0200, 0x0000, 0x0034},
		{0x4000, 0x0004, 0x0038},
		{0x0200, 0x0000, 0x003c},
		{0x4000, 0x0000, 0x0040},
		{0x0001, 0x0000, 0x004c},
		{0x0405, 0x0000, 0x004d},
		{0x6801, 0x0000, 0x0051},
		{0xfffd, 0xffff, 0x0054},
		{0x0403, 0x0000, 0x0055},
		{0x4481, 0x0000, 0x0059},
		{0x0005, 0x0000, 0x005c},
		{0x0403, 0x0000, 0x005d},
		{0x4501, 0x0000, 0x0061},
		{0x0006, 0x0000, 0x0064},
		{0x0403, 0x0000, 0x0065},
		{0x0000, 0x0000, 0x0074},
	};

	load_cfg((void*)cin, 0x20000, 222, 0, 0);
	load_data(din_addr[0], 0x0, 3964, 1, 0, 0);
	load_data(din_addr[1], 0x2000, 3964, 1, 0, 0);
	load_data(din_addr[2], 0x4000, 3964, 1, 0, 0);
	load_data(din_addr[3], 0x8000, 3964, 0, 0, 0);
	config(0x0, 37, 0, 0);
	execute(0x3b, 0, 0);
	store(dout_addr[0], 0xa000, 3836, 0, 0);
}


/* Array initialization. */
void init_array()
{
    int i,j;
    for (i=0;i<SIZE; i++){
      A[i] = i;
    }

}


__attribute__((noinline))
void convolution2d() {
	for (int i = 1; i < NI -1; i++) {
		for (int j = 1; j < NJ-1; j++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif
	/*For 3x3 filter*/
	//	B[i*NJ + j] = c11 * A[(i - 1)*NJ + (j - 1)]  +  c12 * A[(i + 0)*NJ + (j - 1)]  +  c13 * A[(i + 1)*NJ + (j - 1)]
	//			+ c21 * A[(i - 1)*NJ + (j + 0)]  +  c22 * A[(i + 0)*NJ + (j + 0)]  +  c23 * A[(i + 1)*NJ + (j + 0)]
	//			+ c31 * A[(i - 1)*NJ + (j + 1)]  +  c32 * A[(i + 0)*NJ + (j + 1)]  +  c33 * A[(i + 1)*NJ + (j + 1)];

	/*For 2x2 filter*/
		C[i*NJ + j] = c11 * A[(i - 1)*NJ + (j - 1)]  +  c12 * A[(i + 0)*NJ + (j - 1)]
				+ c21 * A[(i - 1)*NJ + (j + 0)]  +  c22 * A[(i + 0)*NJ + (j + 0)];
		}
	}
}

void result_check()
{
  int i, j;

  for (i = 0; i < SIZE; i++)
  {
    if (C[i] != B[i]) printf("There is an error in location (%d)[%d, %d]\n", i, C[i], B[i]);
  }
}

int main(){
    long long unsigned start;
    long long unsigned end;
    init_array();
    printf("Initialization finished!\n");

    start = rdcycle();
    /* Run kernel. */
    convolution2d();
    end = rdcycle();
    printf("It takes %d cycles for CPU to finish the task.\n", end - start);

    void* cgra_din_addr[4] = {A, A, A, A};
    void* cgra_dout_addr[1] = {(void*)B+128};
    start = rdcycle();
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    volatile int result = fence(1);
    end = rdcycle();
    printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

    result_check();
    printf("Done!\n");

    return 0;
}
