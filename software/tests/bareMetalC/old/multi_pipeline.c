#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include benchmark-specific header. */
#include "include/encoding.h"
#include "include/ISA.h"

// ################################ array_add ##########################################
#define array_add_SIZE  1024
int array_add_A[array_add_SIZE] __attribute__((aligned(8)));
int array_add_B[array_add_SIZE] __attribute__((aligned(8)));
int array_add_C[array_add_SIZE] __attribute__((aligned(8)));
int array_add_D[array_add_SIZE] __attribute__((aligned(8)));

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void array_add(){   
   for (int i=0;i<array_add_SIZE; i++){
      #ifdef CGRA_COMPILER
      please_map_me();
      #endif
      array_add_D[i] = array_add_A[i] + array_add_B[i];
   }
}

void array_add_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[15][3] __attribute__((aligned(8))) = {
		{0x1801, 0x0001, 0x0251},
		{0x0110, 0x0000, 0x0298},
		{0x0000, 0x0000, 0x02a0},
		{0x2000, 0x0000, 0x02d8},
		{0x0040, 0x0000, 0x02d9},
		{0x0000, 0x0000, 0x02da},
		{0x0010, 0x0000, 0x02db},
		{0x3000, 0x0000, 0x02e0},
		{0x0040, 0x0000, 0x02e1},
		{0x0000, 0x0000, 0x02e2},
		{0x0410, 0x0001, 0x02e3},
		{0x2000, 0x0000, 0x02e8},
		{0x0040, 0x0000, 0x02e9},
		{0x0000, 0x0000, 0x02ea},
		{0x0010, 0x0000, 0x02eb},
	};

	load_cfg((void*)cin, 0x40000, 90, 0, 0);
	load_data(din_addr[0], 0x28000, 4096, 0, 0);
	load_data(din_addr[1], 0x20000, 4096, 0, 0);
	config(0x0, 15, 0, 0);
	execute(0x700, 0, 0);
	store(dout_addr[0], 0x24000, 4096, 0, 0);
}

// ################################ conv2 ##########################################
#define conv2_N 1024
int conv2_a[conv2_N];
int conv2_b[conv2_N];
int conv2_c[conv2_N];

__attribute__((noinline))
void conv2(){
  int i;
  for (i = 0; i < conv2_N-1; i++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif
    conv2_b[i] = conv2_a[i] * 10 + conv2_a[i + 1] * 20; //+ conv2_a[i+ 2] * 3;
  }
}


void conv2_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[19][3] __attribute__((aligned(8))) = {
    		{0x2000, 0xf000, 0x0008},
    		{0x003f, 0x0000, 0x0009},
    		{0x0000, 0x0000, 0x000a},
    		{0x0010, 0x0000, 0x000b},
    		{0x3001, 0xf000, 0x0010},
    		{0x003f, 0x0000, 0x0011},
    		{0x0000, 0x0000, 0x0012},
    		{0x0010, 0x0000, 0x0013},
    		{0x2000, 0xf000, 0x0018},
    		{0x003f, 0x0000, 0x0019},
    		{0x0000, 0x0000, 0x001a},
    		{0x0510, 0x0001, 0x001b},
    		{0x1010, 0x0000, 0x0060},
    		{0x000a, 0x0000, 0x0098},
    		{0x0803, 0x0000, 0x0099},
    		{0xd001, 0x0000, 0x00a1},
    		{0x0014, 0x0000, 0x00a8},
    		{0x0803, 0x0000, 0x00a9},
    		{0x0000, 0x0000, 0x00e8},
    	};

    	load_cfg((void*)cin, 0x40060, 114, 1, 0);
    	load_data(din_addr[0], 0x0, 4092, 1, 0);
    	load_data(din_addr[1], 0x4000, 4096, 1, 0);
    	config(0xc, 19, 1, 0);
    	execute(0x7, 1, 0);
    	store(dout_addr[0], 0x8000, 4092, 1, 0);
}


// ################################ conv3 ##########################################
#define conv3_N 1024
int conv3_a[conv3_N];
int conv3_b[conv3_N];
int conv3_c[conv3_N];

__attribute__((noinline))
void conv3(){
    int i;
    for (i = 0; i < conv3_N-2; i++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif
        conv3_b[i] = conv3_a[i] * 10 + conv3_a[i + 1] * 20 + conv3_a[i+ 2] * 3;
    }
}

