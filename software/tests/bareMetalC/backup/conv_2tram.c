#include <stdio.h>
#include "include/Tram.h"
#include "include/encoding.h"
#define IH 78
#define IW 40
#define OH 76
#define OW 38
#define KH 3
#define KW 3
#define SH 1
#define SW 1
#define OC 3
#define IC 3

int A[IC][IH][IW] __attribute__((aligned(8)));
int B[OC][OH][OW] __attribute__((aligned(8)));
int R[OC][OH][OW] __attribute__((aligned(8)));
int W[OC][IC][KH + 1][KW] __attribute__((aligned(8)));

void cgra_execute(void** din_addr, void** dout_addr, int target_tram)
{
	volatile unsigned short cin[114][3] __attribute__((aligned(8))) = {
		{0x2800, 0x0c00, 0x0008},
		{0x0040, 0x0018, 0x0009},
		{0x6ffc, 0xf802, 0x000a},
		{0x04df, 0x0002, 0x000b},
		{0x2000, 0x0c00, 0x0010},
		{0x0980, 0x8018, 0x0011},
		{0x6fd7, 0xb102, 0x0012},
		{0x04df, 0x0002, 0x0013},
		{0x3000, 0x0c00, 0x0018},
		{0x0040, 0x0018, 0x0019},
		{0x6ffc, 0xf802, 0x001a},
		{0x04df, 0x0002, 0x001b},
		{0x3800, 0x0c00, 0x0020},
		{0x0040, 0x0018, 0x0021},
		{0x6ffc, 0xf802, 0x0022},
		{0x04df, 0x0002, 0x0023},
		{0x2800, 0x0c00, 0x0028},
		{0x0040, 0x0018, 0x0029},
		{0x6ffc, 0xf802, 0x002a},
		{0x04df, 0x0002, 0x002b},
		{0x2000, 0x0c00, 0x0030},
		{0x0040, 0x0018, 0x0031},
		{0x6ffc, 0xf802, 0x0032},
		{0x04df, 0x0002, 0x0033},
		{0x1000, 0x0c00, 0x0038},
		{0x0000, 0x8018, 0x0039},
		{0x6000, 0x0102, 0x003a},
		{0x04c0, 0x0122, 0x003b},
		{0x0001, 0x0000, 0x003c},
		{0x0000, 0x0000, 0x0058},
		{0x0100, 0x0000, 0x0060},
		{0x0800, 0x0010, 0x0068},
		{0x0000, 0x0000, 0x0070},
		{0x0000, 0x0010, 0x0078},
		{0x0001, 0x0000, 0x0088},
		{0x1003, 0x0001, 0x0099},
		{0x9003, 0x0000, 0x00a9},
		{0xa003, 0x0000, 0x00b1},
		{0xb201, 0x0001, 0x00b9},
		{0x3001, 0x0002, 0x00c1},
		{0x3011, 0x0000, 0x00c9},
		{0x0000, 0xc004, 0x00ca},
		{0x0091, 0x05a4, 0x00cb},
		{0x0000, 0x0000, 0x00e8},
		{0x0300, 0x0000, 0x00f8},
		{0x0810, 0x0000, 0x0100},
		{0x0300, 0x0000, 0x0108},
		{0x0004, 0x0000, 0x0110},
		{0x1003, 0x0002, 0x0141},
		{0xc001, 0x0000, 0x0149},
		{0x9043, 0x0001, 0x0151},
		{0x0c00, 0x0000, 0x0178},
		{0x0000, 0x0100, 0x0188},
		{0x010c, 0x0080, 0x0190},
		{0x0083, 0x0000, 0x0198},
		{0xc001, 0x0000, 0x01c1},
		{0x3201, 0x0002, 0x01c9},
		{0x1201, 0x0002, 0x01d9},
		{0x0002, 0x0000, 0x0210},
		{0x0004, 0x0021, 0x0218},
		{0x0000, 0x0020, 0x0220},
		{0x0004, 0x0000, 0x0228},
		{0x0200, 0x0000, 0x0230},
		{0x2011, 0x0000, 0x0251},
		{0x0000, 0x8004, 0x0252},
		{0x0091, 0x05a4, 0x0253},
		{0x3003, 0x0002, 0x0259},
		{0x3003, 0x0002, 0x0261},
		{0xc003, 0x0001, 0x0269},
		{0xc003, 0x0001, 0x0271},
		{0x1011, 0x0000, 0x0279},
		{0x0000, 0x8004, 0x027a},
		{0x0091, 0x05a4, 0x027b},
		{0x0000, 0x0000, 0x0298},
		{0x0010, 0x0000, 0x02a0},
		{0x0010, 0x0000, 0x02a8},
		{0x0040, 0x0000, 0x02b0},
		{0x0040, 0x0000, 0x02b8},
		{0x0000, 0x0000, 0x02c0},
		{0x0000, 0x0000, 0x02c8},
		{0x1800, 0x0c00, 0x02d8},
		{0x0000, 0x8018, 0x02d9},
		{0x6000, 0x0102, 0x02da},
		{0x04c0, 0x011e, 0x02db},
		{0x0001, 0x0000, 0x02dc},
		{0x3000, 0x0c00, 0x02e0},
		{0x0040, 0x0018, 0x02e1},
		{0x6ffc, 0xf802, 0x02e2},
		{0x04df, 0x0002, 0x02e3},
		{0x2000, 0x0c00, 0x02e8},
		{0x0040, 0x0018, 0x02e9},
		{0x6ffc, 0xf802, 0x02ea},
		{0x04df, 0x0002, 0x02eb},
		{0x2800, 0x0c00, 0x02f0},
		{0x0980, 0x8018, 0x02f1},
		{0x6fd7, 0xb102, 0x02f2},
		{0x04df, 0x0002, 0x02f3},
		{0x2000, 0x0c00, 0x02f8},
		{0x0980, 0x8018, 0x02f9},
		{0x6fd7, 0xb102, 0x02fa},
		{0x04df, 0x0002, 0x02fb},
		{0x3000, 0x0c00, 0x0300},
		{0x0040, 0x0018, 0x0301},
		{0x6ffc, 0xf802, 0x0302},
		{0x04df, 0x0002, 0x0303},
		{0x2800, 0x0c00, 0x0308},
		{0x0040, 0x0018, 0x0309},
		{0x6ffc, 0xf802, 0x030a},
		{0x04df, 0x0002, 0x030b},
		{0x1800, 0x0c00, 0x0310},
		{0x0000, 0x8018, 0x0311},
		{0x6000, 0x0102, 0x0312},
		{0x04c0, 0x011e, 0x0313},
		{0x0000, 0x0000, 0x0314},
	};

	load_cfg((void*)cin, 0x20000, 684, target_tram);

	//load_set(40, 160, 312, target_tram);
	load_data(din_addr[0], 0x18000, 6400, 0, target_tram);
	load_data(din_addr[1], 0x12000, 6400, 0, target_tram);
	load_data(din_addr[2], 0x0, 6400, 0, target_tram);
	//load_set(0, 160, 312, target_tram);
  	
	load_data(din_addr[3], 0x10000, 36, 0, target_tram);
	load_data(din_addr[4], 0x14000, 36, 0, target_tram);
	load_data(din_addr[5], 0x2000, 36, 0, target_tram);
	load_data(din_addr[6], 0x1a000, 36, 0, target_tram);
	load_data(din_addr[7], 0x1c000, 36, 0, target_tram);
	load_data(din_addr[8], 0x4000, 36, 0, target_tram);
	load_data(din_addr[9], 0x6000, 36, 0, target_tram);
	load_data(din_addr[10], 0x8000, 36, 0, target_tram);
	load_data(din_addr[11], 0xa000, 36, 0, target_tram);

	config(0x0, 114, target_tram);
	execute(0xff7f, target_tram);

	//store_set(38, 152, 304, target_tram);
	store(dout_addr[0], 0x16000, 5776, target_tram);
	store(dout_addr[1], 0x1e000, 5776, target_tram);
	store(dout_addr[2], 0xc000, 5776, target_tram);
	//store_set(0, 152, 304, target_tram);
}

