#include "tmwtypes.h"
#define MWSPT_NSEC 3
const int NL_LPF_D [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T NUM_LPF_D [MWSPT_NSEC][3] = {
{ 
0.0031318, 0, 0,
 },
{ 
1, 1, 0,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_D [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T DEN_LPF_D [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -0.99374, 0,
 },
{ 
1, 0, 0,
 }
};

