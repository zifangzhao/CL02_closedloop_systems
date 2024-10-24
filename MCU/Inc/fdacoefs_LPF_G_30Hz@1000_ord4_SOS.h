/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.11 and Signal Processing Toolbox 8.7.
 * Generated on: 26-Jan-2024 17:00:38
 */

/*
 * Discrete-Time IIR Filter (real)
 * -------------------------------
 * Filter Structure    : Direct-Form II, Second-Order Sections
 * Number of Sections  : 2
 * Stable              : Yes
 * Linear Phase        : No
 */

/* General type conversion for MATLAB generated C-code  */
#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * C:\Program Files\MATLAB\R2021b\extern\include\tmwtypes.h 
 */
/*
 * Warning - Filter coefficients were truncated to fit specified data type.  
 *   The resulting response may not match generated theoretical response.
 *   Use the Filter Design & Analysis Tool to design accurate
 *   single-precision filter coefficients.
 */
#define MWSPT_NSEC 5
const int NL_LPF_G[MWSPT_NSEC] = { 1,3,1,3,1 };
const real32_T NUM_LPF_G[MWSPT_NSEC][3] = {
  {
    0.01434336789,              0,              0 
  },
  {
                1,              2,              1 
  },
  {
     0.0127735706,              0,              0 
  },
  {
                1,              2,              1 
  },
  {
                1,              0,              0 
  }
};
const int DL_LPF_G[MWSPT_NSEC] = { 1,3,1,3,1 };
const real32_T DEN_LPF_G[MWSPT_NSEC][3] = {
  {
                1,              0,              0 
  },
  {
                1,   -1.768827915,   0.8262013197 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.575240016,     0.62633425 
  },
  {
                1,              0,              0 
  }
};
