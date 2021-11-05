/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 8.1 and the Signal Processing Toolbox 6.19.
 * Generated on: 26-Dec-2018 18:25:16
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
 * C:\Program Files\MATLAB\R2013a\extern\include\tmwtypes.h 
 */
/*
 * Warning - Filter coefficients were truncated to fit specified data type.  
 *   The resulting response may not match generated theoretical response.
 *   Use the Filter Design & Analysis Tool to design accurate
 *   single-precision filter coefficients.
 */
#define MWSPT_NSEC 5
const int NL_B[MWSPT_NSEC] = { 1,3,1,3,1 };
const real32_T NUM_B[MWSPT_NSEC][3] = {
  {
   0.002665322507,              0,              0 
  },
  {
                1,              0,             -1 
  },
  {
   0.002665322507,              0,              0 
  },
  {
                1,              0,             -1 
  },
  {
                1,              0,              0 
  }
};
const int DL_B[MWSPT_NSEC] = { 1,3,1,3,1 };
const real32_T DEN_B[MWSPT_NSEC][3] = {
  {
                1,              0,              0 
  },
  {
                1,   -1.995019317,   0.9950911403 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.997350931,   0.9973714948 
  },
  {
                1,              0,              0 
  }
};
