# First Run

Use a conservative bench run to confirm that the control center, serial connection, DSP settings, and output timing all behave as expected.

## Connect

1. Launch the CL02 control center.
2. Select the COM port for the connected CL02 device.
3. Click <span class="cl02-button-label primary">Connect</span>.
4. Confirm that waveform panels update.

## Configure a Safe Trigger Profile

Start with settings that are easy to verify on a scope or downstream acquisition system:

| Setting | First-run guidance |
| --- | --- |
| Enable Trigger | Leave disabled until the input and timing settings are reviewed. |
| Interval | Use a long interval for early bench testing. |
| Pulse width | Use a pulse width accepted by the downstream device. |
| Fixed delay | Start at `0 ms` unless the experiment requires deterministic delay. |
| Max random delay | Start at `0 ms` unless validating random-delay behavior. |
| Training delay | Leave enough time to observe waiting and baseline states. |
| Training duration | Use a short but stable baseline window for the bench signal. |
| Filter | Choose the frequency band that matches the bench signal. |
| Trigger threshold or level | Start high enough to avoid accidental triggering. |

## Verify Output

1. Confirm Ready state transitions from waiting to baseline estimation to ready.
2. Enable trigger output only after the input signal is visible and the threshold is reviewed.
3. Observe DOUT or AOUT with bench equipment.
4. Use <span class="cl02-button-label">Force Trigger</span> only when the output path is safe to test.
5. Document any mismatch between configured timing and measured output timing.

!!! warning "Downstream stimulation safety"
    CL02 can assert trigger outputs. Confirm downstream stimulator enable state, pulse interpretation, voltage compatibility, and isolation before connecting stimulation hardware.
