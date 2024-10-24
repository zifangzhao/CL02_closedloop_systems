#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_A [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_A [MWSPT_NSEC][3] = {
{ 
0.00015595, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.012411, 0, 0,
 },
{ 
1, 1, 0,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_A [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_A [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.9746, 0.97518,
 },
{ 
1, 0, 0,
 },
{ 
1, -0.97518, 0,
 },
{ 
1, 0, 0,
 }
};

