# Control Center

The CL02 control center is the main operator interface for connecting the device, viewing waveforms, and configuring closed-loop behavior.

<figure class="cl02-image-frame">
  <img src="../../images/cl02-control-center.png" alt="CL02 Windows control center">
</figure>

## Main Areas

| Area | Purpose |
| --- | --- |
| Waveform panels | Preview CH1, CH2, DSP output, and DOUT state. |
| COM port and connect buttons | Select the serial port, connect, disconnect, and optionally log data to file. |
| Trigger timing controls | Set interval, pulse width, pulse cycles, fixed delay, random delay, training delay, and training duration. |
| DSP controls | Select filter, custom filter, moving-average order, threshold, trigger level, and formula. |
| Manual actions | Restart DSP or force trigger for controlled bench validation. |
| Display controls | Select display time, input gain, DSP gain, DAC gain, and DC removal. |

## Connection Flow

1. Select the COM port.
2. Connect.
3. Review waveform preview.
4. Configure output timing and DSP settings.
5. Enable trigger only after bench validation.
6. Disconnect before rewiring hardware.

!!! tip "Record parameter values"
    A screenshot is useful, but the experiment note should include the actual trigger interval, pulse width, delay, filter, moving-average order, threshold, and firmware image.
