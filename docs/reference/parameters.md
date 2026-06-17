# Parameters

This table mirrors the public control-center parameters described in the original README and visible UI.

| Parameter | Meaning |
| --- | --- |
| Connect | Start communication with the connected CL02 device. |
| Disconnect | Disconnect the device; the original note states the device remains in its state. |
| Enable Trigger | Enable DOUT signaling. |
| External Trigger Override | Experimental file-based int32 override used to mask DOUT. |
| Interval | Minimum trigger interval, in milliseconds. |
| Pulse width | Width of each individual trigger pulse, in milliseconds. |
| Pulse cycles | Number of pulse cycles in a trigger event. |
| Fixed delay | Fixed delay between trigger detection and DOUT output. |
| Max random delay | Random delay range added between trigger detection and DOUT output. |
| Train start | Delay before closed-loop processor initiation after setting. |
| Train duration | Baseline calculation window duration. |
| MA order | Moving-average window order in samples; the original note references `fs=1000`. |
| Filter | DSP filter selection. |
| Custom filter | Operator-loaded filter file. |
| Trigger threshold | Absolute threshold setting. |
| Trigger level | Trigger threshold based on a multiple of baseline level. |
| Formula | Input signal selected for DSP computation. |
| Random trigger range | Time range between triggers in random-trigger mode. |
| Display time | Waveform preview time window. |
| Display gain | Waveform scaling for input and DSP preview. |
| DAC gain | Analog-output gain setting. |
| Remove DC for display | Display-side DC removal for waveform preview. |

## Experiment Record Minimum

At minimum, save the firmware image, installer version, PCB revision, input channel, output channel, filter, trigger threshold or trigger level, train start, train duration, interval, pulse width, delay settings, and whether external override was enabled.
