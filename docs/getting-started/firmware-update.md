# Firmware Update

CL02 firmware images are distributed under `MCU/MDK-ARM`. The original workflow uses ST DfuSe Demo.

<figure class="cl02-image-frame">
  <img src="../../images/cl02-dfu-demo.png" alt="DfuSe Demo firmware update screen with a CL02 DFU image loaded">
</figure>

## Update Procedure

1. Install ST DfuSe Demo from STMicroelectronics.
2. Power the CL02 device off.
3. Move the hardware switch under the USB connector to DFU mode. In the original hardware note, the side opposite the BNC connector is DFU mode.
4. Connect the CL02 device over USB.
5. Open DfuSe Demo.
6. Choose the intended `.dfu` firmware image from `MCU/MDK-ARM`.
7. Click upgrade.
8. Disconnect USB after the update completes.
9. Power the device off.
10. Move the switch back to Run mode. In the original hardware note, the side matching the BNC connector is Run mode.
11. Reconnect and validate with a bench test.

## Firmware Image Record

Record the exact firmware filename used. The repository contains multiple historical firmware images, and behavior may differ across versions.

| Record | Example source |
| --- | --- |
| Firmware image | `MCU/MDK-ARM/*.dfu` or checked `.hex` image |
| Control center installer | `Windows/*.exe` |
| PCB revision | `PCB/V1` or `PCB/V2` |
| Trigger settings | Saved experiment note or screenshot |
