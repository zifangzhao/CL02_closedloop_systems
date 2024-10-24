#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_R [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_R [MWSPT_NSEC][3] = {
{ 
0.026168, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.022514, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_R [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_R [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.6658, 0.77049,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.4332, 0.52328,
 },
{ 
1, 0, 0,
 }
};

