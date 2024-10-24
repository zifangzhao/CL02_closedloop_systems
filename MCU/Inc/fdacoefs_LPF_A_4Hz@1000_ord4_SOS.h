#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_A [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_A [MWSPT_NSEC][3] = {
{ 
0.0001564, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.00015432, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_A [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_A [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.9803, 0.98095,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.954, 0.95462,
 },
{ 
1, 0, 0,
 }
};

