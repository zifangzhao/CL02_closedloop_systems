#include "tmwtypes.h"
#define MWSPT_NSEC 3
const int NL_LPF_R1 [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T NUM_LPF_R1 [MWSPT_NSEC][3] = {
{ 
0.020083, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_R1 [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T DEN_LPF_R1 [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.561, 0.64135,
 },
{ 
1, 0, 0,
 }
};

