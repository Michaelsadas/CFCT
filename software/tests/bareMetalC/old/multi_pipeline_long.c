#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include benchmark-specific header. */
#include "include/encoding.h"
#include "include/ISA.h"

//volatile int result;
// ################################ pedometer ##########################################
#define pedometer_SIZE  1024
#define pedometer_SIZE  1024

int acc_vector[pedometer_SIZE ];
int res_vector[pedometer_SIZE ];
int acceleration_x[pedometer_SIZE ];
int acceleration_y[pedometer_SIZE ];
int acceleration_z[pedometer_SIZE ];
int acc_avg_x[pedometer_SIZE ];
int acc_avg_y[pedometer_SIZE ];
int acc_avg_z[pedometer_SIZE];

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void pedometer(){   
	for(int i=0;i<pedometer_SIZE;i++){
		acc_vector[i] = (acceleration_x[i]- acc_avg_x[i]) * (acceleration_x[i]- acc_avg_x[i])+  (acceleration_y[i]- acc_avg_y[i]) * (acceleration_y[i]- acc_avg_y[i])+ (acceleration_z[i]-acc_avg_z[i]) * (acceleration_z[i]-acc_avg_z[i]) ;
	}
}

void pedometer_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[45][3] __attribute__((aligned(8))) = {
		{0x2000, 0x0000, 0x0020},
		{0x0040, 0x0000, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0010, 0x0000, 0x0023},
		{0x2000, 0x0000, 0x0028},
		{0x0040, 0x0000, 0x0029},
		{0x0000, 0x0000, 0x002a},
		{0x0010, 0x0000, 0x002b},
		{0x5400, 0x0000, 0x0070},
		{0x9003, 0x0000, 0x00b1},
		{0x4802, 0x0000, 0x00b9},
		{0x0000, 0x0200, 0x00f8},
		{0x0c00, 0x0000, 0x0188},
		{0x2003, 0x0001, 0x01c1},
		{0x2003, 0x0001, 0x01c9},
		{0x5901, 0x0000, 0x01d1},
		{0x0105, 0x0000, 0x0210},
		{0x0010, 0x0200, 0x0218},
		{0x1802, 0x0001, 0x0251},
		{0x4801, 0x0000, 0x0259},
		{0x1802, 0x0001, 0x0261},
		{0x0010, 0x0000, 0x0298},
		{0x0004, 0x0000, 0x02a0},
		{0x0310, 0x0000, 0x02a8},
		{0x0000, 0x0000, 0x02b0},
		{0x2000, 0x0000, 0x02d8},
		{0x0040, 0x0000, 0x02d9},
		{0x0000, 0x0000, 0x02da},
		{0x0010, 0x0000, 0x02db},
		{0x3000, 0x0000, 0x02e0},
		{0x0040, 0x0000, 0x02e1},
		{0x0000, 0x0000, 0x02e2},
		{0x0010, 0x0000, 0x02e3},
		{0x2000, 0x0000, 0x02e8},
		{0x0040, 0x0000, 0x02e9},
		{0x0000, 0x0000, 0x02ea},
		{0x0010, 0x0000, 0x02eb},
		{0x3000, 0x0000, 0x02f0},
		{0x0040, 0x0000, 0x02f1},
		{0x0000, 0x0000, 0x02f2},
		{0x0710, 0x0001, 0x02f3},
		{0x2000, 0x0000, 0x02f8},
		{0x0040, 0x0000, 0x02f9},
		{0x0000, 0x0000, 0x02fa},
		{0x0010, 0x0000, 0x02fb},
	};

	load_cfg((void*)cin, 0x40000, 270, 0, 0);
	load_data(din_addr[0], 0x20000, 4096, 0, 0);
	load_data(din_addr[1], 0x24000, 4096, 0, 0);
	load_data(din_addr[2], 0x28000, 4096, 0, 0);
	load_data(din_addr[3], 0x30000, 4096, 0, 0);
	load_data(din_addr[4], 0x8000, 4096, 0, 0);
	load_data(din_addr[5], 0x10000, 4096, 0, 0);
	config(0x0, 45, 0, 0);
	execute(0x1f18, 0, 0);
	store(dout_addr[0], 0x2c000, 4096, 0, 0);
}

// ################################ stencil2d ##########################################
#define TYPE int
#define col_size 32
#define row_size 32
#define f_size 9

TYPE orig[row_size * col_size] __attribute__((aligned(8)));
TYPE sol[row_size * col_size] __attribute__((aligned(8)));
TYPE filter[f_size] __attribute__((aligned(8)));
TYPE res[row_size * col_size] __attribute__((aligned(8)));

