#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_E [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_E [MWSPT_NSEC][3] = {
{ 
0.013971, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.11216, 0, 0,
 },
{ 
1, 1, 0,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_E [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_E [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.7229, 0.77881,
 },
{ 
1, 0, 0,
 },
{ 
1, -0.77568, 0,
 },
{ 
1, 0, 0,
 }
};