void conv3_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[32][3] __attribute__((aligned(8))) = {
		{0x2001, 0xe000, 0x0028},
		{0x003f, 0x0000, 0x0029},
		{0x0000, 0x0000, 0x002a},
		{0x0010, 0x0000, 0x002b},
		{0x0000, 0x0000, 0x0078},
		{0x0014, 0x0000, 0x00c0},
		{0x0803, 0x0000, 0x00c1},
		{0x3000, 0x0000, 0x0100},
		{0x0000, 0x0000, 0x0108},
		{0xd001, 0x0000, 0x0141},
		{0x01c0, 0x0000, 0x0188},
		{0x0901, 0x0001, 0x01d1},
		{0x0000, 0x0208, 0x0218},
		{0x0004, 0x0000, 0x0220},
		{0x000a, 0x0000, 0x0258},
		{0x1803, 0x0000, 0x0259},
		{0x0003, 0x0000, 0x0260},
		{0x1803, 0x0000, 0x0261},
		{0x0010, 0x0000, 0x02a0},
		{0x0310, 0x0000, 0x02a8},
		{0x2000, 0xe000, 0x02e0},
		{0x003f, 0x0000, 0x02e1},
		{0x0000, 0x0000, 0x02e2},
		{0x0010, 0x0000, 0x02e3},
		{0x2000, 0xe000, 0x02e8},
		{0x003f, 0x0000, 0x02e9},
		{0x0000, 0x0000, 0x02ea},
		{0x0010, 0x0000, 0x02eb},
		{0x3000, 0xe000, 0x02f0},
		{0x003f, 0x0000, 0x02f1},
		{0x0000, 0x0000, 0x02f2},
		{0x0610, 0x0001, 0x02f3},
	};

	load_cfg((void*)cin, 0x400d8, 192, 2, 0);
	load_data(din_addr[0], 0x20000, 4088, 2, 0);
	load_data(din_addr[1], 0x10000, 4092, 2, 0);
	load_data(din_addr[2], 0x28000, 4088, 2, 0);
	config(0x1b, 32, 2, 0);
	execute(0xe10, 2, 0);
	store(dout_addr[0], 0x2c000, 4088, 2, 0);
}

// ################################ conv2d_2x2 ##########################################
#define NI 32
#define NJ 32
#define SIZE NI*NJ

int conv2d_2x2_A[SIZE];
int conv2d_2x2_B[SIZE];
int conv2d_2x2_C[SIZE];
#define c11 2
#define c12 -3
#define c21 5
#define c22 6

__attribute__((noinline))
void conv2d_2x2() {
	for (int i = 1; i < NI -1; i++) {
		for (int j = 1; j < NJ-1; j++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif
	/*For 2x2 filter*/
		conv2d_2x2_C[i*NJ + j] = c11 * conv2d_2x2_A[(i - 1)*NJ + (j - 1)]  +  c12 * conv2d_2x2_A[(i + 0)*NJ + (j - 1)]
				+ c21 * conv2d_2x2_A[(i - 1)*NJ + (j + 0)]  +  c22 * conv2d_2x2_A[(i + 0)*NJ + (j + 0)];
		}
	}
}

void conv2d_2x2_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[39][3] __attribute__((aligned(8))) = {
		{0x2001, 0xe000, 0x0008},
		{0x0c01, 0x3c00, 0x0009},
		{0x0000, 0x0000, 0x000a},
		{0x0010, 0x0000, 0x000b},
		{0x3001, 0xe000, 0x0010},
		{0x0c01, 0x3c00, 0x0011},
		{0x0000, 0x0000, 0x0012},
		{0x0710, 0x0101, 0x0013},
		{0x3000, 0xe000, 0x0020},
		{0x0c01, 0x3c00, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0010, 0x0000, 0x0023},
		{0x2400, 0xe000, 0x0028},
		{0x0c01, 0x3c00, 0x0029},
		{0x0000, 0x0000, 0x002a},
		{0x0010, 0x0000, 0x002b},
		{0x3001, 0xe000, 0x0030},
		{0x0c01, 0x3c00, 0x0031},
		{0x0000, 0x0000, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x0000, 0x0000, 0x0058},
		{0x0000, 0x0004, 0x0060},
		{0x9800, 0x0000, 0x0068},
		{0x0000, 0x0002, 0x0070},
		{0x0000, 0x0002, 0x0078},
		{0x0000, 0x0001, 0x0080},
		{0x0005, 0x0000, 0x00a0},
		{0x0803, 0x0000, 0x00a1},
		{0x9101, 0x0000, 0x00a9},
		{0x6101, 0x0000, 0x00b1},
		{0x0001, 0x0000, 0x00b8},
		{0x0807, 0x0000, 0x00b9},
		{0xfffd, 0xffff, 0x00c0},
		{0x0803, 0x0000, 0x00c1},
		{0x0006, 0x0000, 0x00c8},
		{0x0803, 0x0000, 0x00c9},
		{0x0002, 0x0000, 0x0100},
		{0x0100, 0x0001, 0x0108},
		{0x4801, 0x0000, 0x0151},
	};

	load_cfg((void*)cin, 0x40198, 234, 3, 0);
	load_data(din_addr[0], 0xc000, 3832, 3, 0);
	load_data(din_addr[1], 0x11000, 3832, 3, 0);
	load_data(din_addr[2], 0x0, 3836, 3, 0);
	load_data(din_addr[3], 0x14000, 3836, 3, 0);
	config(0x33, 39, 3, 0);
	execute(0x3b, 3, 0);
	store(dout_addr[0], 0x4000, 3836, 3, 0);
}

