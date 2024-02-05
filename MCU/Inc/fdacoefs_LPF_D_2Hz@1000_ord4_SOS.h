/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.11 and Signal Processing Toolbox 8.7.
 * Generated on: 26-Jan-2024 16:58:04
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
const int NL_LPF_D[MWSPT_NSEC] = { 1,3,1,3,1 };
const real32_T NUM_LPF_D[MWSPT_NSEC][3] = {
  {
  3.928896331e-05,              0,              0 
  },
  {
                1,              2,              1 
  },
  {
  3.902483877e-05,              0,              0 
  },
  {
                1,              2,              1 
  },
  {
                1,              0,              0 
  }
};
const int DL_LPF_D[MWSPT_NSEC] = { 1,3,1,3,1 };
const real32_T DEN_LPF_D[MWSPT_NSEC][3] = {
  {
                1,              0,              0 
  },
  {
                1,   -1.990271211,   0.9904283881 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.976891398,   0.9770474434 
  },
  {
                1,              0,              0 
  }
};