# CL02 Instructions

Project: Ephys
Tags: Device

[https://github.com/zifangzhao/CL02_closedloop_systems.git](https://github.com/zifangzhao/CL02_closedloop_systems.git)

# Introduction

CL02 system is a closed-loop system that be used for detect LFP oscillations and trigger stimulations.

## Hardware

### Connection

1. IN0: analog input 0 (0-3.3V, can be configured with +1.5V offset)
2. IN1:analog input 1 (0-3.3V, can be configured with +1.5V offset)
3. DOUT: digital trigger output (0-3.3V)
4. AOUT: analog output(0-3.3V)

### State indicators

1. Ready: Closed-loop processor state (Slow blink: Waiting; Fast blink: estimating baseline; On: Ready)
2. Detect: Indicate a trigger is armed,(actually DOUT trigger depends on the stimulator setting)
3. Trigger: Indicate DOUT states

![CL02 system ](CL02%20Instructions%2031aee3053eb14f1c8ba4a66c70837c62/Untitled.png)

CL02 system 

## Software

A windows program is provided to control the device.

1. Connect: Start communication with a connected device.
2. Disconnect: Disconnect device(Device remains its states)
3. Enable Trigger: Enable DOUT signaling
4. External Trigger Override(experimental): Read a int32 control signal(0/1) from a external file to mask the DOUT. (currently fixed at d:\cl02_control.bin)
    
    Here is an example matlab script that control the device.
    
    [cl02_filecontrol_test.m](CL02%20Instructions%2031aee3053eb14f1c8ba4a66c70837c62/cl02_filecontrol_test.m)
    
5. Interval: Minimal trigger interval(ms)
6. Pulse width: Pulse width of each individual trigger(ms)
7. Fixed delay: A fixed delay added between trigger and actually DOUT signal
8. Max random delay: Range of random delay added between trigger and DOUT
9. Train start: Delay time of the closed-loop processor initiation after setting.
10. Train duration: Duration of the baseline calculation window.
11. MA order: order of the moving average window(samples, fs=1000)
12. Filter: Select a filter for the processor
13. Trigger threshold: set threshold of the trigger.
14. Trigger level: set threshold of the trigger based on times of baseline level.
15. Formula: select input signal of the DSP
16. Random trigger range: set time range between triggers in the random trigger mode

![Control interface](CL02%20Instructions%2031aee3053eb14f1c8ba4a66c70837c62/Untitled%201.png)

Control interface

![Trigger time logic](CL02%20Instructions%2031aee3053eb14f1c8ba4a66c70837c62/Untitled%202.png)

Trigger time logic

# Installation

1. Get latest files from: [https://github.com/zifangzhao/CL02_closedloop_systems.git](https://github.com/zifangzhao/CL02_closedloop_systems.git)
2. Install Setup.exe under \windows 
3. Disable Anti-virus software if installation fails or program not running
4. Connect device with USB, if driver not found or device cannot connect, manually install driver CL02_driver.inf , instruction: [https://answers.microsoft.com/en-us/windows/forum/all/how-to-install-and-update-hardware-drivers-in/a97bbbd1-9973-4d66-9a5b-291300006293](https://answers.microsoft.com/en-us/windows/forum/all/how-to-install-and-update-hardware-drivers-in/a97bbbd1-9973-4d66-9a5b-291300006293)

# Firmware Update

1. Install DFU software from:  [https://www.st.com/en/development-tools/stsw-stm32080.html](https://www.st.com/en/development-tools/stsw-stm32080.html)
2. Open Dfuse Demo
3. Switch device to DFU mode(Switch is under the USB connector, Opposite of BNC connector side is DFU mode, device must be powered-off)
4. Connect device with USB
5. Choose the firmware file(*.dfu), latest version is under folder **CL02_opensource\MCU\MDK-ARM**
6. Click Upgrade
7. Disconnect device with USB
8. Switch device back to Run mode  (Switch is under the USB connector, Same as BNC connector side is Run mode, device must be powered-off)

![Untitled](CL02%20Instructions%2031aee3053eb14f1c8ba4a66c70837c62/Untitled%203.png)