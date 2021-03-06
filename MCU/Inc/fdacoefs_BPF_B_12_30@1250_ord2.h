/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.6 and DSP System Toolbox 9.8.
 * Generated on: 14-Jun-2019 15:43:43
 */

/*
 * Discrete-Time IIR Filter (real)
 * -------------------------------
 * Filter Structure    : Direct-Form II
 * Numerator Length    : 3
 * Denominator Length  : 3
 * Stable              : Yes
 * Linear Phase        : No
 * Arithmetic          : single
 */

/* General type conversion for MATLAB generated C-code  */
#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * C:\Program Files\MATLAB\R2019a\extern\include\tmwtypes.h 
 */
const int NL_B = 3;
const real32_T NUM_B[3] = {
    0.04330921918,              0, -0.04330921918
};
const int DL_B = 3;
const real32_T DEN_B[3] = {
                1,   -1.904680371,   0.9133815765
};
