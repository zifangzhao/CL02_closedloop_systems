"""
main_window.py — Cross-platform GUI for the CL02 closed-loop system.

Hybrid UI strategy:
  • magic-class for control surfaces (Connection, Trigger/DSP, Display)
  • pyqtgraph for high-performance real-time plotting
"""

from __future__ import annotations

import math
import os
import struct
import time
import logging
from typing import Optional

import numpy as np
from magicclass import magicclass, field

from qtpy.QtCore import Qt, QTimer
from qtpy.QtWidgets import (
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QFileDialog,
    QMessageBox,
)
import pyqtgraph as pg

from .ce_core import DeviceConfig, DSP_COUNT
from .ce32_dsp import make_dc_removal_filter
from .data_source import DataSource
from .serial_comm import SerialManager, list_serial_ports, find_cl02_port, TRANS_CH

logger = logging.getLogger(__name__)

# ── Constants ────────────────────────────────────────────────────────────────

SAMPLE_RATE = 1000
CHANNEL_NAMES = ["CH1", "CH2", "DSP", "DOUT"]
N_DISPLAY_CH = 4

DISPLAY_LENGTHS = [1, 2, 5, 10]  # seconds
DISPLAY_LENGTH_LABELS = [f"{s}s" for s in DISPLAY_LENGTHS]

# Voltage gain look-up  (display_label → scale factor)
VOLTAGE_GAINS = [
    ("5V",    3.3 / 65536 / 5 * 2),
    ("1V",    3.3 / 65536 * 2),
    ("500mV", 3.3 / 65536 * 2 * 2),
    ("100mV", 3.3 / 65536 * 10 * 2),
    ("50mV",  3.3 / 65536 * 20 * 2),
    ("10mV",  3.3 / 65536 * 100 * 2),
    ("5mV",   3.3 / 65536 * 200 * 2),
    ("1mV",   3.3 / 65536 * 1000 * 2),
]
VOLTAGE_GAIN_LABELS = [label for label, _ in VOLTAGE_GAINS]

FILTER_TYPES = [
    "Delta\t(1–4)",
    "Theta\t(4–8)",
    "Alpha\t(8–12)",
    "Beta\t(13–30)",
    "Gamma\t(30–80)",
    "Epsilon\t(80–120)",
    "Ripple\t(110–250)",
    "IED\t(60–80)",
    "Ripple\t(100–200)",
    "SPW\t(8–40)",
    "Custom Filter 0",
    "Custom Filter 1",
]

FILTER_FREQS = [4, 8, 13, 30, 80, 110, 250, 60, 200, 40, 100]

DSP_MODES = [
    ("Disabled",             0),
    ("Single A",             1),
    ("Single A (Hilbert)",   6),
    ("Cascade A→B",          3),
    ("Gated A & B",          4),
    ("Random",               5),
]
DSP_MODE_LABELS = [label for label, _ in DSP_MODES]

FORMULAS = [
    "Direct (x=CH1)",
    "Direct (x=CH2)",
    "Differential (x=CH1−CH2)",
]

DOUT_SIGNALS = [
    ("Stimulator 1 (output)",  0x02),
    ("Stimulator 2 (output)",  0x08),
    ("Internal Trigger 1",     0x01),
    ("Internal Trigger 2",     0x04),
]
DOUT_SIGNAL_LABELS = [label for label, _ in DOUT_SIGNALS]

DSP_OFFSET = 0.4


@magicclass(name="Connection", widget_type="groupbox")
class ConnectionPanel:
    port = field(str, label="COM Port", options={"choices": ["<none>"]})
    log_data = field(bool, label="Log Data to File")
    log_file = field(str, label="Log File")

    def __init__(self):
        self._host: Optional["MainWindow"] = None
        self.log_file.value = ""

    def _bind_host(self, host: "MainWindow") -> None:
        self._host = host
        self["disconnect_device"].enabled = False

    def refresh_ports(self) -> None:
        if self._host is not None:
            self._host._refresh_ports()

    def connect_device(self) -> None:
        if self._host is not None:
            self._host._on_connect()

    def disconnect_device(self) -> None:
        if self._host is not None:
            self._host._on_disconnect()


