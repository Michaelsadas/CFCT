#include <stdio.h>
#include "include/ISA.h"
#include "include/encoding.h"

#define IH 51
#define IW 51
#define OH 11
#define OW 11
#define KH 11
#define KW 11
#define SH 4
#define SW 4
#define IC 3
#define OC 3
#define TYPE short int

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[41][3] __attribute__((aligned(8))) = {
		{0x0000, 0x3012, 0x02f8},
		{0x0000, 0x0004, 0x02f9},
		{0x0791, 0x0079, 0x02fa},
		{0x0010, 0x0000, 0x0340},
		{0x1000, 0x0000, 0x0348},
		{0x0000, 0x3003, 0x0380},
		{0x0002, 0x0000, 0x0381},
		{0x0000, 0x2012, 0x0388},
		{0x0000, 0x0004, 0x0389},
		{0x0791, 0x0079, 0x038a},
		{0x0000, 0xc003, 0x0390},
		{0x0001, 0x0000, 0x0391},
		{0x0010, 0x0000, 0x03c8},
		{0x0000, 0x0000, 0x03d0},
		{0x0840, 0x0000, 0x03d8},
		{0x0000, 0x0000, 0x03e0},
		{0x4000, 0x5800, 0x0408},
		{0x0080, 0x00b0, 0x0409},
		{0x7f88, 0x1001, 0x040a},
		{0x02ff, 0x0000, 0x040b},
		{0x0800, 0x0000, 0x040c},
		{0x0000, 0x0000, 0x040d},
		{0x4000, 0x5800, 0x0418},
		{0x1480, 0x00b0, 0x0419},
		{0x7dfc, 0x3801, 0x041a},
		{0x02fd, 0x0000, 0x041b},
		{0x0800, 0x0000, 0x041c},
		{0x0000, 0x0000, 0x041d},
		{0x6000, 0x101e, 0x0420},
		{0xc400, 0x079f, 0x0421},
		{0x0000, 0x0000, 0x0422},
		{0x0000, 0x0000, 0x0423},
		{0x5800, 0xe424, 0x0424},
		{0x3081, 0x0000, 0x0425},
		{0x0000, 0x0000, 0x0426},
		{0x5000, 0x5800, 0x0428},
		{0x0080, 0x00b0, 0x0429},
		{0x7f88, 0x1001, 0x042a},
		{0x02ff, 0x0000, 0x042b},
		{0x0800, 0x0000, 0x042c},
		{0x0000, 0x0000, 0x042d},
	};

	load_cfg((void*)cin, 0x20000, 246);
	load_data(din_addr[0], 0x18000, 5202, 0);
	load_data(din_addr[1], 0x1a000, 242, 0);
	load_data(din_addr[2], 0x10000, 242, 0);
	load_data(dout_addr[0], 0x1c000, 484, 0);
	config(0x0, 41);
	execute(0x7400);
	store(dout_addr[0], 0x1c000, 484);
}

int main(){
    TYPE A0[IH][IW];
    TYPE B0[2][OH][OW];
    TYPE R0[2][OH][OW];
    TYPE W00[KH][KW], W10[KH][KW];

    for(int ih = 0; ih < IH; ih++){
        for(int iw = 0; iw < IW; iw++){
            A0[ih][iw] = (ih + iw) % 10;
        }
    }
    for(int kh = 0; kh < KH; kh++){
        for(int kw = 0; kw < KW; kw++){
            W00[kh][kw] = (kh + kw) % 3;
            W10[kh][kw] = (kh + kw) % 4;
        }
    }
	for(int i = 0; i < 2; i++){
		for(int oh = 0; oh < OH; oh++){
			for(int ow = 0; ow < OW; ow++){
				R0[i][oh][ow] = B0[i][oh][ow] = (i + oh + ow) % 5;
			}
		}
	}
    printf("Initialization finished!\n");
    void* cgra_din_addr[3] = {A0, W00, W10};
    void* cgra_dout_addr[1] = {B0};
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    volatile int result = fence(1);
    printf("CGRA finished!(%d)\n", result);

    TYPE temp1, temp2;
	for(int h = 0; h < OH; h = h + 1){
        for(int w = 0; w < OW; w = w + 1){
            temp1 = 0;
            temp2 = 0;
            for(int kh = 0; kh < KH; kh ++){
                for(int kw = 0; kw < KW; kw ++){
                temp1 += A0[h * SH + kh][w * SW + kw] * W00[kh][kw];
                temp2 += A0[h * SH + kh][w * SW + kw] * W10[kh][kw];  
                }
            }
            R0[0][h][w] += temp1;
            R0[1][h][w] += temp2;
        }
    }
    printf("CPU finished!\n");
    for(int i = 0; i < 2; i++){
        for(int oh = 0; oh < OH; oh++){
            for(int ow = 0; ow < OW; ow++){
                if(R0[i][oh][ow] != B0[i][oh][ow]) printf("Wrong in the location(%d, %d, %d): True: %d, Wrong: %d\n", i, oh, ow, R0[i][oh][ow], B0[i][oh][ow]);
            }
        }
    }
    printf("done!\n");
    return 0; 
}
