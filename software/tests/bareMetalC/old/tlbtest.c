// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/rocc.h"

static inline unsigned long translate(void *ptr, int cmd)
{
	unsigned long value;
	ROCC_INSTRUCTION_DSS(1, value, (uintptr_t) ptr, cmd, 0);
	return value;
}



int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

    unsigned long data1 = 0x3421L;
    unsigned long data2 = 0x3425L;
    unsigned long result;
    result = translate(&data1, 0);
    printf("data1 virtual address: 0x%x, physical address: 0x%x\n", &data1, result);
    result = translate(&data2, 1);
    printf("data2 virtual address: 0x%x, physical address: 0x%x\n", &data2, result);
    result = translate(&data1, 1);
    printf("data1 virtual address: 0x%x, physical address: 0x%x\n", &data1, result);
    result = translate(&data2, 0);
    printf("data2 virtual address: 0x%x, physical address: 0x%x\n", &data2, result);
    result = translate(&data1, (1<<8) | 0);
    printf("data1 virtual address: 0x%x, physical address: 0x%x\n", &data1, result);
    result = translate(&data2, (1<<8) | 1);
    printf("data2 virtual address: 0x%x, physical address: 0x%x\n", &data2, result);
    result = translate(&data1, (1<<8) | 1);
    printf("data1 virtual address: 0x%x, physical address: 0x%x\n", &data1, result);
    result = translate(&data2, (1<<8) | 0);
    printf("data2 virtual address: 0x%x, physical address: 0x%x\n", &data2, result);

}

