#include "tmwtypes.h"
#define MWSPT_NSEC 5
const int NL_SPW [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T NUM_SPW [MWSPT_NSEC][3] = {
{ 
0.05439, 0, 0,
 },
{ 
1, 0, -1,
 },
{ 
0.05439, 0, 0,
 },
{ 
1, 0, -1,
 },
{ 
1, 0, 0,
 }
};
const int DL_SPW [MWSPT_NSEC][3] = { 1,3,1,3,1 };
const real32_T DEN_SPW [MWSPT_NSEC][3] = {
{ 
1, 0, 0,
 },
{ 
1, -1.872, 0.89894,
 },
{ 
1, 0, 0,
 },
{ 
1, -1.9411, 0.948,
 },
{ 
1, 0, 0,
 }
};

