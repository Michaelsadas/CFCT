#include <stdio.h>
#include "include/encoding.h"
#include "include/ISA.h"
#include "include/half.h"
#include <math.h>
#define TYPE float
#define batch 8
#define N 55

    float A[batch][N + 1][N];
    float R[batch][N + 1][N];
int main() {
    float sum = 0.0;
    for (int i = 0; i < batch; i ++) {
        for (int j = 0; j < N; j ++) {
            for (int k = 0; k < N; k ++) {
                A[i][j][k] = ((j + k) % 10) * 0.1;
            }
        }
    }
    printf("Initialization finished!\n");

    long long unsigned start, end;

    start = rdcycle();
    float mean = 0;
    for (int i = 0; i < batch; i ++) {
        for (int j = 0; j < N; j ++) {
            for (int k = 0; k < N; k ++) {
                mean += A[i][j][k];
            }

            
        }
    }
    mean = mean / 24200.0;
    printf("The mean number is %f\n", mean);
    
    float root = 0;
    for (int i = 0; i < batch; i ++) {
        for (int j = 0; j < N; j ++) {
            for (int k = 0; k < N; k ++) {
                root += pow((A[i][j][k] - mean), 2.0);
            }
        }
    }
    root = root / 24200.0;
    root = sqrt(root);
    printf("The root number is %f\n", root);

    for (int i = 0; i < batch; i ++) {
        for (int j = 0; j < N; j ++) {
            for (int k = 0; k < N; k ++) {
                R[i][j][k] = (A[i][j][k] - mean) / root;
            }
        }
    }
    end = rdcycle();
    printf("It takes %d to finish the work via CPU.\n");

    for(int b = 0; b < batch; b ++){
        for(int h = 0; h < N; h ++){
            for(int w = 0; w < N; w ++){
                sum += R[b][h][w];
            }
        }
    }
    printf("The final sum is %f\n", sum);
    return 0;
}