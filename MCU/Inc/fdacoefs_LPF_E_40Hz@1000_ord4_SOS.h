#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_E [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_E [MWSPT_NSEC][3] = {
{ 
0.014343, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.012774, 0, 0,
 },
{ 
1, 2, 1,
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
1, -1.7688, 0.8262,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.5752, 0.62633,
 },
{ 
1, 0, 0,
 }
};