__attribute__((noinline))
void stencil2d(){
    int r, c, k1, k2;
    TYPE temp, mul;

    for (r=0; r<row_size-2; r++) {
        for (c=0; c<col_size-2; c++) {
            temp = (TYPE)0;
            for (k1=0;k1<3;k1++){
                #pragma unroll
                for (k2=0;k2<3;k2++){
                    mul = filter[k1*3 + k2] * orig[(r+k1)*col_size + c+k2];
                    temp += mul;
                }
            }
            sol[(r*col_size) + c] = temp;
        }
    }
}


void stencil2d_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[45][3] __attribute__((aligned(8))) = {
		{0x1001, 0x3004, 0x0008},
		{0x0400, 0x3dff, 0x0009},
		{0xe180, 0x07bf, 0x000a},
		{0x0010, 0x0000, 0x000b},
		{0x6001, 0x3000, 0x0010},
		{0xe800, 0x3dff, 0x0011},
		{0xfd00, 0x07bf, 0x0012},
		{0x0010, 0x0000, 0x0013},
		{0x0000, 0x3000, 0x0018},
		{0x0400, 0x3c00, 0x0019},
		{0x0180, 0x0780, 0x001a},
		{0x0710, 0x0101, 0x001b},
		{0x1000, 0x3004, 0x0028},
		{0x0400, 0x3dff, 0x0029},
		{0xe180, 0x07bf, 0x002a},
		{0x0010, 0x0000, 0x002b},
		{0x6400, 0x3000, 0x0030},
		{0xe800, 0x3dff, 0x0031},
		{0xfd00, 0x07bf, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x1000, 0x3004, 0x0038},
		{0x0400, 0x3dff, 0x0039},
		{0xe180, 0x07bf, 0x003a},
		{0x0010, 0x0000, 0x003b},
		{0x6000, 0x3000, 0x0040},
		{0xe800, 0x3dff, 0x0041},
		{0xfd00, 0x07bf, 0x0042},
		{0x0010, 0x0000, 0x0043},
		{0x0400, 0x0000, 0x0058},
		{0x0000, 0x0004, 0x0060},
		{0x0000, 0x0008, 0x0068},
		{0x1200, 0x0000, 0x0070},
		{0x8400, 0x0000, 0x0078},
		{0x0000, 0x0001, 0x0080},
		{0x4400, 0x0000, 0x0088},
		{0x4803, 0x0000, 0x00a1},
		{0x100f, 0x0000, 0x00b1},
		{0x0000, 0x0002, 0x00b2},
		{0x0183, 0x7080, 0x00b3},
		{0x0000, 0x0000, 0x00b4},
		{0x8801, 0x0000, 0x00b9},
		{0x4803, 0x0000, 0x00c1},
		{0x9801, 0x0000, 0x00c9},
		{0x4803, 0x0000, 0x00d1},
		{0x0000, 0x0000, 0x0110},
	};

	load_cfg((void*)cin, 0x40110, 270, 1, 0);
	load_data(din_addr[0], 0x11000, 28, 1, 0);
	load_data(din_addr[1], 0x14000, 4088, 1, 0);
	load_data(din_addr[2], 0x0, 32, 1, 0);
	load_data(din_addr[3], 0x4000, 4092, 1, 0);
	load_data(din_addr[4], 0x18000, 28, 1, 0);
	load_data(din_addr[5], 0x1c000, 4088, 1, 0);
	config(0x22, 45, 1, 0);
	execute(0xf7, 1, 0);
	store(dout_addr[0], 0x8000, 3832, 1, 0);
}


// ################################ gemm ##########################################
//#define row_size 32
//#define col_size 32
#define N (row_size*col_size)

TYPE gemm_m1[N] __attribute__((aligned(8)));
TYPE gemm_m2[N] __attribute__((aligned(8)));
TYPE gemm_prod0[N] __attribute__((aligned(8)));
TYPE gemm_prod[N] __attribute__((aligned(8)));


__attribute__((noinline))
void gemm()
{
    int i, j, k;
    int k_col, i_col;
    TYPE mult;

    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<col_size;j++) {
            i_col = i * col_size;
            TYPE sum = 0;
            inner:for(k=0;k<row_size;k++) {
                k_col = k * col_size;
                mult = gemm_m1[i_col + k] * gemm_m2[k_col + j];
                sum += mult;
            }
            gemm_prod0[i_col + j]  = sum;
        }
    }
}

