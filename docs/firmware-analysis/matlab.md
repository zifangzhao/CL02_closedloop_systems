# MATLAB Tools

The `Matlab` folder contains scripts for filter generation, validation, and helper workflows.

## Key Files

| File | Role |
| --- | --- |
| `CL02_filterDesigner.m` | CL02 filter design entry point. |
| `CL02_genFilter_withData.m` | Filter generation using data-driven workflow. |
| `script_validate_CL02_filterDesigner.m` | Minimal validation entry for CL02 filter design. |
| `CE32_filterDesigner.m` | Shared CE32 filter design helper. |
| `CE32_filterGen_SOS.m` | Second-order-section coefficient generation. |
| `CE32_filterDef.h` | Generated or source filter definition map used by firmware-side includes. |
| `readmulti_frank.m` | MATLAB data helper included in the repository. |

## Validation Example

The repository validation script constructs a filter and opens MATLAB visualization:

```matlab
Hd=CE32_filterDesigner(100,1000,2);
fvtool(Hd,'fs',1000);
```

## Documentation Practice

When custom filters are used, preserve:

1. The MATLAB script version.
2. The input parameters.
3. The generated coefficient file.
4. The firmware image that consumed the coefficients.
5. The control-center screenshot or parameter export for the run.
