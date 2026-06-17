# Filters

CL02 includes built-in filter coefficient headers and MATLAB tools for generating filter definitions.

## Built-In Filter Families

The firmware include tree contains filter headers for commonly used bands, including delta, theta, alpha, beta, gamma, high-gamma, epilepsy-band, sharp-wave, ripple, and ripple-1 style entries. The public headers include band-pass filters, low-pass filters, and Hilbert-transform filter variants at the CL02 DSP sample-rate workflow.

Examples from `MCU/Inc/CE32_filterDef.h` include:

| Family | Representative headers |
| --- | --- |
| Delta | `fdacoefs_BPF_D_1.5_5.5Hz@1000_ord1_SOS.h` |
| Theta | `fdacoefs_BPF_T_4_12Hz@1000_ord1_SOS.h` |
| Alpha | `fdacoefs_BPF_A_8_13Hz@1000_ord1_SOS.h` |
| Beta | `fdacoefs_BPF_B_13_30Hz@1000_ord1_SOS.h` |
| Gamma | `fdacoefs_BPF_G_30_80Hz@1000_ord1_SOS.h` |
| Ripple | `fdacoefs_BPF_R_110_250Hz@1000_ord1_SOS.h` |
| Ripple 1 | `fdacoefs_BPF_R1_100_200Hz@1000_ord1_SOS.h` |

## Custom Filters

The control center exposes custom filter loading. Keep custom filter files alongside the experiment record and document which DSP channel used each file.

## MATLAB Filter Tools

The `Matlab` folder contains CL02 and CE32 filter-design scripts, including `CL02_filterDesigner.m`, `CE32_filterDesigner.m`, and validation scripts. Use these scripts as the auditable source for generated coefficients rather than hand-editing coefficient headers.
