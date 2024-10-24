#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_R1 [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_R1 [MWSPT_NSEC][3] = {
{ 
0.021197, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.13673, 0, 0,
 },
{ 
1, 1, 0,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_R1 [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_R1 [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.6476, 0.73234,
 },
{ 
1, 0, 0,
 },
{ 
1, -0.72654, 0,
 },
{ 
1, 0, 0,
 }
};

