#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_I [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_I [MWSPT_NSEC][3] = {
{ 
0.0082638, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.0075494, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_I [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_I [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.8331, 0.86618,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.6747, 0.70486,
 },
{ 
1, 0, 0,
 }
};

