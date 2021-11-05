/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 8.1 and the Signal Processing Toolbox 6.19.
 * Generated on: 29-May-2018 00:22:35
 */

/*
 * Discrete-Time IIR Filter (real)
 * -------------------------------
 * Filter Structure    : Direct-Form II, Second-Order Sections
 * Number of Sections  : 5
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
#define MWSPT_NSEC 11
const int NL_E[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1,3,1 };
const real32_T NUM_E[MWSPT_NSEC][3] = {
  {
    0.09740120918,              0,              0 
  },
  {
                1,              0,             -1 
  },
  {
    0.09740120918,              0,              0 
  },
  {
                1,              0,             -1 
  },
  {
     0.0931205675,              0,              0 
  },
  {
                1,              0,             -1 
  },
  {
     0.0931205675,              0,              0 
  },
  {
                1,              0,             -1 
  },
  {
    0.09162836522,              0,              0 
  },
  {
                1,              0,             -1 
  },
  {
                1,              0,              0 
  }
};
const int DL_E[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1,3,1 };
const real32_T DEN_E[MWSPT_NSEC][3] = {
  {
                1,              0,              0 
  },
  {
                1,    -1.59540832,   0.9299485683 
  },
  {
                1,              0,              0 
  },
  {
                1,    -1.79179287,   0.9503180385 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.557225704,   0.8342912197 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.690221667,   0.8651767969 
  },
  {
                1,              0,              0 
  },
  {
                1,    -1.60010314,   0.8167432547 
  },
  {
                1,              0,              0 
  }
};
