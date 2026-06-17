# Hardware

CL02 hardware exposes two analog inputs, one digital trigger output, one analog output, and visible state indicators for closed-loop state.

<div class="cl02-grid two">
  <a class="cl02-card cl02-card-link" href="connections-indicators/">
    <h3>Connections and indicators</h3>
    <p>IN0, IN1, DOUT, AOUT, and the Ready, Detect, and Trigger state indicators.</p>
  </a>
  <a class="cl02-card cl02-card-link" href="pcb/">
    <h3>PCB and assembly files</h3>
    <p>Public board files, fabrication outputs, and BOM-related files under the repository PCB directory.</p>
  </a>
</div>

<figure class="cl02-image-frame">
  <img src="../images/cl02-front-panel.png" alt="CL02 front panel connection labels and state indicators">
</figure>

## Hardware Asset Map

| Area | Repository path |
| --- | --- |
| V1 board files | [PCB/V1](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/PCB/V1) |
| V2 board files | [PCB/V2](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/PCB/V2) |
| V2 CAM outputs | [PCB/V2/CAMOutputs](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/PCB/V2/CAMOutputs) |
| Firmware images | [MCU/MDK-ARM](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/MCU/MDK-ARM) |
| Windows control software | [Windows](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/Windows) |

## Compatibility Record

For each CL02 build or dataset, record:

| Record | Why it matters |
| --- | --- |
| PCB revision | Distinguishes board files and connector details. |
| Firmware filename | Ties trigger and DSP behavior to a specific image. |
| Control-center installer | Defines UI and serial command behavior. |
| Input source | Identifies signal range, offset, and coupling assumptions. |
| Output destination | Defines DOUT or AOUT interpretation by downstream hardware. |
