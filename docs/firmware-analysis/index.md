# Firmware and Analysis

The CL02 repository includes STM32 firmware source, firmware image artifacts, and MATLAB tools for filter design and validation.

<div class="cl02-grid two">
  <a class="cl02-card cl02-card-link" href="firmware/">
    <h3>Firmware</h3>
    <p>STM32 source files, command handling, closed-loop logic, stimulation control, and prebuilt images.</p>
  </a>
  <a class="cl02-card cl02-card-link" href="matlab/">
    <h3>MATLAB tools</h3>
    <p>Filter design, coefficient generation, validation scripts, and file-control example workflows.</p>
  </a>
</div>

## Core Repository Areas

| Area | Purpose |
| --- | --- |
| `MCU/Src` | Firmware source files, including command, closed-loop, stimulation, data, USB, and UART modules. |
| `MCU/Inc` | Firmware headers and generated filter coefficient headers. |
| `MCU/MDK-ARM` | Keil project files and firmware image artifacts. |
| `Matlab` | Filter-design and validation scripts. |
| `Windows` | Control-center application and installer assets. |