void work()
{
    for (int oc = 0; oc < OC; oc ++) {
        for (int ic = 0; ic < IC; ic ++) {
            for (int h = 0; h < IH - KH + 1; h = h + SH) {
                for (int w = 0; w < IW - KW + 1; w = w + SW) {
                    for (int kh = 0; kh < KH; kh ++) {
                        for (int kw = 0; kw < KW; kw ++) {
                            //#ifdef CGRA_COMPILER
                            //please_map_me();
                           R[oc][h / SH][w / SW] += A[ic][h + kh][w + kw] * W[oc][ic][kh][kw];
	        	            //#endif
                        }
                    }
                }
            }
        }
    }
}

void init(){
    for (int ic = 0; ic < IC; ic ++ ){
        for (int h = 0; h < IH; h = h + 1 ) {
            for (int w = 0; w < IW; w = w + 1 ) {
                A[ic][h][w] = (h + w) % 10;
            }
        }
    }

    for (int ic = 0; ic < IC; ic ++){
        for (int oc = 0; oc < OC; oc ++){
            for (int h = 0; h < KH; h = h + 1){
                for (int w = 0; w < KW; w = w + 1){
                    W[oc][ic][h][w] = h + w;
                }
            }
        }
    }
}

void check(){
    for (int oc = 0; oc < OC; oc ++){
        for (int h = 0; h < OH; h = h + 10 * SH ) {
            for (int w = 0; w < OW; w = w + 10 * SW) {
                if(R[oc][h][w] != B[oc][h][w]) printf("Wrong in location (%d, %d, %d): True: %d, Wrong: %d\n", oc, h, w, R[oc][h][w], B[oc][h][w]);
            }
        }
    }
}

int main(){
    init();
    printf("Initialization done!\n");
    long long unsigned start, end;

    start = rdcycle();
    void* cgra_din_addr1[12] = {&A[0][0], &A[1][0], &A[2][0], &W[0][0], &W[0][1], &W[0][2], &W[1][0], &W[1][1], &W[1][2], &W[2][0], &W[2][1], &W[2][2]};
    void* cgra_dout_addr1[3] = {&B[0][0], &B[1][0], &B[2][0]};
    cgra_execute(cgra_din_addr1, cgra_dout_addr1, 0);
    void* cgra_din_addr2[12] = {&A[0][38], &A[1][38], &A[2][38], &W[0][0], &W[0][1], &W[0][2], &W[1][0], &W[1][1], &W[1][2], &W[2][0], &W[2][1], &W[2][2]};
    void* cgra_dout_addr2[3] = {&B[0][38], &B[1][38], &B[2][38]};	
	cgra_execute(cgra_din_addr2, cgra_dout_addr2, 1);
    volatile int result = fence(1, 0);
	result = fence(1, 1);
    end = rdcycle();
    printf("It takes %d cycles for CGRA to finish the task(%d)\n", end - start, result);

//    start = rdcycle();
    work();
//    end = rdcycle();
//    printf("It takes %d cycles for CPU to finish the task.\n", end - start);

    check();
    printf("done!(%d)\n", B[0][0][0]);
}