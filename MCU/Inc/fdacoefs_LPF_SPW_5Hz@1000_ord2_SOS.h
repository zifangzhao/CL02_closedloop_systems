#include "tmwtypes.h"
#define MWSPT_NSEC 3
const int NL_LPF_SPW [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T NUM_LPF_SPW [MWSPT_NSEC][3] = {
{ 
0.00024136, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_SPW [MWSPT_NSEC][3] = { 1,3,1 };
const real32_T DEN_LPF_SPW [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.9556, 0.95654,
 },
{ 
1, 0, 0,
 }
};