void gemm_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[19][3] __attribute__((aligned(8))) = {
		{0x0000, 0x0000, 0x0030},
		{0x0402, 0x4000, 0x0031},
		{0x0080, 0x0800, 0x0032},
		{0x0510, 0x0101, 0x0033},
		{0x13fe, 0x0004, 0x0038},
		{0x8402, 0x41f0, 0x0039},
		{0x0080, 0x083e, 0x003a},
		{0x0010, 0x0000, 0x003b},
		{0x2008, 0x0000, 0x0040},
		{0x8402, 0x41ff, 0x0041},
		{0x0080, 0x0800, 0x0042},
		{0x0010, 0x0000, 0x0043},
		{0x0000, 0x0000, 0x0080},
		{0x1400, 0x0000, 0x0088},
		{0x100f, 0x0000, 0x00c9},
		{0x0000, 0x0002, 0x00ca},
		{0x1002, 0x8000, 0x00cb},
		{0x0000, 0x0000, 0x00cc},
		{0x4803, 0x0000, 0x00d1},
	};

	load_cfg((void*)cin, 0x40220, 114, 2, 0);
	load_data(din_addr[0], 0x18020, 4096, 2, 0);
	load_data(din_addr[1], 0x1cff8, 4096, 2, 0);
	config(0x44, 19, 2, 0);
	execute(0xe0, 2, 0);
	store(dout_addr[0], 0x10000, 4096, 2, 0);
}


// ################################ gemm-unroll4 ##########################################
TYPE gemm_unroll4_m1[N] __attribute__((aligned(8)));
TYPE gemm_unroll4_m2[N] __attribute__((aligned(8)));
TYPE gemm_unroll4_prod0[N] __attribute__((aligned(8)));
TYPE gemm_unroll4_prod[N] __attribute__((aligned(8)));

__attribute__((noinline))
void gemm_unroll4()
{
    int i, j, k;
    int k_col, i_col;
    TYPE mult;

    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<col_size;j++) {
            i_col = i * col_size;
            TYPE sum = 0;
            #pragma unroll 4
            for(k=0;k<row_size;k++) {
                k_col = k * col_size;
                mult = gemm_unroll4_m1[i_col + k] * gemm_unroll4_m2[k_col + j];
                sum += mult;
            }
            gemm_unroll4_prod0[i_col + j]  = sum;
        }
    }
}

void gemm_unroll4_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[58][3] __attribute__((aligned(8))) = {
		{0x8000, 0x8000, 0x0008},
		{0x9000, 0x41ff, 0x0009},
		{0x0200, 0x0800, 0x000a},
		{0x0010, 0x0000, 0x000b},
		{0x1000, 0x8010, 0x0010},
		{0x0400, 0x41f2, 0x0011},
		{0x3080, 0x083e, 0x0012},
		{0x0010, 0x0000, 0x0013},
		{0x0000, 0x8010, 0x0018},
		{0x0400, 0x41f2, 0x0019},
		{0x3080, 0x083e, 0x001a},
		{0x0010, 0x0000, 0x001b},
		{0x9001, 0x8000, 0x0020},
		{0x9000, 0x41ff, 0x0021},
		{0x0200, 0x0800, 0x0022},
		{0x0010, 0x0000, 0x0023},
		{0x0000, 0x0000, 0x0058},
		{0x0800, 0x0000, 0x0060},
		{0x0400, 0x0000, 0x0068},
		{0x4823, 0x0000, 0x00a9},
		{0x4803, 0x0000, 0x00b1},
		{0x0100, 0x0000, 0x00f8},
		{0x4821, 0x0000, 0x0141},
		{0x4000, 0x0000, 0x0190},
		{0x9861, 0x0000, 0x01d1},
		{0x6041, 0x0000, 0x01d9},
		{0x0110, 0x0000, 0x0218},
		{0x0000, 0x0000, 0x0228},
		{0x1803, 0x0001, 0x0259},
		{0x080f, 0x0000, 0x0261},
		{0x0000, 0x0002, 0x0262},
		{0x0404, 0x8000, 0x0263},
		{0x0000, 0x0000, 0x0264},
		{0x1803, 0x0001, 0x0271},
		{0x0010, 0x0000, 0x02a0},
		{0x0000, 0x0000, 0x02a8},
		{0x0010, 0x0000, 0x02b8},
		{0x0000, 0x0000, 0x02c0},
		{0x0000, 0x8010, 0x02e0},
		{0x0400, 0x41f2, 0x02e1},
		{0x3080, 0x083e, 0x02e2},
		{0x0010, 0x0000, 0x02e3},
		{0x1000, 0x8000, 0x02e8},
		{0x0400, 0x4000, 0x02e9},
		{0x0080, 0x0800, 0x02ea},
		{0x0910, 0x0101, 0x02eb},
		{0x8001, 0x8000, 0x02f0},
		{0x9000, 0x41ff, 0x02f1},
		{0x0200, 0x0800, 0x02f2},
		{0x0010, 0x0000, 0x02f3},
		{0x0000, 0x8010, 0x02f8},
		{0x0400, 0x41f2, 0x02f9},
		{0x3080, 0x083e, 0x02fa},
		{0x0010, 0x0000, 0x02fb},
		{0x8000, 0x8000, 0x0308},
		{0x9000, 0x41ff, 0x0309},
		{0x0200, 0x0800, 0x030a},
		{0x0010, 0x0000, 0x030b},
	};

	load_cfg((void*)cin, 0x40298, 348, 3, 0);
	load_data(din_addr[0], 0x0, 4084, 3, 0);
	load_data(din_addr[1], 0x4000, 3712, 3, 0);
	load_data(din_addr[2], 0xc000, 4088, 3, 0);
	load_data(din_addr[3], 0x38000, 4084, 3, 0);
	load_data(din_addr[4], 0x30000, 3712, 3, 0);
	load_data(din_addr[5], 0x28000, 4088, 3, 0);
	load_data(din_addr[6], 0x20000, 3712, 3, 0);
	load_data(din_addr[7], 0x8000, 3712, 3, 3);
	config(0x53, 58, 3, 0);
	execute(0x5e0f, 3, 0);
	store(dout_addr[0], 0x2c000, 4096, 3, 0);
