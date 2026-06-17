# Installation

The CL02 control workflow uses the Windows application distributed in the repository.

## Install the Windows Control Center

1. Open the repository `Windows` folder.
2. Choose the installer that matches the device and release you intend to use.
3. Run the installer.
4. If the installer or application is blocked by local security software, whitelist the known CL02 installer path or retry in a controlled setup environment.
5. Start the CL02 control center after installation.

## Connect the Device

1. Connect CL02 over USB.
2. Open Windows Device Manager and confirm that a COM port is present.
3. If Windows does not bind a useful COM port automatically, install `Windows/CL02_driver.inf` manually.
4. Restart the control center after driver installation.

!!! tip "Keep the exact installer name"
    The repository contains multiple installer versions. Record the file name used for each experiment or bench validation so the control-center behavior can be reproduced later.

## Driver Notes

The original CL02 instructions point to manual Windows driver installation when the device cannot connect. On modern Windows systems, this usually means selecting the CL02 `.inf` file from Device Manager and confirming the driver binding for the USB serial device.
