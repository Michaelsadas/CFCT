/*
Implemenataion based on:
A. Danalis, G. Marin, C. McCurdy, J. S. Meredith, P. C. Roth, K. Spafford, V. Tipparaju, and J. S. Vetter.
The scalable heterogeneous computing (shoc) benchmark suite.
In Proceedings of the 3rd Workshop on General-Purpose Computation on Graphics Processing Units, 2010.
*/

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif

#define TYPE int

// Problem Constants
#define nAtoms        256
#define maxNeighbors  16
// LJ coefficients
#define lj1           1//1.5
#define lj2           2//2.0

TYPE force_x[nAtoms];
TYPE force_y[nAtoms];
TYPE force_z[nAtoms];
TYPE force_x0[nAtoms];
TYPE force_y0[nAtoms];
TYPE force_z0[nAtoms];
TYPE position_x[nAtoms];
TYPE position_y[nAtoms];
TYPE position_z[nAtoms];
int32_t NL[nAtoms*maxNeighbors];

void md_kernel()
{
    TYPE delx, dely, delz, r2inv;
    TYPE r6inv, potential, force, j_x, j_y, j_z;
    TYPE i_x, i_y, i_z, fx, fy, fz;

    int32_t i, j, jidx;

loop_i : for (i = 0; i < nAtoms; i++){
             i_x = position_x[i];
             i_y = position_y[i];
             i_z = position_z[i];
             fx = 0;
             fy = 0;
             fz = 0;
loop_j : for( j = 0; j < maxNeighbors; j++){
//	please_map_me();
             // Get neighbor
             jidx = NL[i*maxNeighbors + j];
             // Look up x,y,z positions
             j_x = position_x[jidx];
             j_y = position_y[jidx];
             j_z = position_z[jidx];
             // Calc distance
             delx = i_x - j_x;
             dely = i_y - j_y;
             delz = i_z - j_z;
             r2inv = 1/( delx*delx + dely*dely + delz*delz );//r2inv = 1.0/( delx*delx + dely*dely + delz*delz );
             // Assume no cutoff and aways account for all nodes in area
             r6inv = r2inv * r2inv * r2inv;
             potential = r6inv*(lj1*r6inv - lj2);
             // Sum changes in force
             force = r2inv*potential;
             fx += delx * force;
             fy += dely * force;
             fz += delz * force;
         }
         //Update forces after all neighbors accounted for.
         force_x0[i] = fx;
         force_y0[i] = fy;
         force_z0[i] = fz;
         //printf("dF=%lf,%lf,%lf\n", fx, fy, fz);
         }
}

TYPE distance(
//        TYPE position_x[nAtoms],
//        TYPE position_y[nAtoms],
//        TYPE position_z[nAtoms],
        int i,
        int j)
{
    TYPE delx, dely, delz, r2inv;
    delx = position_x[i] - position_x[j];
    dely = position_y[i] - position_y[j];
    delz = position_z[i] - position_z[j];
    r2inv = delx * delx + dely * dely + delz * delz;
    return r2inv;
}

inline void insertInOrder(TYPE currDist[maxNeighbors],
        int currList[maxNeighbors],
        int j,
        TYPE distIJ)
{
    int dist, pos, currList_t;
    TYPE currMax, currDist_t;
    pos = maxNeighbors - 1;
    currMax = currDist[pos];
    if (distIJ > currMax){
        return;
    }
    for (dist = pos; dist > 0; dist--){
        if (distIJ < currDist[dist]){
            currDist[dist] = currDist[dist - 1];
            currList[dist]  = currList[pos  - 1];
        }
        else{
            break;
        }
        pos--;
    }
    currDist[dist] = distIJ;
    currList[dist]  = j;
}

int populateNeighborList(TYPE currDist[maxNeighbors],
        int currList[maxNeighbors],
        const int i
//        int NL[nAtoms][maxNeighbors]
){
    int idx, validPairs, distanceIter, neighborIter;
    idx = 0; validPairs = 0;
    for (neighborIter = 0; neighborIter < maxNeighbors; neighborIter++){
//        NL[i][neighborIter] = currList[neighborIter];
        NL[i*maxNeighbors + neighborIter] = currList[neighborIter];
        validPairs++;
    }
    return validPairs;
}

int buildNeighborList(
//        TYPE position_x[nAtoms],
//        TYPE position_y[nAtoms],
//        TYPE position_z[nAtoms],
//        int NL[nAtoms][maxNeighbors]
        )
{
    int totalPairs, i, j, k;
    totalPairs = 0;
    TYPE distIJ;
    for (i = 0; i < nAtoms; i++){
        int currList[maxNeighbors];
        TYPE currDist[maxNeighbors];
        for(k=0; k<maxNeighbors; k++){
            currList[k] = 0;
            currDist[k] = 999999999;
        }
        for (j = 0; j < maxNeighbors; j++){
            if (i == j){
                continue;
            }
            distIJ = distance(i, j);
            currList[j] = j;
            currDist[j] = distIJ;
        }
        totalPairs += populateNeighborList(currDist, currList, i);
    }
    return totalPairs;
}



