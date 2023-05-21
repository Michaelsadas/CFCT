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

#define TYPE uint32_t
typedef uint32_t tok_t;
typedef TYPE prob_t;
typedef uint32_t state_t;
typedef uint32_t step_t;

tok_t obs[N_OBS];
prob_t init[N_STATES];
prob_t transition[N_STATES*N_STATES];
prob_t emission[N_STATES*N_TOKENS];
state_t path[N_OBS];
prob_t llike[N_OBS][N_STATES];

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[15][3] __attribute__((aligned(8))) = {
		{0x1801, 0x0001, 0x0251},
		{0x0110, 0x0000, 0x0298},
		{0x0000, 0x0000, 0x02a0},
		{0x2000, 0x0000, 0x02d8},
		{0x0040, 0x0000, 0x02d9},
		{0x0000, 0x0000, 0x02da},
		{0x0010, 0x0000, 0x02db},
		{0x3000, 0x0000, 0x02e0},
		{0x0040, 0x0000, 0x02e1},
		{0x0000, 0x0000, 0x02e2},
		{0x0410, 0x0001, 0x02e3},
		{0x2000, 0x0000, 0x02e8},
		{0x0040, 0x0000, 0x02e9},
		{0x0000, 0x0000, 0x02ea},
		{0x0010, 0x0000, 0x02eb},
	};

	load_cfg((void*)cin, 0x40000, 90, 0, 0);
	load_data(din_addr[0], 0x28000, 4096, 0, 0);
	load_data(din_addr[1], 0x20000, 4096, 0, 0);
	config(0x0, 15, 0, 0);
	execute(0x700, 0, 0);
	store(dout_addr[0], 0x24000, 4096, 0, 0);
}


/* Array initialization. */
void init_array()
{
  // Initialize with first observation and initial probabilities
  L_init: for( s=0; s<N_STATES; s++ ) {
    llike[0][s] = init[s] + emission[s*N_TOKENS+obs[0]];
  }

}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
int viterbi()
{
  step_t t;
  state_t prev, curr;
  prob_t min_p, p;
  state_t min_s, s;

  // Iteratively compute the probabilities over time
  L_timestep: for( t=1; t<N_OBS; t++ ) {
    L_curr_state: for( curr=0; curr<N_STATES; curr++ ) {
      // Compute likelihood HMM is in current state and where it came from.
      prev = 0;
      int tem = obs[t];
      min_p = llike[t-1][prev] +
              transition[prev*N_STATES+curr] +
              emission[curr*N_TOKENS+tem];
//      please_map_me();
      #pragma unroll
      for( prev=1; prev<N_STATES; prev++ ) {
        p = llike[t-1][prev] +
            transition[prev*N_STATES+curr] +
            emission[curr*N_TOKENS+tem];
        if( p<min_p ) {
          min_p = p;
        }
      }
      llike[t][curr] = min_p;
    }
  }

  // Identify end state
  min_s = 0;
  min_p = llike[N_OBS-1][min_s];
  L_end: for( s=1; s<N_STATES; s++ ) {
    p = llike[N_OBS-1][s];
    if( p<min_p ) {
      min_p = p;
      min_s = s;
    }
  }
  path[N_OBS-1] = min_s;

  // Backtrack to recover full path
  L_backtrack: for( t=N_OBS-2; t>=0; t-- ) {
    min_s = 0;
    min_p = llike[t][min_s] + transition[min_s*N_STATES+path[t+1]];
    L_state: for( s=1; s<N_STATES; s++ ) {
      p = llike[t][s] + transition[s*N_STATES+path[t+1]];
      if( p<min_p ) {
        min_p = p;
        min_s = s;
      }
    }
    path[t] = min_s;
  }


void result_check()
{
  int i, j;

  for (i = 0; i < SIZE; i++)
  {
    if (C[i] != D[i]) printf("There is an error in location (%d)[%d, %d]\n", i, C[i], D[i]);
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
  array_add();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[2] = {A, B};
  void* cgra_dout_addr[1] = {C};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}