@magicclass(name="Display", widget_type="groupbox")
class DisplayPanel:
    display_time = field(str, label="Display time", options={"choices": DISPLAY_LENGTH_LABELS})
    input_gain = field(str, label="Display gain (Input)", options={"choices": VOLTAGE_GAIN_LABELS})
    dsp_gain = field(str, label="Display gain (DSP)", options={"choices": VOLTAGE_GAIN_LABELS})
    dout_signal = field(str, label="Preview signal (DOUT)", options={"choices": DOUT_SIGNAL_LABELS})
    dac_gain = field(float, label="DAC Gain", options={"min": 0.1, "max": 1000.0, "step": 0.1})
    remove_dc = field(bool, label="Remove DC for display")

    def __init__(self):
        self._host: Optional["MainWindow"] = None
        self.display_time.value = DISPLAY_LENGTH_LABELS[-1]
        self.input_gain.value = VOLTAGE_GAIN_LABELS[0]
        self.dsp_gain.value = VOLTAGE_GAIN_LABELS[0]
        self.dout_signal.value = DOUT_SIGNAL_LABELS[0]
        self.dac_gain.value = 5.0
        self.remove_dc.value = True

    def _bind_host(self, host: "MainWindow") -> None:
        self._host = host
        self.dout_signal.changed.connect(lambda _v: self._host._on_dout_changed())
        self.dac_gain.changed.connect(lambda _v: self._host._on_dac_gain_changed())


@magicclass(name="Basic")
class TriggerBasicPanel:
    enable_trigger = field(bool, label="Enable Trigger")
    external_trigger_override = field(bool, label="External Trigger Override")
    dsp_mode = field(str, label="DSP mode", options={"choices": DSP_MODE_LABELS})
    dsp_id = field(int, label="DSP ID", options={"min": 0, "max": 1})

    interval_ms = field(float, label="Interval (ms)", options={"min": 0.0, "max": 65535.0, "step": 0.1})
    pulse_width_ms = field(float, label="Pulse Width (ms)", options={"min": 0.0, "max": 65535.0, "step": 0.1})
    pulse_cycles = field(int, label="Pulse Cycles", options={"min": 1, "max": 65535})

    filter_type = field(str, label="Filter", options={"choices": FILTER_TYPES})
    ma_order = field(int, label="MA Order", options={"min": 0, "max": 512})
    formula = field(str, label="Formula", options={"choices": FORMULAS})

    trigger_threshold = field(float, label="Trigger Threshold", options={"min": 0.1, "max": 65535.0, "step": 0.1})
    trigger_level_std = field(float, label="Trigger Level (× Std)", options={"min": 0.1, "max": 100.0, "step": 0.1})
    trigger_mode = field(str, label="Trigger Mode", options={"choices": ["First", "Last"]})
    abs_threshold = field(str, label="Absolute Threshold")


@magicclass(name="Advanced")
class TriggerAdvancedPanel:
    fixed_delay_ms = field(float, label="Fixed Delay (ms)", options={"min": 0.0, "max": 3000.0, "step": 0.1})
    max_rnd_delay_ms = field(float, label="Max Rnd Delay (ms)", options={"min": 0.0, "max": 3000.0, "step": 0.1})

    training_delay_s = field(int, label="Training Delay (s)", options={"min": 0, "max": 65535})
    training_duration_s = field(int, label="Training Duration (s)", options={"min": 0, "max": 65535})

    random_trigger_min_ms = field(float, label="Random Trigger Min (ms)", options={"min": 0.0, "max": 6553500.0, "step": 100.0})
    random_trigger_max_ms = field(float, label="Random Trigger Max (ms)", options={"min": 100.0, "max": 6553500.0, "step": 100.0})

    phase_lo_deg = field(float, label="Phase Low (deg)", options={"min": -180.0, "max": 180.0, "step": 0.1})
    phase_hi_deg = field(float, label="Phase High (deg)", options={"min": -180.0, "max": 180.0, "step": 0.1})

    custom_filter_status = field(str, label="Custom Filter Status")


