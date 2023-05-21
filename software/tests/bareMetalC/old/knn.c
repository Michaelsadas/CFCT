#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
//#include "include/polybench.h"

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"

#define TYPE unsigned

// Problem Constants
#define nAtoms        64
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
unsigned NL[nAtoms*maxNeighbors];

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[104][3] __attribute__((aligned(8))) = {
		{0x1800, 0x4000, 0x0004},
		{0x0040, 0x0200, 0x0005},
		{0x0000, 0xab00, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x1000, 0x4000, 0x0008},
		{0x0040, 0x0200, 0x0009},
		{0x0000, 0x0500, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x0800, 0x4000, 0x000c},
		{0x0040, 0x0200, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x0000, 0x0000, 0x0010},
		{0x0010, 0x0000, 0x0011},
		{0x0000, 0x0d00, 0x0012},
		{0x0081, 0x0000, 0x0013},
		{0x0800, 0x0000, 0x0014},
		{0x0010, 0x0000, 0x0015},
		{0x0000, 0x0900, 0x0016},
		{0x0081, 0x0000, 0x0017},
		{0x2000, 0x4000, 0x0018},
		{0x0040, 0x0200, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x1000, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x0900, 0x001e},
		{0x0081, 0x0000, 0x001f},
		{0x1800, 0x4000, 0x0020},
		{0x0040, 0x0200, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x1000, 0x0000, 0x0030},
		{0x0000, 0x0000, 0x0034},
		{0x0003, 0x0000, 0x0038},
		{0x0100, 0x0010, 0x003c},
		{0x0000, 0x0000, 0x0040},
		{0x0101, 0x0000, 0x0044},
		{0x0830, 0x0000, 0x0051},
		{0x4442, 0x0000, 0x0055},
		{0x4442, 0x0000, 0x0059},
		{0x6c03, 0x0000, 0x005d},
		{0x0004, 0x0000, 0x0060},
		{0x0403, 0x0000, 0x0061},
		{0x0004, 0x0000, 0x0064},
		{0x0403, 0x0000, 0x0065},
		{0x2442, 0x0000, 0x0069},
		{0x0000, 0x0000, 0x0070},
		{0x0000, 0x0200, 0x0074},
		{0x3500, 0x0000, 0x0078},
		{0x0000, 0x0000, 0x007c},
		{0x3000, 0x0000, 0x0080},
		{0x2000, 0x0001, 0x0084},
		{0x3000, 0x0000, 0x0088},
		{0x0500, 0x0001, 0x008c},
		{0x100d, 0x0000, 0x0095},
		{0x0000, 0x3001, 0x0096},
		{0x0041, 0x0010, 0x0097},
		{0x0800, 0x0000, 0x0099},
		{0x2403, 0x0000, 0x009d},
		{0x6811, 0x0000, 0x00a1},
		{0x0004, 0x0000, 0x00a4},
		{0x0803, 0x0000, 0x00a5},
		{0x2801, 0x0000, 0x00a9},
		{0x0800, 0x0000, 0x00ad},
		{0x2403, 0x0000, 0x00b1},
		{0x8101, 0x0000, 0x00bc},
		{0x1000, 0x0000, 0x00c4},
		{0x4000, 0x0000, 0x00c8},
		{0x0000, 0x0000, 0x00d0},
		{0x5203, 0x0000, 0x00dd},
		{0x0430, 0x0000, 0x00e1},
		{0xf830, 0xffff, 0x00e4},
		{0x0801, 0x0000, 0x00e5},
		{0x5083, 0x0000, 0x00e9},
		{0x000a, 0x0000, 0x00ec},
		{0x2004, 0x0000, 0x00ed},
		{0x0840, 0x0000, 0x00f1},
		{0x1402, 0x0000, 0x0104},
		{0x1400, 0x0001, 0x0108},
		{0x1100, 0x0040, 0x010c},
		{0x0500, 0x0080, 0x0110},
		{0x0600, 0x0000, 0x0114},
		{0x080d, 0x0000, 0x0125},
		{0x0000, 0x2001, 0x0126},
		{0x0041, 0x0010, 0x0127},
		{0x2a03, 0x0000, 0x0129},
		{0x2803, 0x0000, 0x012d},
		{0x4503, 0x0000, 0x0131},
		{0x2403, 0x0000, 0x0135},
		{0x2603, 0x0000, 0x0139},
		{0x0c0d, 0x0000, 0x013d},
		{0x0000, 0x3001, 0x013e},
		{0x0041, 0x0010, 0x013f},
		{0x0000, 0x0000, 0x0148},
		{0x0100, 0x0000, 0x0160},
		{0x0000, 0x4000, 0x016c},
		{0x0040, 0x0200, 0x016d},
		{0x0000, 0xa700, 0x016e},
		{0x0000, 0x0000, 0x016f},
		{0x0000, 0x4000, 0x0184},
		{0x0040, 0x0200, 0x0185},
		{0x0000, 0xa900, 0x0186},
		{0x0000, 0x0000, 0x0187},
	};

	load_cfg((void*)cin, 0x20000, 624, 0, 0);
	load_data(din_addr[0], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0xa000, 256, 1, 0, 0);
	load_data(din_addr[2], 0x2000, 256, 0, 0, 0);
	load_data(din_addr[3], 0xc000, 256, 1, 0, 0);
	load_data(din_addr[4], 0xe000, 256, 0, 0, 0);
	load_data(din_addr[5], 0x0, 256, 1, 0, 0);
	load_data(din_addr[6], 0x4000, 256, 0, 0, 0);
	config(0x0, 104, 0, 0);
	execute(0x41ff, 0, 0);
	store(dout_addr[0], 0x10000, 256, 0, 0);
	store(dout_addr[1], 0x18000, 256, 0, 0);
	store(dout_addr[2], 0x6000, 256, 0, 0);
}

/* Array initialization. */
void init_array()
{
    int i, j;
    for  (i = 0; i < nAtoms; i++)
    {
        position_x[i] = i;
        position_y[i] = i+1;
        position_z[i] = i+2;
//        force_x[i] = 0;
//        force_y[i] = 0;
//        force_z[i] = 0;
    }
    for(i=0; i<nAtoms; i++){
        for(j = 0; j < maxNeighbors; ++j){
            NL[i*maxNeighbors+j] = (i+j)%nAtoms;
        }
    }
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
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
             r2inv = 10/( delx*delx + dely*dely + delz*delz );//r2inv = 1.0/( delx*delx + dely*dely + delz*delz );
             // Assume no cutoff and aways account for all nodes in area
             r6inv = r2inv * r2inv * r2inv;
             potential = r6inv*(lj1*r6inv - lj2*1000);
             // Sum changes in force
             force = r2inv*potential;
             /*potential = r6inv/100000000*(lj1*r6inv - lj2*1000000000);
             // Sum changes in force
             force = r2inv*potential/10000000000;*/
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

void result_check()
{
  int i, j;

  for (i = 0; i < nAtoms; i++)
  {
    if (force_x0[i] != force_x[i] || force_y0[i] != force_y[i] || force_z0[i] != force_z[i])
        printf("There is an error in location (%d)[%d, %d, %d; %d, %d, %d]\n",
            i, force_x0[i], force_x[i], force_y0[i], force_y[i], force_z0[i], force_z[i]);
  }
}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  init_array();
  printf("Initialization finished!\n");

  start = rdcycle();
  /* Run kernel. */
  md_kernel();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[7] = {NL, position_x, position_x, position_y, position_y, position_z, position_z};
  void* cgra_dout_addr[3] = {force_x, force_y, force_z};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}