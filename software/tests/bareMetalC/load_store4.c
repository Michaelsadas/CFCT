#include <stdio.h>
#include "include/ISA.h"
#include "include/encoding.h"
#define IH 31
#define IW 31
#define OH 31
#define OW 31
#define KH 5
#define KW 5
#define SH 1
#define SW 1
#define IC 3
#define OC 3

int A[IC][IH + 1][IW] __attribute__((aligned(8)));
int B[OC][OH + 1][OW] __attribute__((aligned(8)));

void init(){
    for (int ic = 0; ic < IC; ic ++ ){
        for (int h = 0; h < 2; h = h + 1 ) {
            for (int w = 0; w < IW; w = w + 1 ) {
                A[ic][h][w] = (h + w) % 10;
            }
        }
    }
}

void check(){
    for (int oc = 0; oc < OC; oc ++){
        for (int h = 0; h < 2; h ++) {
            for (int w = 0; w < OW; w ++) {
                if(A[oc][h][w] != B[oc][h][w]) printf("Wrong in location (%d, %d, %d): True: %d, Wrong: %d\n", oc, h, w, A[oc][h][w], B[oc][h][w]);
            }
        }
    }
}

int main(){
    init();
    printf("Initialization done!\n");
    long long unsigned start, end;

    load_data(&A[0], 0x0000, 200, 0, 1);
    load_data(&A[1], 0x2000, 200, 0, 1);
    load_data(&A[2], 0x4000, 200, 0, 1);
    fence(0);
    printf("Load ok !\n");
//    store(&B[0], 0x0000, 200, 1);
    fence(0);
    store(&B[1], 0x2000, 200, 1);
    fence(0);
//    store(&B[2], 0x4000, 200, 1);
    volatile int result = fence(1);

    check();
    printf("done!(%d)\n", B[0][0][0]);
}