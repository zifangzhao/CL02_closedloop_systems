#include "tmwtypes.h"
#define MWSPT_NSEC 3
const int NL_LPF_I [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T NUM_LPF_I [MWSPT_NSEC][3] = {
{ 
0.086364, 0, 0,
 },
{ 
1, 1, 0,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_I [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T DEN_LPF_I [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -0.82727, 0,
 },
{ 
1, 0, 0,
 }
};

