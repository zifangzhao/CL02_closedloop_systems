# Firmware

CL02 firmware source is organized under `MCU`.

## High-Signal Source Files

| File | Role |
| --- | --- |
| `MCU/Src/CE32_ClosedLoop.c` | Closed-loop modes, baseline training, threshold checks, random-trigger update, and trigger actions. |
| `MCU/Src/CE32_StimControl.c` | Stimulation-control behavior and output timing support. |
| `MCU/Src/CE32_COMMAND.c` | Command handling for host-device communication. |
| `MCU/Src/CL02_Server.c` | CL02 server-side firmware behavior. |
| `MCU/Inc/CE32_filterDef.h` | Filter coefficient include map. |
| `MCU/Inc/CE32_ClosedLoop.h` | Closed-loop type and interface definitions. |

## Firmware Images

Prebuilt images and historical images are under `MCU/MDK-ARM`. Record the exact image filename used in each run.

## Firmware Behavior Notes

The firmware source includes closed-loop modes such as disabled, single, double, cascade, gated, random, and Hilbert-transform variants. Public experiment notes should state the mode and filter path used rather than relying only on a UI screenshot.
