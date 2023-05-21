#include <stdio.h>
#include "include/encoding.h"
#include "include/ISA.h"

// This program is to test the function of loadset, fused and storeset
int main(){
    short int A[10][40];
    short int B[10][20];

    for (int j = 0; j < 10; j++){
        for (int i = 0; i < 20; i ++){
            A[j][i] = i * 10 + j;
        }
    }
    printf("Initialization finished!\n");
    load_set(10, 20, 80);
    load_data(A, 0x0, 20, 1, 1);
    load_data(A, 0x2000, 20, 1, 1);
    load_data(A, 0x4000, 20, 0, 1);
    load_set(0, 0, 0);
    fence(0);
//    printf("Load is ok!\n");
    store_set(10, 20, 40);
    store(B, 0x0, 20, 1);
    int result = fence(1);
    printf("Work finished!(%d)\n", result);
//    for(int i = 0; i < 5; i ++){
//        for (int j = 0; j < 20; j ++){
//            printf("%d\t", B[i][j]);
//        }
//        printf("\n");
//    }
    printf("A matrix:\n");
    for (int i = 0; i < 10; i ++){
        for (int j = 0; j < 10; j++){
            printf("%d\t", A[i][j]);
        }
        printf("\n");
    }

    printf("B matrix:\n");
    for (int i = 0; i < 10; i ++){
        for (int j = 0; j < 10; j++){
            printf("%d\t", B[i][j]);
        }
        printf("\n");
    }
    return 0;
}