// ################################ conv2d_3x3 ##########################################
int conv2d_3x3_A[SIZE];
int conv2d_3x3_B[SIZE];
int conv2d_3x3_C[SIZE];

/*For 3x3 filter*/
#define c13 4
#define c23 7
#define c31 -8
#define c32 -9
#define c33 10

__attribute__((noinline))
void conv2d_3x3() {
	for (int i = 1; i < NI -1; i++) {
		for (int j = 1; j < NJ-1; j++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif
	/*For 3x3 filter*/
		conv2d_3x3_C[i*NJ + j] = c11 * conv2d_3x3_A[(i - 1)*NJ + (j - 1)]  +  c12 * conv2d_3x3_A[(i + 0)*NJ + (j - 1)]  +  c13 * conv2d_3x3_A[(i + 1)*NJ + (j - 1)]
				+ c21 * conv2d_3x3_A[(i - 1)*NJ + (j + 0)]  +  c22 * conv2d_3x3_A[(i + 0)*NJ + (j + 0)]  +  c23 * conv2d_3x3_A[(i + 1)*NJ + (j + 0)]
				+ c31 * conv2d_3x3_A[(i - 1)*NJ + (j + 1)]  +  c32 * conv2d_3x3_A[(i + 0)*NJ + (j + 1)]  +  c33 * conv2d_3x3_A[(i + 1)*NJ + (j + 1)];
		}
	}
}

void conv2d_3x3_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[88][3] __attribute__((aligned(8))) = {
		{0x23c0, 0xe000, 0x0010},
		{0x0c01, 0x3c00, 0x0011},
		{0x0000, 0x0000, 0x0012},
		{0x0010, 0x0000, 0x0013},
		{0x2001, 0xe000, 0x0020},
		{0x0c01, 0x3c00, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0010, 0x0000, 0x0023},
		{0x27be, 0xe000, 0x0030},
		{0x0c01, 0x3c00, 0x0031},
		{0x0000, 0x0000, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x2000, 0xe000, 0x0038},
		{0x0c01, 0x3c00, 0x0039},
		{0x0000, 0x0000, 0x003a},
		{0x0010, 0x0000, 0x003b},
		{0x0000, 0x0000, 0x0060},
		{0x0000, 0x0000, 0x0070},
		{0x0000, 0x0000, 0x0080},
		{0x0000, 0x0000, 0x0088},
		{0x0001, 0x0000, 0x00a8},
		{0x0807, 0x0000, 0x00a9},
		{0x0006, 0x0000, 0x00b8},
		{0x0803, 0x0000, 0x00b9},
		{0x0002, 0x0000, 0x00c8},
		{0x0807, 0x0000, 0x00c9},
		{0xfffd, 0xffff, 0x00d0},
		{0x0803, 0x0000, 0x00d1},
		{0x0000, 0x0000, 0x00f8},
		{0x0000, 0x0080, 0x0100},
		{0x0000, 0x0080, 0x0108},
		{0x1800, 0x0000, 0x0110},
		{0x0000, 0x0000, 0x0118},
		{0xa401, 0x0000, 0x0141},
		{0x9201, 0x0000, 0x0151},
		{0x5021, 0x0000, 0x0159},
		{0x0000, 0x0000, 0x0190},
		{0x0200, 0x0000, 0x0198},
		{0xd301, 0x0000, 0x01d9},
		{0x0c01, 0x0001, 0x01e1},
		{0x1c01, 0x0001, 0x01e9},
		{0xdc01, 0x0000, 0x01f1},
		{0x0000, 0x0040, 0x0218},
		{0x0840, 0x0000, 0x0220},
		{0x3000, 0x0000, 0x0228},
		{0x0000, 0x0043, 0x0230},
		{0x0080, 0x0000, 0x0238},
		{0x000a, 0x0000, 0x0258},
		{0x1883, 0x0000, 0x0259},
		{0x0005, 0x0000, 0x0260},
		{0x1803, 0x0000, 0x0261},
		{0x5401, 0x0000, 0x0269},
		{0xfff7, 0xffff, 0x0270},
		{0x1843, 0x0000, 0x0271},
		{0x0007, 0x0000, 0x0278},
		{0x1823, 0x0000, 0x0279},
		{0xfff8, 0xffff, 0x0280},
		{0x1843, 0x0000, 0x0281},
		{0x0010, 0x0000, 0x02a0},
		{0x0010, 0x0000, 0x02a8},
		{0x0000, 0x0000, 0x02b0},
		{0x0010, 0x0000, 0x02b8},
		{0x0010, 0x0000, 0x02c0},
		{0x0010, 0x0000, 0x02c8},
		{0x2000, 0xe000, 0x02e0},
		{0x0c01, 0x3c00, 0x02e1},
		{0x0000, 0x0000, 0x02e2},
		{0x0110, 0x0000, 0x02e3},
		{0x2001, 0xe000, 0x02e8},
		{0x0c01, 0x3c00, 0x02e9},
		{0x0000, 0x0000, 0x02ea},
		{0x0010, 0x0000, 0x02eb},
		{0x3001, 0xe000, 0x02f0},
		{0x0c01, 0x3c00, 0x02f1},
		{0x0000, 0x0000, 0x02f2},
		{0x0e10, 0x0101, 0x02f3},
		{0x3000, 0xe000, 0x02f8},
		{0x0c01, 0x3c00, 0x02f9},
		{0x0000, 0x0000, 0x02fa},
		{0x0010, 0x0000, 0x02fb},
		{0x2001, 0xe000, 0x0300},
		{0x0c01, 0x3c00, 0x0301},
		{0x0000, 0x0000, 0x0302},
		{0x0010, 0x0000, 0x0303},
		{0x2000, 0xe000, 0x0308},
		{0x0c01, 0x3c00, 0x0309},
		{0x0000, 0x0000, 0x030a},
		{0x0010, 0x0000, 0x030b},
	};

	load_cfg((void*)cin, 0x40000, 528, 4, 2);
	load_data(din_addr[0], 0xf00, 3832, 4, 0);
	load_data(din_addr[1], 0x18000, 3832, 4, 0);
	load_data(din_addr[2], 0x11ef8, 3832, 4, 0);
	load_data(din_addr[3], 0x28000, 3836, 4, 0);
	load_data(din_addr[4], 0x8000, 3836, 4, 0);
	load_data(din_addr[5], 0x30000, 3836, 4, 0);
	load_data(din_addr[6], 0x38000, 3832, 4, 0);
	load_data(din_addr[7], 0x34000, 3832, 4, 0);
	load_data(din_addr[8], 0x20000, 3832, 4, 0);
	config(0x0, 88, 4, 0);
	execute(0x7e6a, 4, 0);
	store(dout_addr[0], 0x2c000, 3836, 4, 0);
}


