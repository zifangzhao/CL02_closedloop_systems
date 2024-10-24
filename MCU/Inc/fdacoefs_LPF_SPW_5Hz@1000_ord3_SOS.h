#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_SPW [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_SPW [MWSPT_NSEC][3] = {
{ 
0.0002429, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.015466, 0, 0,
 },
{ 
1, 1, 0,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_SPW [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_SPW [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.9681, 0.96907,
 },
{ 
1, 0, 0,
 },
{ 
1, -0.96907, 0,
 },
{ 
1, 0, 0,
 }
};
