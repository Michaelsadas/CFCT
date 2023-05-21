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

#define height_size 8
#define col_size 8
#define row_size 16
//Data Bounds
#define TYPE float
#define MAX 1000
#define MIN 1
//Convenience Macros
#define SIZE (row_size * col_size * height_size)
#define INDX(_row_size,_col_size,_i,_j,_k) ((_i)+_row_size*((_j)+_col_size*(_k)))

TYPE C[2]; 
TYPE orig[SIZE]; 
TYPE sol[SIZE];
TYPE res[SIZE];

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[59][3] __attribute__((aligned(8))) = {
		{0x0800, 0x3800, 0x0018},
		{0x0000, 0x0030, 0x0019},
		{0x6000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x2081, 0x3800, 0x001c},
		{0x00c0, 0x8030, 0x001d},
		{0x6011, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x3001, 0x3800, 0x0020},
		{0x00c0, 0x8030, 0x0021},
		{0x6011, 0x9500, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0400, 0x0000, 0x0040},
		{0x0030, 0x0000, 0x0044},
		{0x2493, 0x0000, 0x0065},
		{0x0000, 0x0000, 0x008c},
		{0x2e01, 0x0000, 0x00b1},
		{0x0030, 0x0000, 0x00d4},
		{0x0000, 0x0008, 0x011c},
		{0x8c01, 0x0000, 0x0129},
		{0x6c81, 0x0000, 0x012d},
		{0x8d01, 0x0000, 0x0131},
		{0x6d81, 0x0000, 0x0135},
		{0x8e01, 0x0000, 0x0139},
		{0x6c43, 0x0000, 0x013d},
		{0x0010, 0x0000, 0x014c},
		{0x0040, 0x0000, 0x0150},
		{0x0000, 0x0000, 0x0154},
		{0x0040, 0x0000, 0x0158},
		{0x0000, 0x0000, 0x015c},
		{0x0010, 0x0000, 0x0160},
		{0x2801, 0x3800, 0x016c},
		{0x00c0, 0x8030, 0x016d},
		{0x6011, 0x0100, 0x016e},
		{0x0000, 0x0000, 0x016f},
		{0x3091, 0x3800, 0x0170},
		{0x00c0, 0x8030, 0x0171},
		{0x6011, 0x0100, 0x0172},
		{0x0000, 0x0000, 0x0173},
		{0x2101, 0x3800, 0x0174},
		{0x00c0, 0x8030, 0x0175},
		{0x6011, 0x0100, 0x0176},
		{0x0000, 0x0000, 0x0177},
		{0x3882, 0x3800, 0x0178},
		{0x00c0, 0x8030, 0x0179},
		{0x6011, 0x0100, 0x017a},
		{0x0000, 0x0000, 0x017b},
		{0x2071, 0x3800, 0x017c},
		{0x00c0, 0x8030, 0x017d},
		{0x6011, 0x0100, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x1001, 0x3800, 0x0180},
		{0x0000, 0x0030, 0x0181},
		{0x6000, 0x0300, 0x0182},
		{0x0000, 0x0000, 0x0183},
		{0x2880, 0x3800, 0x0184},
		{0x00c0, 0x8030, 0x0185},
		{0x6011, 0x0100, 0x0186},
		{0x0000, 0x0000, 0x0187},
	};

	load_cfg((void*)cin, 0x20000, 354, 0, 0);
	load_data(din_addr[0], 0xa000, 8, 0, 0, 0);
	load_data(din_addr[1], 0x1c000, 8, 0, 0, 0);
	load_data(din_addr[2], 0x8000, 3964, 0, 0, 0);
	load_data(din_addr[3], 0x10000, 3964, 0, 0, 0);
	load_data(din_addr[4], 0x12000, 3964, 0, 0, 0);
	load_data(din_addr[5], 0x14000, 3964, 0, 0, 0);
	load_data(din_addr[6], 0x18000, 3964, 0, 0, 0);
	load_data(din_addr[7], 0x16000, 3964, 0, 0, 0);
	load_data(din_addr[8], 0x1a000, 3964, 0, 0, 0);
	config(0x0, 59, 0, 0);
	execute(0x7fe0, 0, 0);
	store(dout_addr[0], 0xc000, 2940, 0, 0);
}


/* Array initialization. */
static
void init_array()
{
//	int i, j, k;
//    for(j=0; j<col_size; j++) {
//        for(k=0; k<row_size; k++) {
//            sol[INDX(row_size, col_size, k, j, 0)] = orig[INDX(row_size, col_size, k, j, 0)];
//            sol[INDX(row_size, col_size, k, j, height_size-1)] = orig[INDX(row_size, col_size, k, j, height_size-1)];
//        }
//    }
//    for(i=1; i<height_size-1; i++) {
//        for(k=0; k<row_size; k++) {
//            sol[INDX(row_size, col_size, k, 0, i)] = orig[INDX(row_size, col_size, k, 0, i)];
//            sol[INDX(row_size, col_size, k, col_size-1, i)] = orig[INDX(row_size, col_size, k, col_size-1, i)];
//        }
//    }
//    for(i=1; i<height_size-1; i++) {
//        for(j=1; j<col_size-1; j++) {
//            sol[INDX(row_size, col_size, 0, j, i)] = orig[INDX(row_size, col_size, 0, j, i)];
//            sol[INDX(row_size, col_size, row_size-1, j, i)] = orig[INDX(row_size, col_size, row_size-1, j, i)];
//        }
//    }
    for(int i = 0; i < SIZE; i++){
        orig[i] = i;
    }
	C[0] = 1;
	C[1] = 2;
}

__attribute__((noinline))
void stencil3d (){
    int i,j,k;
	TYPE sum0, sum1, mul0, mul1;

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
                mul0 = sum0 * C[0];
                mul1 = sum1 * C[1];
                sol[INDX(row_size, col_size, k, j, i)] = mul0 + mul1;
            }
        }
    }
}

TYPE absolute_value(TYPE a)
{
	if (a >= 0) return a;
	else return -a;
}

static
void result_check()
{
  	int i, j, k, r, c;
	for(i = 1; i < height_size - 1; i++){
        for(j = 1; j < col_size - 1; j++){
            for(k = 1; k < row_size - 1; k++){
            int idx = INDX(row_size, col_size, k, j, i);
    		if (absolute_value(res[idx] - sol[idx]) >= 0.1) printf("There is an error in location (%d)[%d, %d]\n", idx, (int)res[idx], (int)sol[idx]);
  			}
		}
	}
}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  init_array();
  printf("Initialization finished!\n");

  start = rdcycle();
  stencil3d();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[9] = {C, C, (void*)orig+64, (void*)orig+64, (void*)orig+64, (void*)orig+64, (void*)orig+64, (void*)orig+64, (void*)orig+64};
  void* cgra_dout_addr[1] = {(void*)res+576};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}