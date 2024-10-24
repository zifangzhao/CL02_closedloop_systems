#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_B [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_B [MWSPT_NSEC][3] = {
{ 
0.0009566, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.030469, 0, 0,
 },
{ 
1, 1, 0,
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
1, -1.9353, 0.93912,
 },
{ 
1, 0, 0,
 },
{ 
1, -0.93906, 0,
 },
{ 
1, 0, 0,
 }
};

