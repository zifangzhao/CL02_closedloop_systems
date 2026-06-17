# Getting Started

CL02 setup moves from software installation to a conservative bench test before use with experiment hardware.

<div class="cl02-grid two">
  <a class="cl02-card cl02-card-link" href="installation/">
    <h3>1. Install</h3>
    <p>Install the Windows control center, connect the USB device, and install the driver manually if Windows does not expose a usable COM port.</p>
  </a>
  <a class="cl02-card cl02-card-link" href="first-run/">
    <h3>2. First run</h3>
    <p>Select the COM port, connect, configure a known-safe trigger timing profile, and verify the waveform and output path.</p>
  </a>
  <a class="cl02-card cl02-card-link" href="firmware-update/">
    <h3>3. Firmware update</h3>
    <p>Use ST DfuSe Demo with the hardware DFU switch and a checked firmware image from the repository.</p>
  </a>
</div>

## Before You Begin

- Windows with permission to install desktop software and USB drivers.
- A CL02 device connected over USB.
- A downstream stimulator or measurement device that accepts 0-3.3 V logic for DOUT if DOUT is used.
- A bench measurement path for first validation, such as an oscilloscope or compatible data acquisition input.
- A documented firmware image and installer version for the device under test.

!!! warning "Bench-test first"
    Do not enable stimulation output in an animal experiment until DOUT or AOUT timing, voltage, polarity, and downstream-device behavior have been verified on the bench.

## Repository Asset Map

| Asset | Repository path |
| --- | --- |
| Windows installers | [Windows](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/Windows) |
| USB driver | [Windows/CL02_driver.inf](https://github.com/zifangzhao/CL02_closedloop_systems/blob/V2.2/Windows/CL02_driver.inf) |
| Firmware images | [MCU/MDK-ARM](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/MCU/MDK-ARM) |
| Firmware source | [MCU/Src](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/MCU/Src) and [MCU/Inc](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/MCU/Inc) |
| PCB files | [PCB](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/PCB) |
| MATLAB tools | [Matlab](https://github.com/zifangzhao/CL02_closedloop_systems/tree/V2.2/Matlab) |

## First Success Target

1. The CL02 device appears as a COM port.
2. The control center opens and connects.
3. Waveform preview updates for the configured input channel.
4. A conservative trigger profile produces the expected DOUT or AOUT behavior on bench equipment.
5. The operator records the installer file, firmware image, filter selection, threshold, trigger timing, and hardware cabling used.
