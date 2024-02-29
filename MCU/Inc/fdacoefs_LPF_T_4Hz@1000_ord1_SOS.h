#include "tmwtypes.h"
#define MWSPT_NSEC 3
const int NL_LPF_T [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T NUM_LPF_T [MWSPT_NSEC][3] = {
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
const int DL_LPF_T [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T DEN_LPF_T [MWSPT_NSEC][3] = {
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

