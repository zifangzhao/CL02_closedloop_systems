#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_B [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_B [MWSPT_NSEC][3] = {
{ 
0.00096348, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.00093254, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_B [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_B [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.9492, 0.95307,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.8866, 0.89034,
 },
{ 
1, 0, 0,
 }
};

