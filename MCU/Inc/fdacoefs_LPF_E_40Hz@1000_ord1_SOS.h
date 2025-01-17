#include "tmwtypes.h"
#define MWSPT_NSEC_LPF_E 3
const int NL_LPF_E [MWSPT_NSEC_LPF_E][3] = { 1,3,1 };
const real32_T NUM_LPF_E [MWSPT_NSEC_LPF_E][3] = {
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
const int DL_LPF_E [MWSPT_NSEC_LPF_E][3] = { 1,3,1 };
const real32_T DEN_LPF_E [MWSPT_NSEC_LPF_E][3] = {
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