// ################################ gemver ##########################################
#define DATA_TYPE int
#define gemver_N 20
//#define _PB_N 20

//int n = gemver_N;
//DATA_TYPE alpha;
//DATA_TYPE beta;

DATA_TYPE gemver_A[gemver_N][gemver_N];
DATA_TYPE gemver_O[gemver_N][gemver_N];
DATA_TYPE u1[gemver_N];
DATA_TYPE v1[gemver_N];
DATA_TYPE u2[gemver_N];
DATA_TYPE v2[gemver_N];

__attribute__((noinline))
void kernel_gemver()
{
  int i, j;

  for (i = 0; i < gemver_N; i++)
  {
    for (j = 0; j < gemver_N; j++)
    {
      gemver_O[i][j] += u1[i] * v1[j] + u2[i] * v2[j];
    }
  }
}

void gemver_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[35][3] __attribute__((aligned(8))) = {
		{0x3000, 0x0000, 0x0018},
		{0x0402, 0x4000, 0x0019},
		{0x0000, 0x0000, 0x001a},
		{0x0610, 0x0101, 0x001b},
		{0x2780, 0x0000, 0x0020},
		{0x0402, 0x4000, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0010, 0x0000, 0x0023},
		{0x33be, 0x0000, 0x0028},
		{0x8402, 0x41ff, 0x0029},
		{0x0000, 0x0000, 0x002a},
		{0x0010, 0x0000, 0x002b},
		{0x03be, 0x0000, 0x0030},
		{0x0402, 0x4000, 0x0031},
		{0x0000, 0x0000, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x3000, 0x0000, 0x0038},
		{0x8402, 0x41ff, 0x0039},
		{0x0000, 0x0000, 0x003a},
		{0x0010, 0x0000, 0x003b},
		{0x03c0, 0x0000, 0x0040},
		{0x0402, 0x4000, 0x0041},
		{0x0000, 0x0000, 0x0042},
		{0x0010, 0x0000, 0x0043},
		{0x0002, 0x0000, 0x0068},
		{0x0000, 0x0001, 0x0070},
		{0x0400, 0x0000, 0x0078},
		{0x0100, 0x0000, 0x0080},
		{0x0000, 0x0000, 0x0088},
		{0x6201, 0x0000, 0x00b9},
		{0x4803, 0x0000, 0x00c1},
		{0x8803, 0x0000, 0x00c9},
		{0x0100, 0x0000, 0x0108},
		{0x0000, 0x0000, 0x0110},
		{0x8801, 0x0000, 0x0151},
	};

	load_cfg((void*)cin, 0x40200, 210, 5, 0);
	load_data(din_addr[0], 0x18f00, 128, 5, 0);
	load_data(din_addr[1], 0x1c000, 128, 5, 0);
	load_data(din_addr[2], 0x10ef8, 128, 5, 0);
	load_data(din_addr[3], 0x14ef8, 128, 5, 0);
	load_data(din_addr[4], 0x9e00, 4096, 5, 0);
	config(0x40, 35, 5, 0);
	execute(0xfc, 5, 0);
	store(dout_addr[0], 0xc000, 4096, 5, 0);
}

