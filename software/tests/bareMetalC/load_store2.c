#include <stdio.h>
#include "include/encoding.h"
#include "include/cdnn.h"

//This task is to test the function of dual cdnn load & store
int main(){
    int A[10][10];
    int B[10][10];
    int C[10][10];

    for (int j = 0; j < 10; j++){
        for (int i = 0; i < 10; i ++){
            A[j][i] = i * 10 + j;
        }
    }

    load_data(A, 0x0, 400, 0, 0);
    load_data(A, 0x0, 400, 0, 1);
    fence(0, 0);
    fence(0, 1);
    store(B, 0x0, 400, 0);
    store(C, 0x0, 400, 1);
    volatile int result = fence(1, 0);
    result = fence(1, 1);
    printf("Work finished!(%d)\n", result);
//    for(int i = 0; i < 5; i ++){
//        for (int j = 0; j < 20; j ++){
//            printf("%d\t", B[i][j]);
//        }
//        printf("\n");
//    }
    printf("B matrix:\n");
    for (int i = 0; i < 10; i ++){
        for (int j = 0; j < 10; j++){
            printf("%d\t", B[i][j]);
        }
        printf("\n");
    }
    printf("C matrix:\n");
    for (int i = 0; i < 10; i ++){
        for (int j = 0; j < 10; j++){
            printf("%d\t", C[i][j]);
        }
        printf("\n");
    }
    return 0;
}