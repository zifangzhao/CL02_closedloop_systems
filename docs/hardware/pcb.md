# PCB and Assembly Files

The repository contains CL02 board assets for multiple revisions.

## Public Board Files

| Revision | Assets |
| --- | --- |
| V1 | `PCB/V1/CL02.sch`, `PCB/V1/CL02.brd`, and `CL02_default_BOM_values.xlsx`. |
| V2 | `PCB/V2/CL02_V2.sch`, `PCB/V2/CL02_V2.brd`, project files, CAM outputs, and assembly files. |

## Fabrication Outputs

The V2 folder includes CAM outputs:

- `PCB/V2/CAMOutputs/GerberFiles`
- `PCB/V2/CAMOutputs/DrillFiles`
- `PCB/V2/CAMOutputs/Assembly`
- `PCB/V2/CAMOutputs/CL02_V2_BOM.xlsx`
- `PCB/V2/CAMOutputs/CL02_V2.zip`

## Documentation Practice

When documenting or reproducing a CL02 build, include the board revision, CAM output archive, BOM file, firmware image, and Windows control-center version. Do not infer connector behavior from board-file names alone; verify against the hardware labels and bench measurements.