// ################################ mvt ##########################################
#define mvt_N 32

int mvt_A[mvt_N][mvt_N];
int x1[mvt_N];
int x2[mvt_N];
int y_1[mvt_N];

__attribute__((noinline))
void kernel_mvt()
{
  int i, j ,s;

  for (i = 0; i < mvt_N; i++)
  {
    s = 0;
    for (j = 0; j < mvt_N; j++)
    {
      s += mvt_A[i][j] * y_1[j];
    }
    x2[i] += s;
  }
}

void mvt_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[25][3] __attribute__((aligned(8))) = {
		{0x0b80, 0x0000, 0x0020},
		{0x0402, 0x4000, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0010, 0x0000, 0x0023},
		{0x1000, 0x0000, 0x0028},
		{0x0402, 0x4000, 0x0029},
		{0x0000, 0x0000, 0x002a},
		{0x0610, 0x0101, 0x002b},
		{0x23de, 0x0000, 0x0030},
		{0x8402, 0x41ff, 0x0031},
		{0x0000, 0x0000, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x23e0, 0x0000, 0x0038},
		{0x0402, 0x4000, 0x0039},
		{0x0000, 0x0000, 0x003a},
		{0x0010, 0x0000, 0x003b},
		{0x0000, 0x0000, 0x0070},
		{0x4001, 0x0000, 0x0078},
		{0x1400, 0x0000, 0x0080},
		{0x8841, 0x0000, 0x00b9},
		{0x100f, 0x0000, 0x00c1},
		{0x0000, 0x0002, 0x00c2},
		{0x1002, 0x0400, 0x00c3},
		{0x0000, 0x0000, 0x00c4},
		{0x4803, 0x0000, 0x00c9},
	};

	load_cfg((void*)cin, 0x402d8, 150, 6, 0);
	load_data(din_addr[0], 0x18f80, 4096, 6, 0);
	load_data(din_addr[1], 0x10f78, 128, 6, 0);
	load_data(din_addr[2], 0xae00, 128, 6, 0);
	config(0x5b, 25, 6, 0);
	execute(0x78, 6, 0);
	store(dout_addr[0], 0x14000, 128, 6, 0);
}

// ################################ stencil3d ##########################################
#define height_size 8
#define col_size 8
#define row_size 16
//Data Bounds
#define TYPE int
//#define MAX 1000
//#define MIN 1
//Convenience Macros
#define stencil3d_SIZE (row_size * col_size * height_size)
#define INDX(_row_size,_col_size,_i,_j,_k) ((_i)+_row_size*((_j)+_col_size*(_k)))

TYPE stencil3d_C[2];
TYPE orig[stencil3d_SIZE];
TYPE sol[stencil3d_SIZE];
TYPE res[stencil3d_SIZE];

__attribute__((noinline))
void stencil3d (){
    int i,j,k;
	int sum0, sum1, mul0, mul1;

	for(i = 1; i < height_size - 1; i++){
        for(j = 1; j < col_size - 1; j++){
            for(k = 1; k < row_size - 1; k++){

                sum0 = orig[INDX(row_size, col_size, k, j, i)];
                sum1 = orig[INDX(row_size, col_size, k, j, i + 1)] +
                       orig[INDX(row_size, col_size, k, j, i - 1)] +
                       orig[INDX(row_size, col_size, k, j + 1, i)] +
                       orig[INDX(row_size, col_size, k, j - 1, i)] +
                       orig[INDX(row_size, col_size, k + 1, j, i)] +
                       orig[INDX(row_size, col_size, k - 1, j, i)];
                mul0 = sum0 * stencil3d_C[0];
                mul1 = sum1 * stencil3d_C[1];
                sol[INDX(row_size, col_size, k, j, i)] = mul0 + mul1;
            }
        }
    }
}

