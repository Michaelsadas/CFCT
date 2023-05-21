#include <stdio.h>
#include "include/encoding.h"
#include "include/ISA.h"

//This program is to test the function of move and fused
int main(){
//    int A[10][10];
//    int B[10][10];
//    int C[10][10];
//
//    for (int j = 0; j < 10; j++){
//        for (int i = 0; i < 10; i ++){
//            A[j][i] = i * 10 + j;
//        }
//    }
//
//    load_data(A, 0x0, 400, 1, 0);
//    load_data(A, 0x4000, 400, 1, 0);
//    load_data(A, 0x6000, 400, 0, 0);
//    move(0x0, 0x2000, 400);
//    fence(0);
//    store(B, 0x2000, 400, 1);
//    store(C, 0x0, 400, 1);
//    volatile int result = fence(1);
//    printf("Work finished!(%d)\n", result);
//    printf("B matrix:\n");
//    for (int i = 0; i < 10; i ++){
//        for (int j = 0; j < 10; j++){
//            printf("%d\t", B[i][j]);
//        }
//        printf("\n");
//    }
//    printf("C matrix:\n");
//    for (int i = 0; i < 10; i ++){
//        for (int j = 0; j < 10; j++){
//            printf("%d\t", C[i][j]);
//        }
//        printf("\n");
//    }
	move(0x20000, 0x0, 1336);
	move(0x14000, 0x20000, 1336);
	move(0x4000, 0x30000, 4);
	move(0x0, 0x4000, 1336);
    volatile int result = fence(1);
    printf("%d\n");
    return 0;
}