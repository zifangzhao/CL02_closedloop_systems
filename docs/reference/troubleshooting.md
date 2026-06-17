# Troubleshooting

## Device Does Not Connect

1. Confirm USB cable and power state.
2. Confirm the device appears as a COM port in Windows Device Manager.
3. Install `Windows/CL02_driver.inf` manually if Windows does not bind the driver.
4. Restart the control center and reselect the COM port.
5. Check that no other program is holding the same COM port.

## Firmware Update Does Not Start

1. Power the device off before switching modes.
2. Confirm the hardware switch is in DFU mode.
3. Reconnect USB after entering DFU mode.
4. Confirm DfuSe Demo sees the DFU device.
5. Choose a `.dfu` image from `MCU/MDK-ARM`.

## Trigger Does Not Appear on DOUT

1. Confirm DOUT is connected to bench measurement equipment.
2. Confirm trigger output is enabled.
3. Confirm the processor reaches Ready state.
4. Lower threshold only after validating that the input signal and baseline are stable.
5. Check interval, pulse width, fixed delay, and random delay.
6. Verify that external trigger override is not masking the output.

## External Override Does Not Work

1. Confirm external override is enabled in the control center.
2. Confirm the file path is `d:\cl02_control.bin`.
3. Confirm the file writer uses int32 format.
4. Confirm the workstation has a `D:` drive and write permission.
5. Validate DOUT masking on bench equipment.

## Filter Behavior Is Unexpected

1. Record the selected built-in filter or custom filter file.
2. Confirm moving-average order and formula settings.
3. Recreate the filter with the MATLAB script used to generate it.
4. Confirm firmware image and control-center version.