void stencil3d_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[59][3] __attribute__((aligned(8))) = {
		{0x3021, 0xe000, 0x0028},
		{0x0c00, 0x0c00, 0x0029},
		{0x1180, 0x0180, 0x002a},
		{0x0a10, 0x0101, 0x002b},
		{0x03ff, 0xe000, 0x0030},
		{0x0000, 0x0c00, 0x0031},
		{0x0000, 0x0180, 0x0032},
		{0x0110, 0x0000, 0x0033},
		{0x1000, 0xe000, 0x0038},
		{0x0000, 0x0c00, 0x0039},
		{0x0000, 0x0180, 0x003a},
		{0x0010, 0x0000, 0x003b},
		{0x27e1, 0xe000, 0x0040},
		{0x0c00, 0x0c00, 0x0041},
		{0x1180, 0x0180, 0x0042},
		{0x0010, 0x0000, 0x0043},
		{0x0000, 0x0000, 0x0078},
		{0x8000, 0x0000, 0x0080},
		{0x0400, 0x0001, 0x0088},
		{0xa401, 0x0000, 0x00c1},
		{0x4803, 0x0000, 0x00d1},
		{0x0300, 0x0000, 0x0110},
		{0x0883, 0x0001, 0x0159},
		{0x000c, 0x0000, 0x01a8},
		{0x0000, 0x0000, 0x0238},
		{0x1801, 0x0001, 0x0261},
		{0xd901, 0x0000, 0x0269},
		{0x1a01, 0x0001, 0x0271},
		{0xdb01, 0x0000, 0x0279},
		{0x1c01, 0x0001, 0x0281},
		{0x0010, 0x0000, 0x02a8},
		{0x0040, 0x0000, 0x02b0},
		{0x0000, 0x0000, 0x02b8},
		{0x0040, 0x0000, 0x02c0},
		{0x0000, 0x0000, 0x02c8},
		{0x2001, 0xe000, 0x02e8},
		{0x0c00, 0x0c00, 0x02e9},
		{0x1180, 0x0180, 0x02ea},
		{0x0010, 0x0000, 0x02eb},
		{0x3001, 0xe000, 0x02f0},
		{0x0c00, 0x0c00, 0x02f1},
		{0x1180, 0x0180, 0x02f2},
		{0x0010, 0x0000, 0x02f3},
		{0x2001, 0xe000, 0x02f8},
		{0x0c00, 0x0c00, 0x02f9},
		{0x1180, 0x0180, 0x02fa},
		{0x0010, 0x0000, 0x02fb},
		{0x3000, 0xe000, 0x0300},
		{0x0c00, 0x0c00, 0x0301},
		{0x1180, 0x0180, 0x0302},
		{0x0010, 0x0000, 0x0303},
		{0x2001, 0xe000, 0x0308},
		{0x0c00, 0x0c00, 0x0309},
		{0x1180, 0x0180, 0x030a},
		{0x0010, 0x0000, 0x030b},
		{0x3000, 0xe000, 0x0310},
		{0x0c00, 0x0c00, 0x0311},
		{0x1180, 0x0180, 0x0312},
		{0x0010, 0x0000, 0x0313},
	};

	load_cfg((void*)cin, 0x40000, 354, 7, 0);
	load_data(din_addr[0], 0x19f80, 2940, 7, 0);
	load_data(din_addr[1], 0x30000, 2940, 7, 0);
	load_data(din_addr[2], 0x28000, 2940, 7, 0);
	load_data(din_addr[3], 0x2c000, 2940, 7, 0);
	load_data(din_addr[4], 0x38000, 2940, 7, 0);
	load_data(din_addr[5], 0x34000, 2936, 7, 0);
	load_data(din_addr[6], 0x3c000, 2936, 7, 0);
	load_data(din_addr[7], 0x1c000, 4, 7, 0);
	load_data(din_addr[8], 0x10ff8, 8, 7, 0);
	config(0x0, 59, 7, 0);
	execute(0xfcf0, 7, 0);
	store(dout_addr[0], 0x14080, 2940, 7, 0);
}

// ################################ fir ##########################################
#define NTAPS 32

int fir_input[NTAPS];
int fir_output[NTAPS];
int fir_res[NTAPS];
int coefficients[NTAPS] = {025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125};

__attribute__((noinline))
void fir()//(int fir_input[], int fir_output[], int coefficients[])
/*   fir_input :           fir_input sample array */
/*   fir_output:           fir_output sample array */
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
      sum += fir_input[i] * coefficients[i];
    }
    fir_res[j] += sum;
 }
}

void fir_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[25][3] __attribute__((aligned(8))) = {
		{0x0000, 0x0000, 0x0018},
		{0x0402, 0x4000, 0x0019},
		{0x0000, 0x0000, 0x001a},
		{0x0010, 0x0000, 0x001b},
		{0x1000, 0x0000, 0x0020},
		{0x0402, 0x4000, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0610, 0x0101, 0x0023},
		{0x3000, 0x0000, 0x0028},
		{0x8402, 0x41ff, 0x0029},
		{0x0000, 0x0000, 0x002a},
		{0x0010, 0x0000, 0x002b},
		{0x2bc0, 0x0000, 0x0030},
		{0x8402, 0x41ff, 0x0031},
		{0x0000, 0x0000, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x0000, 0x0000, 0x0068},
		{0x4001, 0x0000, 0x0070},
		{0x1400, 0x0000, 0x0078},
		{0x8841, 0x0000, 0x00b1},
		{0x100f, 0x0000, 0x00b9},
		{0x0000, 0x0002, 0x00ba},
		{0x1002, 0x0400, 0x00bb},
		{0x0000, 0x0000, 0x00bc},
		{0x4803, 0x0000, 0x00c1},
	};

	load_cfg((void*)cin, 0x40168, 150, 8, 0);
	load_data(din_addr[0], 0x12f00, 128, 8, 0);
	load_data(din_addr[1], 0x14000, 128, 8, 0);
	load_data(din_addr[2], 0x8000, 128, 8, 0);
	config(0x2d, 25, 8, 0);
	execute(0x3c, 8, 0);
	store(dout_addr[0], 0xc000, 128, 8, 0);
}


