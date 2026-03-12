# CL02 Closed-Loop System — Cross-Platform Control Center

A Python/PyQt6 replacement for the original Windows-only C# WinForms application.
Runs on **Linux, macOS, and Windows**.

## Quick Start

```bash
cd CrossPlatform/

# Install dependencies
pip install -r requirements.txt

# Run
python main.py
# or
python -m cl02_center
```

## Features

- **Real-time 4-channel waveform display** (CH1, CH2, DSP, DOUT) using pyqtgraph — significantly faster than the original GDI+ rendering
- **Full MCU command protocol** — identical packet framing and parameter serialization
- **Cross-platform serial port** auto-detection via pyserial
- **All original controls**: filter selection, DSP mode, trigger parameters, phase limits, custom filter loading, force trigger, DAC gain, etc.
- **Data logging** to binary `.dat` files (same format as the C# app)
- **DC removal filter** for display (same IIR coefficients)
- **NumPy-backed ring buffer** — efficient circular data storage

## Architecture

| File | Role |
|------|------|
| `ce_core.py` | Device parameter structs & binary serialization (replaces `CE_core.cs`) |
| `ce32_dsp.py` | IIR filter for display DC removal (replaces `CE32_DSP.cs`) |
| `data_source.py` | NumPy ring buffer (replaces `DataSource.cs`) |
| `serial_comm.py` | Serial port management, protocol framing, all commands (replaces serial logic in `Form1.cs`) |
| `main_window.py` | PyQt6 GUI with pyqtgraph plots (replaces `Form1.cs` + `Form1.Designer.cs`) |
| `main.py` | Entry point |

## Dependencies

- Python 3.10+
- PyQt6
- pyqtgraph
- pyserial
- numpy
