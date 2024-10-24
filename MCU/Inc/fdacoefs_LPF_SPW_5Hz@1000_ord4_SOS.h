#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_LPF_SPW [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_LPF_SPW [MWSPT_NSEC][3] = {
{ 
0.00024379, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
0.00023976, 0, 0,
 },
{ 
1, 2, 1,
 },
{ 
1, 0, 0,
 }
};
const int DL_LPF_SPW [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_LPF_SPW [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.9753, 0.97624,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.9426, 0.9436,
 },
{ 
1, 0, 0,
 }
};