@magicclass(name="Actions")
class TriggerActionsPanel:
    def __init__(self):
        self._host: Optional["MainWindow"] = None

    def _bind_host(self, host: "MainWindow") -> None:
        self._host = host

    def load_custom_filter_0(self) -> None:
        if self._host is not None:
            self._host._load_custom_filter(0)

    def load_custom_filter_1(self) -> None:
        if self._host is not None:
            self._host._load_custom_filter(1)

    def download_parameters(self) -> None:
        if self._host is not None:
            self._host._on_download_params()

    def force_trigger(self) -> None:
        if self._host is not None:
            self._host._on_force_trigger()


@magicclass(name="Trigger / DSP", widget_type="tabbed")
class TriggerDSPPanel:
    basic = TriggerBasicPanel
    advanced = TriggerAdvancedPanel
    actions = TriggerActionsPanel

    def __init__(self):
        self._host: Optional["MainWindow"] = None
        self._set_defaults()

    def _bind_host(self, host: "MainWindow") -> None:
        self._host = host
        self.actions._bind_host(host)
        self._wire_events()

    # Compatibility aliases for existing MainWindow logic
    @property
    def enable_trigger(self):
        return self.basic.enable_trigger

    @property
    def external_trigger_override(self):
        return self.basic.external_trigger_override

    @property
    def dsp_mode(self):
        return self.basic.dsp_mode

    @property
    def dsp_id(self):
        return self.basic.dsp_id

    @property
    def interval_ms(self):
        return self.basic.interval_ms

    @property
    def pulse_width_ms(self):
        return self.basic.pulse_width_ms

    @property
    def pulse_cycles(self):
        return self.basic.pulse_cycles

    @property
    def filter_type(self):
        return self.basic.filter_type

    @property
    def ma_order(self):
        return self.basic.ma_order

    @property
    def formula(self):
        return self.basic.formula

    @property
    def trigger_threshold(self):
        return self.basic.trigger_threshold

    @property
    def trigger_level_std(self):
        return self.basic.trigger_level_std

    @property
    def trigger_mode(self):
        return self.basic.trigger_mode

    @property
    def abs_threshold(self):
        return self.basic.abs_threshold

    @property
    def fixed_delay_ms(self):
        return self.advanced.fixed_delay_ms

    @property
    def max_rnd_delay_ms(self):
        return self.advanced.max_rnd_delay_ms

    @property
    def training_delay_s(self):
        return self.advanced.training_delay_s

    @property
    def training_duration_s(self):
        return self.advanced.training_duration_s

    @property
    def random_trigger_min_ms(self):
        return self.advanced.random_trigger_min_ms

    @property
    def random_trigger_max_ms(self):
        return self.advanced.random_trigger_max_ms

    @property
    def phase_lo_deg(self):
        return self.advanced.phase_lo_deg

    @property
    def phase_hi_deg(self):
        return self.advanced.phase_hi_deg

    @property
    def custom_filter_status(self):
        return self.advanced.custom_filter_status

    def _set_defaults(self) -> None:
        self.dsp_mode.value = DSP_MODES[1][0]
        self.interval_ms.value = 100.0
        self.filter_type.value = FILTER_TYPES[6]
        self.pulse_width_ms.value = 10.0
        self.ma_order.value = 50
        self.pulse_cycles.value = 1
        self.formula.value = FORMULAS[0]
        self.trigger_threshold.value = 1000.0
        self.trigger_level_std.value = 3.0
        self.trigger_mode.value = "First"

        self.fixed_delay_ms.value = 0.0
        self.max_rnd_delay_ms.value = 0.0
        self.training_delay_s.value = 0
        self.training_duration_s.value = 5
        self.random_trigger_min_ms.value = 500.0
        self.random_trigger_max_ms.value = 5000.0
        self.phase_lo_deg.value = -180.0
        self.phase_hi_deg.value = 180.0
        self.abs_threshold.value = "0"
        self.custom_filter_status.value = ""

    def _wire_events(self) -> None:
        if self._host is None:
            return

        self.enable_trigger.changed.connect(lambda v: self._host._on_enable_trigger(bool(v)))
        self.dsp_id.changed.connect(lambda _v: self._host._on_dsp_id_changed())

        self.interval_ms.changed.connect(lambda _v: self._host._on_stim_param_changed())
        self.pulse_width_ms.changed.connect(lambda _v: self._host._on_stim_param_changed())
        self.pulse_cycles.changed.connect(lambda _v: self._host._on_stim_param_changed())
        self.fixed_delay_ms.changed.connect(lambda _v: self._host._on_stim_param_changed())
        self.max_rnd_delay_ms.changed.connect(lambda _v: self._host._on_stim_param_changed())

        self.filter_type.changed.connect(
            lambda _v: self._host._on_filter_changed(self._choice_index(self.filter_type.value, FILTER_TYPES))
        )
        self.trigger_threshold.changed.connect(lambda _v: self._host._on_thresh_changed())
        self.trigger_level_std.changed.connect(lambda _v: self._host._on_gain_changed())
        self.trigger_mode.changed.connect(lambda _v: self._host._on_trig_mode_changed())
        self.phase_lo_deg.changed.connect(lambda _v: self._host._on_phase_changed())
        self.phase_hi_deg.changed.connect(lambda _v: self._host._on_phase_changed())

    @staticmethod
    def _choice_index(value: str, choices: list[str]) -> int:
        try:
            return choices.index(value)
        except ValueError:
            return 0

