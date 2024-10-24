#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_G [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_G [MWSPT_NSEC][3] = {
{ 
0.0021419, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.0020415, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_G [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_G [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.9219, 0.93048,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.8319, 0.84002,
 },
{ 
1, 0, 0,
 }
};

