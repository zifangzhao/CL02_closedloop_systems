# CL02 Closed-Loop System

CL02 is a closed-loop electrophysiology trigger controller for detecting configured LFP oscillation features and driving timed stimulation outputs.

The project website is now built from the MkDocs source in [`docs/`](docs/) with configuration in [`mkdocs.yml`](mkdocs.yml). It documents:

- hardware connections and state indicators,
- Windows control-center setup and operation,
- trigger timing and external override behavior,
- firmware update workflow,
- firmware and MATLAB filter-design source locations.

## Local Documentation Preview

```powershell
pip install -r requirements.txt
mkdocs serve
```

For a strict build:

```powershell
mkdocs build --strict --verbose
```

## Repository Areas

| Area | Contents |
| --- | --- |
| `Windows` | CL02 control-center source, installers, and USB driver files. |
| `MCU` | STM32 firmware source, headers, filters, Keil project files, and firmware images. |
| `PCB` | CL02 board files, CAM outputs, assembly files, and BOM assets. |
| `Matlab` | Filter-design and validation scripts. |
| `docs` | MkDocs website source. |