# ── Main window ──────────────────────────────────────────────────────────────

class MainWindow(QMainWindow):
    def __init__(
        self,
        trigger_panel: Optional[TriggerDSPPanel] = None,
        connection_panel: Optional[ConnectionPanel] = None,
        display_panel: Optional[DisplayPanel] = None,
    ):
        super().__init__()
        self.setWindowTitle("CL02 Closed-Loop System")
        self.setMinimumSize(1250, 822)

        # ── State ────────────────────────────────────────────────────────
        self.device = DeviceConfig(ch_num=2)
        self.serial = SerialManager()
        self.serial.on_data = self._on_serial_data

        self.data = DataSource(N_DISPLAY_CH, DISPLAY_LENGTHS[-1] * SAMPLE_RATE * 100, SAMPLE_RATE)
        self.dc_filters = [make_dc_removal_filter(), make_dc_removal_filter()]

        self.dsp_id_curr = 0
        self.dout_mask = DOUT_SIGNALS[0][1]
        self.thresh_value = 0
        self.custom_filter_data: list[Optional[bytes]] = [None, None]
        self.custom_filter_loaded = [False, False]
        self.log_file = None
        self.log_writer = None

        # magic-class panels (pre-created in entry points for stability)
        self.trigger_panel = trigger_panel if trigger_panel is not None else TriggerDSPPanel()
        self.connection_panel = connection_panel if connection_panel is not None else ConnectionPanel()
        self.display_panel = display_panel if display_panel is not None else DisplayPanel()

        self.trigger_panel._bind_host(self)
        self.connection_panel._bind_host(self)
        self.display_panel._bind_host(self)

        # ── Build UI ─────────────────────────────────────────────────────
        self._build_ui()
        self._init_defaults()

        # ── Repaint timer ────────────────────────────────────────────────
        self._paint_timer = QTimer(self)
        self._paint_timer.timeout.connect(self._repaint_plots)
        self._paint_timer.setInterval(50)  # 20 fps

    # =====================================================================
    #  UI Construction
    # =====================================================================

    def _build_ui(self) -> None:
        central = QWidget()
        self.setCentralWidget(central)
        root = QVBoxLayout(central)
        root.setContentsMargins(4, 4, 4, 4)

        # ── Top: 4 plot panels in 2×2 grid ───────────────────────────────
        plot_widget = pg.GraphicsLayoutWidget()
        plot_widget.setBackground("w")
        self.plots: list[pg.PlotItem] = []
        self.curves: list[pg.PlotDataItem] = []
        self.thresh_lines: list[Optional[pg.InfiniteLine]] = []

        for row in range(2):
            for col in range(2):
                idx = row * 2 + col
                p = plot_widget.addPlot(row=row, col=col, title=CHANNEL_NAMES[idx])
                p.showGrid(x=False, y=True, alpha=0.3)
                p.setLabel("left", "Amplitude")
                p.setMouseEnabled(x=False, y=False)
                p.enableAutoRange(axis="y")
                curve = p.plot(pen=pg.mkPen("b", width=1))
                self.plots.append(p)
                self.curves.append(curve)
                if idx == 2:
                    line = pg.InfiniteLine(angle=0, pen=pg.mkPen("r", width=1, style=Qt.PenStyle.DashLine))
                    line.setVisible(False)
                    p.addItem(line)
                    self.thresh_lines.append(line)
                else:
                    self.thresh_lines.append(None)

        root.addWidget(plot_widget, stretch=3)

        # ── Bottom: control panels ───────────────────────────────────────
        bottom = QHBoxLayout()
        bottom.addWidget(self.connection_panel.native, stretch=1)
        bottom.addWidget(self.trigger_panel.native, stretch=3)
        bottom.addWidget(self.display_panel.native, stretch=1)
        root.addLayout(bottom, stretch=1)

    # =====================================================================
    #  Defaults
    # =====================================================================

    def _init_defaults(self) -> None:
        self._refresh_ports()

    def _refresh_ports(self) -> None:
        ports = list_serial_ports()
        if not ports:
            ports = ["<none>"]

        self.connection_panel.port.choices = ports

        auto = find_cl02_port()
        if auto and auto in ports:
            self.connection_panel.port.value = auto
        elif self.connection_panel.port.value not in ports:
            self.connection_panel.port.value = ports[0]

    # =====================================================================
    #  Parameter helpers
    # =====================================================================

    def _get_display_length(self) -> int:
        value = self.display_panel.display_time.value
        try:
            idx = DISPLAY_LENGTH_LABELS.index(value)
            return DISPLAY_LENGTHS[idx]
        except ValueError:
            return DISPLAY_LENGTHS[-1]

    def _get_input_gain(self) -> float:
        label = self.display_panel.input_gain.value
        for k, v in VOLTAGE_GAINS:
            if k == label:
                return v
        return VOLTAGE_GAINS[0][1]

    def _get_dsp_gain(self) -> float:
        label = self.display_panel.dsp_gain.value
        for k, v in VOLTAGE_GAINS:
            if k == label:
                return v
        return VOLTAGE_GAINS[0][1]

    def _get_cl_mode(self) -> int:
        try:
            idx = DSP_MODE_LABELS.index(self.trigger_panel.dsp_mode.value)
        except ValueError:
            idx = 0
        return DSP_MODES[idx][1] if 0 <= idx < len(DSP_MODES) else 0

    def _ratio(self) -> int:
        return 10 * SAMPLE_RATE // 1000

    def _update_params(self) -> None:
        """Collect all GUI values into DeviceConfig (mirrors C# UpdParams)."""
        cl_mode = self._get_cl_mode()
        ratio = self._ratio()
        dsp_id = int(self.trigger_panel.dsp_id.value)

        common = dict(
            sampling_rate=1000,
            interval=int(self.trigger_panel.interval_ms.value * ratio),
            delay=int(self.trigger_panel.fixed_delay_ms.value * ratio),
            rnd_delay=int(self.trigger_panel.max_rnd_delay_ms.value * ratio),
            pw=int(self.trigger_panel.pulse_width_ms.value * ratio),
            cyc=int(self.trigger_panel.pulse_cycles.value),
            train_start=int(self.trigger_panel.training_delay_s.value),
            train_dur=int(self.trigger_panel.training_duration_s.value),
            mode=cl_mode,
            stim_on=int(bool(self.trigger_panel.enable_trigger.value)),
            rand_min=int(self.trigger_panel.random_trigger_min_ms.value),
            rand_max=int(self.trigger_panel.random_trigger_max_ms.value),
            param1=float(self.trigger_panel.phase_lo_deg.value / 180 * math.pi),
            param2=float(self.trigger_panel.phase_hi_deg.value / 180 * math.pi),
        )
        self.device.set_sys_params(0, gain=self.device.sys.trigger_gain[0], **common)
        self.device.set_sys_params(dsp_id, gain=float(self.trigger_panel.trigger_level_std.value), **common)

        try:
            formula_idx = FORMULAS.index(self.trigger_panel.formula.value)
        except ValueError:
            formula_idx = 0
        try:
            filter_idx = FILTER_TYPES.index(self.trigger_panel.filter_type.value)
        except ValueError:
            filter_idx = 0

        self.device.set_dsp_params(
            dsp_id,
            formula=formula_idx,
            func1=filter_idx,
            ma_ord=int(self.trigger_panel.ma_order.value),
        )

    # =====================================================================
    #  Serial data callback (runs in reader thread)
    # =====================================================================

    def _on_serial_data(self, raw: bytes, thresholds: list[int]) -> None:
        n_samples = len(raw) // (2 * TRANS_CH)
        if n_samples == 0:
            return

        input_gain = self._get_input_gain()
        dsp_gain = self._get_dsp_gain()
        dsp_ch = int(self.trigger_panel.dsp_id.value) + 4
        remove_dc = bool(self.display_panel.remove_dc.value)

        out = np.zeros((N_DISPLAY_CH, n_samples), dtype=np.float64)

        for s in range(n_samples):
            base = s * 2 * TRANS_CH

            v = struct.unpack_from("<h", raw, base + 0)[0] * input_gain
            if remove_dc:
                v = self.dc_filters[0].process(v)
            out[0, s] = v

            v = struct.unpack_from("<h", raw, base + 2)[0] * input_gain
            if remove_dc:
                v = self.dc_filters[1].process(v)
            out[1, s] = v

            v = struct.unpack_from("<h", raw, base + 2 * dsp_ch)[0] * dsp_gain
            out[2, s] = v - DSP_OFFSET

            digi = struct.unpack_from("<H", raw, base + 2 * 6)[0]
            bit = digi & self.dout_mask
            out[3, s] = (1.8 if bit else 0.0) - 0.9

        self.data.append(out)

        if self.log_writer:
            try:
                self.log_writer.write(raw[:n_samples * 2 * TRANS_CH])
            except Exception:
                pass

        self.thresh_value = thresholds[int(self.trigger_panel.dsp_id.value)]

    # =====================================================================
    #  Plot repaint
    # =====================================================================

    def _repaint_plots(self) -> None:
        disp_len = self._get_display_length()
        n = disp_len * SAMPLE_RATE
        block = self.data.get_latest(n)
        if block.shape[1] == 0:
            return

        x = np.arange(block.shape[1]) / SAMPLE_RATE

        for i in range(N_DISPLAY_CH):
            self.curves[i].setData(x, block[i])

        line = self.thresh_lines[2]
        if line is not None:
            dsp_gain_val = self._get_dsp_gain()
            dac_gain_val = self.display_panel.dac_gain.value
            t = self.thresh_value * dac_gain_val * dsp_gain_val - DSP_OFFSET
            line.setValue(t)
            line.setVisible(True)

        self.trigger_panel.abs_threshold.value = str(self.thresh_value)

    # =====================================================================
    #  Connection handlers
    # =====================================================================

    def _on_connect(self) -> None:
        port = self.connection_panel.port.value
        if not port or port == "<none>":
            QMessageBox.warning(self, "Error", "No COM port selected")
            return

        if bool(self.connection_panel.log_data.value):
            path, _ = QFileDialog.getSaveFileName(self, "Save Data", "", "Binary File (*.dat)")
            if path:
                self.connection_panel.log_file.value = path
                self.log_file = open(path, "wb")
                self.log_writer = self.log_file
            else:
                return

        try:
            self.serial.open(port)
        except Exception as e:
            QMessageBox.warning(self, "Error", f"Failed to open {port}:\n{e}")
            return

        self._send_all_settings()

        self.serial.start_preview()
        self.serial.start_reader()
        self._paint_timer.start()

        self.connection_panel["connect_device"].enabled = False
        self.connection_panel["disconnect_device"].enabled = True

    def _on_disconnect(self) -> None:
        self._paint_timer.stop()
        self.serial.stop_preview()
        self.serial.close()

        if self.log_file:
            self.log_file.close()
            self.log_file = None
            self.log_writer = None

        self.connection_panel["connect_device"].enabled = True
        self.connection_panel["disconnect_device"].enabled = False

    # =====================================================================
    #  Settings senders
    # =====================================================================

    def _send_all_settings(self) -> None:
        self._update_params()
        self.serial.send_sys_settings(self.device)
        self._send_custom_filter(0)
        self._send_custom_filter(1)
        self.serial.send_dsp_settings(self.device, 0)
        self._send_dsp_param()
        self._send_stim_param()
        self._send_dac_gain()
        self._send_trig_mode()

    def _send_stim_param(self) -> None:
        self._update_params()
        for dsp_id in range(DSP_COUNT):
            self.serial.set_stim_param(
                dsp_id,
                self.device.sys.stim_delay[dsp_id],
                self.device.sys.stim_rnd_delay[dsp_id],
                self.device.sys.pulse_width[dsp_id],
                self.device.sys.stim_interval[dsp_id],
                self.device.sys.pulse_cyc[dsp_id],
            )
            time.sleep(0.05)

    def _send_dsp_param(self) -> None:
        self._update_params()
        for dsp_id in range(DSP_COUNT):
            ft = self.device.func[dsp_id]
            if self.device.sys.cl_mode > 5:
                ft += 24
            self.serial.set_dsp_param(dsp_id, self.device.ma_ord[dsp_id], ft, self.device.formula[dsp_id])
            time.sleep(0.05)

    def _send_cl_params(self) -> None:
        self._update_params()
        self.serial.set_cl_params(self.device.sys.cl_param1, self.device.sys.cl_param2)

    def _send_custom_filter(self, fid: int) -> None:
        data = self.custom_filter_data[fid]
        if data:
            self.serial.send_custom_filter(fid, data)

    def _send_dac_gain(self) -> None:
        self.serial.set_dac_gain(int(self.display_panel.dac_gain.value))

    def _send_trig_mode(self) -> None:
        mode = self.trigger_panel.trigger_mode.value
        self.serial.set_trig_mode(0 if mode == "First" else 1)

    # =====================================================================
    #  UI event handlers
    # =====================================================================

    def _on_enable_trigger(self, checked: bool) -> None:
        self.serial.set_stim(checked)

    def _on_stim_param_changed(self) -> None:
        if self.trigger_panel.pulse_width_ms.value > self.trigger_panel.interval_ms.value:
            self.trigger_panel.interval_ms.value = self.trigger_panel.pulse_width_ms.value
        self._send_stim_param()

    def _on_gain_changed(self) -> None:
        self.serial.set_gain(int(self.trigger_panel.dsp_id.value), float(self.trigger_panel.trigger_level_std.value))

    def _on_thresh_changed(self) -> None:
        self.serial.set_gain_abs(int(self.trigger_panel.dsp_id.value), float(self.trigger_panel.trigger_threshold.value))

    def _on_filter_changed(self, idx: int) -> None:
        if idx < len(FILTER_FREQS):
            v = int(1.0 / FILTER_FREQS[idx] * SAMPLE_RATE * 3)
            v = min(v, int(self.trigger_panel.ma_order.max))
            self.trigger_panel.ma_order.value = v

        n_custom = 2
        n_builtin = len(FILTER_TYPES) - n_custom
        if idx == n_builtin:
            if not self.custom_filter_loaded[0]:
                self._load_custom_filter(0)
        elif idx == n_builtin + 1:
            if not self.custom_filter_loaded[1]:
                self._load_custom_filter(1)

    def _on_dsp_id_changed(self) -> None:
        self._update_params()
        self.dsp_id_curr = int(self.trigger_panel.dsp_id.value)
        self._load_params_to_ui()

    def _on_download_params(self) -> None:
        self._send_all_settings()

    def _on_force_trigger(self) -> None:
        self.serial.force_trigger()

    def _on_trig_mode_changed(self) -> None:
        self._send_trig_mode()

    def _on_dac_gain_changed(self) -> None:
        self._send_dac_gain()

    def _on_dout_changed(self) -> None:
        label = self.display_panel.dout_signal.value
        for k, v in DOUT_SIGNALS:
            if k == label:
                self.dout_mask = v
                return

    def _on_phase_changed(self) -> None:
        self._update_params()
        self._send_cl_params()

    def _load_custom_filter(self, fid: int) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self,
            "Load Custom Filter",
            "",
            "Filter Parameter (*.filter);;All Files (*)",
        )
        if path and os.path.isfile(path):
            with open(path, "rb") as f:
                self.custom_filter_data[fid] = f.read(512)
            self.custom_filter_loaded[fid] = True
            self.trigger_panel.custom_filter_status.value = path
            self._send_custom_filter(fid)

    def _load_params_to_ui(self) -> None:
        d = self.device
        sid = self.dsp_id_curr
        ratio = self._ratio()

        idx = max(0, d.sys.cl_mode)
        mode_values = [v for _, v in DSP_MODES]
        if idx in mode_values:
            self.trigger_panel.dsp_mode.value = DSP_MODES[mode_values.index(idx)][0]

        self.trigger_panel.interval_ms.value = d.sys.stim_interval[0] / ratio if ratio else 0
        self.trigger_panel.fixed_delay_ms.value = d.sys.stim_delay[0] / ratio if ratio else 0
        self.trigger_panel.max_rnd_delay_ms.value = d.sys.stim_rnd_delay[0] / ratio if ratio else 0
        self.trigger_panel.pulse_width_ms.value = d.sys.pulse_width[0] / ratio if ratio else 0
        self.trigger_panel.pulse_cycles.value = d.sys.pulse_cyc[0]
        self.trigger_panel.trigger_level_std.value = d.sys.trigger_gain[sid]

        if 0 <= d.formula[sid] < len(FORMULAS):
            self.trigger_panel.formula.value = FORMULAS[d.formula[sid]]
        if 0 <= d.func[sid] < len(FILTER_TYPES):
            self.trigger_panel.filter_type.value = FILTER_TYPES[d.func[sid]]

        self.trigger_panel.ma_order.value = d.ma_ord[sid]
        self.trigger_panel.phase_lo_deg.value = d.sys.cl_param1[sid] / math.pi * 180
        self.trigger_panel.phase_hi_deg.value = d.sys.cl_param2[sid] / math.pi * 180

    # =====================================================================
    #  Cleanup
    # =====================================================================

    def closeEvent(self, event) -> None:
        self._on_disconnect()
        super().closeEvent(event)
