#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_R1 [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_R1 [MWSPT_NSEC][3] = {
{ 
0.021884, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.019037, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_R1 [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_R1 [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.701, 0.7885,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.4797, 0.55582,
 },
{ 
1, 0, 0,
 }
};