#include "include/rocc.h"
#include "include/ISA.h"
#include "include/encoding.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	unsigned short int cin[123][3] __attribute__((aligned(8))) = {
		{0x0000, 0x0000, 0x0008},
		{0x0401, 0x0000, 0x0009},
		{0x0002, 0x0000, 0x000a},
		{0x0110, 0x0000, 0x000b},
		{0x1000, 0x0000, 0x0010},
		{0x0401, 0x0000, 0x0011},
		{0x0002, 0x0000, 0x0012},
		{0x1810, 0x0101, 0x0013},
		{0x1000, 0x0000, 0x0018},
		{0x0401, 0x0000, 0x0019},
		{0x0002, 0x0000, 0x001a},
		{0x1710, 0x0101, 0x001b},
		{0x0000, 0x0000, 0x0020},
		{0x0100, 0x0000, 0x0021},
		{0x0000, 0x0000, 0x0022},
		{0x0410, 0x0102, 0x0023},
		{0x1000, 0x0000, 0x0028},
		{0x0401, 0x0000, 0x0029},
		{0x0002, 0x0000, 0x002a},
		{0x1710, 0x0101, 0x002b},
		{0x2000, 0x0000, 0x0030},
		{0x0401, 0x0000, 0x0031},
		{0x0002, 0x0000, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x0000, 0x0000, 0x0038},
		{0x0100, 0x0000, 0x0039},
		{0x0000, 0x0000, 0x003a},
		{0x0410, 0x0002, 0x003b},
		{0x1000, 0x0000, 0x0040},
		{0x0401, 0x0000, 0x0041},
		{0x0002, 0x0000, 0x0042},
		{0x0010, 0x0000, 0x0043},
		{0x0000, 0x0000, 0x0058},
		{0x8003, 0x0000, 0x0060},
		{0x2003, 0x0000, 0x0068},
		{0x0000, 0x0002, 0x0070},
		{0x0103, 0x0012, 0x0078},
		{0x0010, 0x0001, 0x0080},
		{0x5100, 0x0000, 0x0088},
		{0x8882, 0x0000, 0x00a1},
		{0xd001, 0x0000, 0x00a9},
		{0x0004, 0x0000, 0x00b8},
		{0x1003, 0x0000, 0x00b9},
		{0x0004, 0x0000, 0x00c0},
		{0x0803, 0x0000, 0x00c1},
		{0x9003, 0x0000, 0x00c9},
		{0x4882, 0x0000, 0x00d1},
		{0x0000, 0x0040, 0x00e8},
		{0x0080, 0x0020, 0x00f0},
		{0x0000, 0x0020, 0x00f8},
		{0x0000, 0x0620, 0x0108},
		{0x1100, 0x0000, 0x0118},
		{0x9003, 0x0000, 0x0129},
		{0x0800, 0x0000, 0x0139},
		{0x1080, 0x0000, 0x0159},
		{0x0820, 0x0000, 0x0161},
		{0x0000, 0x0020, 0x0180},
		{0x0000, 0x0620, 0x0188},
		{0x2000, 0x0820, 0x0198},
		{0x0000, 0x0000, 0x01a0},
		{0x1080, 0x0000, 0x01c1},
		{0x0004, 0x0000, 0x01d8},
		{0x1003, 0x0000, 0x01d9},
		{0x0000, 0x0000, 0x0210},
		{0x0000, 0x0600, 0x0218},
		{0x0000, 0x0200, 0x0220},
		{0x0000, 0x0600, 0x0228},
		{0x200f, 0x0000, 0x0259},
		{0x0000, 0x8002, 0x025a},
		{0x080a, 0x2000, 0x025b},
		{0x0000, 0x0000, 0x025c},
		{0x200f, 0x0000, 0x0261},
		{0x0000, 0x8002, 0x0262},
		{0x080a, 0x2000, 0x0263},
		{0x0000, 0x0000, 0x0264},
		{0x180f, 0x0000, 0x0271},
		{0x0000, 0x8002, 0x0272},
		{0x080a, 0x2000, 0x0273},
		{0x0000, 0x0000, 0x0274},
		{0x0000, 0x0600, 0x02a0},
		{0x0803, 0x3000, 0x02a8},
		{0x0000, 0x3200, 0x02b0},
		{0x0020, 0x0002, 0x02b8},
		{0x0403, 0x0003, 0x02f1},
		{0xc403, 0x0004, 0x02f9},
		{0x0002, 0x0000, 0x0300},
		{0xc00c, 0x0000, 0x0301},
		{0x0000, 0x0000, 0x0308},
		{0xc02e, 0x0030, 0x0309},
		{0x0000, 0x0c00, 0x0338},
		{0x20c2, 0x6000, 0x0340},
		{0x0040, 0x0400, 0x0348},
		{0x4080, 0x0001, 0x0350},
		{0x0000, 0x0001, 0x0358},
		{0xe403, 0x0000, 0x0381},
		{0x0801, 0x0001, 0x0389},
		{0x9203, 0x0000, 0x0391},
		{0x5103, 0x0000, 0x0399},
		{0x4803, 0x0000, 0x03a1},
		{0x00c0, 0x0000, 0x03c8},
		{0x0000, 0x0600, 0x03d0},
		{0x400c, 0x0000, 0x03d8},
		{0x0000, 0x0000, 0x03e0},
		{0x2000, 0x0000, 0x0411},
		{0x9003, 0x0000, 0x0419},
		{0xfffe, 0xffff, 0x0420},
		{0x1001, 0x0000, 0x0421},
		{0x0100, 0x0000, 0x0458},
		{0x0000, 0x0600, 0x0460},
		{0x0000, 0x0000, 0x0468},
		{0x0840, 0x0000, 0x04a1},
		{0x1882, 0x0001, 0x04a9},
		{0xd803, 0x0000, 0x04b1},
		{0x0310, 0x0000, 0x04f0},
		{0x0004, 0x0000, 0x04f8},
		{0x0000, 0x0000, 0x0530},
		{0x0401, 0x0000, 0x0531},
		{0x0002, 0x0000, 0x0532},
		{0x0410, 0x0000, 0x0533},
		{0x0000, 0x0000, 0x0538},
		{0x0100, 0x0000, 0x0539},
		{0x0000, 0x0000, 0x053a},
		{0x0810, 0x0002, 0x053b},
	};

	load_cfg(cin, 0x40000, 738, 0, 0);
	load_data(din_addr[0], 0x10000, 16384, 0, 0);
	load_data(din_addr[1], 0x8000, 1024, 0, 0);
	load_data(din_addr[2], 0x18000, 1024, 0, 0);
	load_data(din_addr[3], 0x30000, 1024, 0, 0);
	load_data(din_addr[4], 0x0, 1024, 0, 0);
	load_data(din_addr[5], 0x1c000, 1024, 0, 0);
	load_data(din_addr[6], 0x28000, 1024, 0, 0);
	config(0x0, 123, 0, 0);
	execute(0x18ff, 0, 0);
	store(dout_addr[0], 0x14000, 1024, 0, 0);
	store(dout_addr[1], 0xc000, 1024, 0, 0);
	store(dout_addr[2], 0x4000, 1024, 0, 0);
	int result = fence(1);
}