// ################################ fir-unroll4 ##########################################
int fir_unroll4_input[NTAPS];
int fir_unroll4_output[NTAPS];
int fir_unroll4_res[NTAPS];

__attribute__((noinline))
void fir_unroll4()//(int fir_unroll4_input[], int fir_unroll4_output[], int coefficients[])
/*   fir_unroll4_input :           fir_unroll4_input sample array */
/*   fir_unroll4_output:           fir_unroll4_output sample array */
/*   coefficient:      coefficient array */
{
  int i;
  int j = 0;

 for(j=0; j< NTAPS; ++j) {
 	  int sum = 0;
    #pragma unroll 4
    for (i = 0; i < NTAPS; ++i) {
      #ifdef CGRA_COMPILER
      please_map_me();
      #endif
      sum += fir_unroll4_input[i] * coefficients[i];
    }
    res[j] += sum;
 }
}

void fir_unroll4_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[65][3] __attribute__((aligned(8))) = {
		{0x9000, 0x8000, 0x0008},
		{0x9000, 0x41ff, 0x0009},
		{0x0000, 0x0000, 0x000a},
		{0x0010, 0x0000, 0x000b},
		{0x8000, 0x8000, 0x0010},
		{0x9000, 0x41ff, 0x0011},
		{0x0000, 0x0000, 0x0012},
		{0x0010, 0x0000, 0x0013},
		{0x8020, 0x8000, 0x0020},
		{0x9000, 0x41ff, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0010, 0x0000, 0x0023},
		{0x8be0, 0x8000, 0x0028},
		{0x9000, 0x41ff, 0x0029},
		{0x0000, 0x0000, 0x002a},
		{0x0010, 0x0000, 0x002b},
		{0x0400, 0x0000, 0x0058},
		{0x0400, 0x0000, 0x0070},
		{0x4803, 0x0000, 0x00a1},
		{0x4803, 0x0000, 0x00b9},
		{0x0000, 0x0000, 0x00f0},
		{0x4000, 0x0000, 0x00f8},
		{0x0000, 0x0000, 0x0100},
		{0x8821, 0x0000, 0x0139},
		{0x9801, 0x0000, 0x0141},
		{0x0030, 0x0000, 0x0188},
		{0x6041, 0x0000, 0x01d1},
		{0x180f, 0x0000, 0x01d9},
		{0x0000, 0x8002, 0x01da},
		{0x0403, 0x0400, 0x01db},
		{0x0000, 0x0000, 0x01dc},
		{0x0000, 0x0008, 0x0218},
		{0x0000, 0x0000, 0x0220},
		{0x1803, 0x0001, 0x0259},
		{0xa081, 0x0000, 0x0261},
		{0x1803, 0x0001, 0x0269},
		{0x3000, 0x0000, 0x0298},
		{0x0010, 0x0003, 0x02a0},
		{0x0004, 0x0000, 0x02a8},
		{0x0010, 0x0000, 0x02b0},
		{0x0000, 0x0000, 0x02b8},
		{0x1000, 0x8000, 0x02d8},
		{0x0400, 0x4000, 0x02d9},
		{0x0000, 0x0000, 0x02da},
		{0x0a10, 0x0101, 0x02db},
		{0x8001, 0x8000, 0x02e0},
		{0x9000, 0x41ff, 0x02e1},
		{0x0000, 0x0000, 0x02e2},
		{0x0010, 0x0000, 0x02e3},
		{0x8001, 0x8000, 0x02e8},
		{0x9000, 0x41ff, 0x02e9},
		{0x0000, 0x0000, 0x02ea},
		{0x0010, 0x0000, 0x02eb},
		{0x9001, 0x8000, 0x02f0},
		{0x9000, 0x41ff, 0x02f1},
		{0x0000, 0x0000, 0x02f2},
		{0x0010, 0x0000, 0x02f3},
		{0x1000, 0x8000, 0x02f8},
		{0x0400, 0x4000, 0x02f9},
		{0x0000, 0x0000, 0x02fa},
		{0x0110, 0x0000, 0x02fb},
		{0x8001, 0x8000, 0x0300},
		{0x9000, 0x41ff, 0x0301},
		{0x0000, 0x0000, 0x0302},
		{0x0010, 0x0000, 0x0303},
	};

	load_cfg((void*)cin, 0x40200, 390, 9, 0);
	load_data(din_addr[0], 0x12f80, 116, 9, 0);
	load_data(din_addr[1], 0x8080, 116, 9, 0);
	load_data(din_addr[2], 0x28000, 120, 9, 0);
	load_data(din_addr[3], 0x20000, 120, 9, 0);
	load_data(din_addr[4], 0x0, 116, 9, 0);
	load_data(din_addr[5], 0x4000, 116, 9, 0);
	load_data(din_addr[6], 0x30000, 120, 9, 0);
	load_data(din_addr[7], 0x2c000, 120, 9, 0);
	load_data(din_addr[8], 0x34000, 128, 9, 0);
	config(0x40, 65, 9, 0);
	execute(0x3f1b, 9, 0);
	store(dout_addr[0], 0x24000, 128, 9, 0);
}

