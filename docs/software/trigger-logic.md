# Trigger Logic

CL02 trigger behavior combines DSP output, baseline training, threshold logic, timing constraints, and output gating.

<figure class="cl02-image-frame">
  <img src="../../images/cl02-trigger-timing.png" alt="CL02 trigger timing diagram showing delay, random delay, pulse width, and interval">
</figure>

## Timing Controls

| Parameter | Meaning |
| --- | --- |
| Interval | Minimum interval between triggers, in milliseconds. |
| Pulse width | Width of each individual trigger pulse, in milliseconds. |
| Pulse cycles | Number of pulse cycles in the configured output event. |
| Fixed delay | Fixed delay added between trigger detection and DOUT assertion. |
| Max random delay | Maximum random delay added between detection and DOUT assertion. |
| Train start | Delay before closed-loop processor training begins. |
| Train duration | Baseline calculation window duration. |

## Detection Controls

| Parameter | Meaning |
| --- | --- |
| MA order | Moving-average window order. The original README notes sample units with `fs=1000`. |
| Filter | Built-in or custom filter used by the DSP path. |
| Trigger threshold | Absolute trigger threshold setting. |
| Trigger level | Threshold based on a multiple of the baseline level. |
| Formula | Input signal selected for DSP computation. |
| Random trigger range | Time range between triggers in random-trigger mode. |

## Recommended Validation

1. Disable trigger output while confirming signal preview.
2. Confirm the baseline state reaches Ready.
3. Enable output with conservative interval and pulse width.
4. Measure DOUT timing externally.
5. Document configured timing and measured timing.