//	result = fence(1);
}



///* Array initialization. */
//void init_array()
//{
//int i,j;
//for (i=0;i<pedometer_SIZE; i++){
//      gemver_A[i] = i * 2 + 5;
//      stencil2dd_2x2_B[i] = i * 3;
//    }
//
//}
//
//void result_check()
//{
//  int i, j;
//
//  for (i = 0; i < pedometer_SIZE; i++)
//  {
//    if (stencil2dd_2x2_C[i] != D[i]) printf("There is an error in location (%d)[%d, %d]\n", i, stencil2dd_2x2_C[i], D[i]);
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
  pedometer();
  stencil2d();
  gemm_unroll4();
//  gemm();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  start = rdcycle();
  void* pedometer_cgra_din_addr[6] = {acceleration_x, acc_avg_x, acceleration_y, acc_avg_y, acceleration_z, acc_avg_z};
  void* pedometer_cgra_dout_addr[1] = {res_vector};
  pedometer_cgra_execute(pedometer_cgra_din_addr, pedometer_cgra_dout_addr);
//  result = fence(1);

  void* stencil2d_cgra_din_addr[6] = {filter, orig, filter, orig, (void*)filter+8, (void*)orig+8};
  void* stencil2d_cgra_dout_addr[1] = {res};
  stencil2d_cgra_execute(stencil2d_cgra_din_addr, stencil2d_cgra_dout_addr);
//  result = fence(1);

  void* gemm_unroll4_cgra_din_addr[8] = {gemm_unroll4_m1, gemm_unroll4_m2, gemm_unroll4_m1, (void*)gemm_unroll4_m2+128, (void*)gemm_unroll4_m1+8, (void*)gemm_unroll4_m2+256, (void*)gemm_unroll4_m1+8, (void*)gemm_unroll4_m2+384};
  void* gemm_unroll4_cgra_dout_addr[1] = {gemm_unroll4_prod};
  gemm_unroll4_cgra_execute(gemm_unroll4_cgra_din_addr, gemm_unroll4_cgra_dout_addr);
  result = fence(1);

//  void* gemm_cgra_din_addr[2] = {gemm_m1, gemm_m2};
//  void* gemm_cgra_dout_addr[1] = {gemm_prod};
//  gemm_cgra_execute(gemm_cgra_din_addr, gemm_cgra_dout_addr);
//  result = fence(1);

//  void* gemm_unroll4_cgra_din_addr[8] = {gemm_unroll4_m1, gemm_unroll4_m2, gemm_unroll4_m1, (void*)gemm_unroll4_m2+128, (void*)gemm_unroll4_m1+8, (void*)gemm_unroll4_m2+256, (void*)gemm_unroll4_m1+8, (void*)gemm_unroll4_m2+384};
//  void* gemm_unroll4_cgra_dout_addr[1] = {gemm_unroll4_prod};
//  gemm_unroll4_cgra_execute(gemm_unroll4_cgra_din_addr, gemm_unroll4_cgra_dout_addr);
//  result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

//  result_check();
  printf("Done!\n");

  return 0;
}