///* Array initialization. */
//void init_array()
//{
//int i,j;
//for (i=0;i<array_add_SIZE; i++){
//      gemver_A[i] = i * 2 + 5;
//      conv2d_2x2_B[i] = i * 3;
//    }
//
//}
//
//void result_check()
//{
//  int i, j;
//
//  for (i = 0; i < array_add_SIZE; i++)
//  {
//    if (conv2d_2x2_C[i] != D[i]) printf("There is an error in location (%d)[%d, %d]\n", i, conv2d_2x2_C[i], D[i]);
//  }
//}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  volatile int result;
//  init_array();
//  printf("Initialization finished!\n");

  start = rdcycle();
  /* Run kernels. */
  array_add();
  conv2();
  conv3();
  conv2d_2x2();
  conv2d_3x3();
  kernel_gemver();
  kernel_mvt();
  stencil3d();
  fir();
  fir_unroll4();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  start = rdcycle();
  void* array_add_cgra_din_addr[2] = {array_add_A, array_add_B};
  void* array_add_cgra_dout_addr[1] = {array_add_C};
  array_add_cgra_execute(array_add_cgra_din_addr, array_add_cgra_dout_addr);
  result = fence(1);

  void* conv2_cgra_din_addr[2] = {conv2_a, conv2_a};
  void* conv2_cgra_dout_addr[1] = {conv2_c};
  conv2_cgra_execute(conv2_cgra_din_addr, conv2_cgra_dout_addr);
  result = fence(1);

  void* conv3_cgra_din_addr[3] = {conv3_a, conv3_a, (void*)conv3_a+8};
  void* conv3_cgra_dout_addr[1] = {conv3_c};
  conv3_cgra_execute(conv3_cgra_din_addr, conv3_cgra_dout_addr);
  result = fence(1);

  void* conv2d_2x2_cgra_din_addr[4] = {conv2d_2x2_A, (void*)conv2d_2x2_A+128, conv2d_2x2_A, (void*)conv2d_2x2_A+128};
  void* conv2d_2x2_cgra_dout_addr[1] = {(void*)conv2d_2x2_B+128};
  conv2d_2x2_cgra_execute(conv2d_2x2_cgra_din_addr, conv2d_2x2_cgra_dout_addr);
  result = fence(1);

  void* conv2d_3x3_cgra_din_addr[9] = {conv2d_3x3_A, (void*)conv2d_3x3_A+128, (void*)conv2d_3x3_A+256, conv2d_3x3_A, (void*)conv2d_3x3_A+128, (void*)conv2d_3x3_A+256, (void*)conv2d_3x3_A+8, (void*)conv2d_3x3_A+136, (void*)conv2d_3x3_A+264};
  void* conv2d_3x3_cgra_dout_addr[1] = {(void*)conv2d_3x3_B+128};
  conv2d_3x3_cgra_execute(conv2d_3x3_cgra_din_addr, conv2d_3x3_cgra_dout_addr);
  result = fence(1);

  void* gemver_cgra_din_addr[5] = {u1, v1, u2, v2, gemver_A};
  void* gemver_cgra_dout_addr[1] = {gemver_A};
  gemver_cgra_execute(gemver_cgra_din_addr, gemver_cgra_dout_addr);
  result = fence(1);

  void* mvt_cgra_din_addr[9] = {mvt_A, y_1, x1};
  void* mvt_cgra_dout_addr[1] = {x1};
  mvt_cgra_execute(mvt_cgra_din_addr, mvt_cgra_dout_addr);
  result = fence(1);

  void* stencil3d_cgra_din_addr[9] = {(void*)orig+576, (void*)orig+1088, (void*)orig+64, (void*)orig+640, (void*)orig+512, (void*)orig+584, (void*)orig+576, stencil3d_C, stencil3d_C};
  void* stencil3d_cgra_dout_addr[1] = {(void*)res+576};
  stencil3d_cgra_execute(stencil3d_cgra_din_addr, stencil3d_cgra_dout_addr);
  result = fence(1);

  void* fir_cgra_din_addr[3] = {fir_input, coefficients, fir_output};
  void* fir_cgra_dout_addr[1] = {fir_output};
  fir_cgra_execute(fir_cgra_din_addr, fir_cgra_dout_addr);
  result = fence(1);

  void* fir_unroll4_cgra_din_addr[9] = {fir_unroll4_input, coefficients, fir_unroll4_input, coefficients, (void*)fir_unroll4_input+8, (void*)coefficients+8, (void*)fir_unroll4_input+8, (void*)coefficients+8, fir_unroll4_output};
  void* fir_unroll4_cgra_dout_addr[1] = {fir_unroll4_output};
  fir_unroll4_cgra_execute(fir_unroll4_cgra_din_addr, fir_unroll4_cgra_dout_addr);
  result = fence(1);

  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

//  result_check();
  printf("Done!\n");

  return 0;
}