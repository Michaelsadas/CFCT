#include <stdio.h>
#include "include/ISA.h"
#include "include/encoding.h"
#define IH 40
#define IW 40
#define OH 38
#define OW 38
#define KH 3
#define KW 3
#define SH 1
#define SW 1
#define OC 3
#define IC 3

int A[IH][IW] __attribute__((aligned(8)));
int B[OH][OW] __attribute__((aligned(8)));
int R[OH][OW] __attribute__((aligned(8)));
int K[KH][KW] __attribute__((aligned(8)));

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[19][3] __attribute__((aligned(8))) = {
		{0x3003, 0x0002, 0x0251},
		{0x3011, 0x0000, 0x0259},
		{0x0000, 0x0004, 0x025a},
		{0x0091, 0x05a4, 0x025b},
		{0x0010, 0x0000, 0x0298},
		{0x0000, 0x0000, 0x02a0},
		{0x2800, 0x0c00, 0x02d8},
		{0x0040, 0x0018, 0x02d9},
		{0x6ffc, 0xf802, 0x02da},
		{0x04df, 0x0002, 0x02db},
		{0x1000, 0x0c00, 0x02e0},
		{0x0000, 0x8018, 0x02e1},
		{0x6000, 0x0102, 0x02e2},
		{0x04c0, 0x0116, 0x02e3},
		{0x0001, 0x0000, 0x02e4},
		{0x2000, 0x0c00, 0x02e8},
		{0x0980, 0x8018, 0x02e9},
		{0x6fd7, 0xb102, 0x02ea},
		{0x04df, 0x0002, 0x02eb},
	};

	load_cfg((void*)cin, 0x20000, 114, 1);
	load_data(din_addr[0], 0x10000, 6400, 0, 1);
	load_data(din_addr[1], 0x12000, 36, 0, 1);
	config(0x0, 19, 1);
	execute(0x700, 1);
	store(dout_addr[0], 0x14000, 5776, 1);
}

void work()
{
    for (int oc = 0; oc < OC; oc ++) {
        for (int ic = 0; ic < IC; ic ++) {
            for (int h = 0; h < OH; h = h + SH ) {
                for (int w = 0; w < OW; w = w + SW) {
                    for (int kh = 0; kh < KH; kh ++) {
                        for (int kw = 0; kw < KW; kw ++) {
                            //#ifdef CGRA_COMPILER
                            //please_map_me();
                           R[h][w] += A[h + kh][w + kw] * K[kh][kw];
	        	            //#endif
                        }
                    }
                }
            }
        }
    }
}

void init(){
    for (int h = 0; h < IH; h = h + 1 ) {
        for (int w = 0; w < IW; w = w + 1 ) {
            A[h][w] = (h + w) % 10;
        }
    }

    for (int h = 0; h < KH; h = h + 1){
        for (int w = 0; w < KW; w = w + 1){
            K[h][w] = h + w;
        }
    }
}

void check(){
    for (int h = 0; h < OH; h = h + SH ) {
        for (int w = 0; w < OW; w = w + SW) {
            if(R[h][w] != B[h][w]) printf("Wrong in location (%d, %d): True: %d, Wrong: %d\n", h, w, R[h][w], B[h][w]);
        }
    }
}

int main(){
//    init();
    long long unsigned start, end;
//    start = rdcycle();
//    work();
//    end = rdcycle();
//    printf("It takes %d cycles for CPU to finish the task.\n", end - start);
    start = rdcycle();
    void* cgra_din_addr[2] = {A, K};
    void* cgra_dout_addr[1] = {B};
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    volatile int result = fence(1);
    end = rdcycle();
    printf("It takes %d cycles for CGRA to finish the task(%d)\n", end - start, result);
    check();
    printf("done!(%d)\n", B[12][11]);
}