int main(){
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

    unsigned long long time0, time1;

    printf("here");
    int i, iter, j, totalPairs;
    iter = 0;

//    srand(8650341L);

    printf("here");
//    TYPE position_x[nAtoms];
//    TYPE position_y[nAtoms];
//    TYPE position_z[nAtoms];
//    TYPE force_x[nAtoms];
//    TYPE force_y[nAtoms];
//    TYPE force_z[nAtoms];
//    TYPE NL[nAtoms][maxNeighbors];
    int size = nAtoms * maxNeighbors;
    int neighborList[size];

    printf("here");
    for  (i = 0; i < nAtoms; i++)
    {
        position_x[i] = i;
        position_y[i] = i+1;
        position_z[i] = 2*i;
//        force_x[i] = rand();
//        force_y[i] = rand();
//        force_z[i] = rand();
    }


    printf("here");
    for(i=0; i<nAtoms; i++){
        for(j = 0; j < maxNeighbors; ++j){
//            NL[i][j] = 0;
            NL[i*maxNeighbors + j] = 0;
        }
    }
    printf("here");
//    totalPairs = buildNeighborList(position_x, position_y, position_z, NL);
    totalPairs = buildNeighborList();

    for(i=0; i<nAtoms; i++){
        for(j = 0; j < maxNeighbors; ++j)
            neighborList[i*maxNeighbors + j] = NL[i*maxNeighbors + j];
    }

    //Function Call
    time0 = rdcycle();
    md_kernel();
    time0 = rdcycle() - time0;

    time1 = rdcycle();
    void* cgra_din_addr[7] = {NL, position_x, position_y, position_z, position_x, position_y, position_z};
    void* cgra_dout_addr[3] = {force_x, force_y, force_z};
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    time1 = rdcycle() - time1;

    printf("Execution time (CPU, CGRA): %ul, %ul\n", time0, time1);
    printf("Compare the result:\n");
    for  (i = 0; i < nAtoms; i++)
    {
        if(force_x[i] != force_x0[i] && force_y[i] != force_y0[i] && force_z[i] != force_z0[i]){
            printf("%x, %x, %x, %x, %x, %x\n", force_x[i], force_x0[i], force_y[i], force_y0[i], force_z[i], force_z0[i]);
        }
    }

    printf("Succeed!\n");
//    for(iter = 0; iter< MAX_ITERATION; iter++) {
//        md_kernel(force_x, force_y, force_z, position_x, position_y, position_z, neighborList);
//    }

    return 0;
}