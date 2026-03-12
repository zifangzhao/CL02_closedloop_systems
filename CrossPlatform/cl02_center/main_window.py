"""
main_window.py — Cross-platform GUI for the CL02 closed-loop system.

Replaces the C# WinForms Form1 with a PyQt6 + pyqtgraph application that
reproduces the exact same layout, controls, and serial protocol behaviour.

Layout (top-to-bottom):
    ┌──────────────────────────────────────────────────────┐
    │  4 real-time plots (CH1, CH2, DSP, DOUT) — 2×2 grid │
    ├────────────┬─────────────────────────┬───────────────┤
    │ Connection │  Trigger / DSP controls │ Display opts  │
    └────────────┴─────────────────────────┴───────────────┘
"""

from __future__ import annotations

import math
import os
import struct
import time
import logging
from pathlib import Path
from typing import Optional

import numpy as np

from PyQt6.QtCore import Qt, QTimer
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QGridLayout, QGroupBox, QLabel, QComboBox, QCheckBox, QPushButton,
    QDoubleSpinBox, QSpinBox, QFileDialog, QMessageBox, QSplitter,
    QSizePolicy,
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

DSP_OFFSET = 0.4


# ── Main window ──────────────────────────────────────────────────────────────

class MainWindow(QMainWindow):
    def __init__(self):
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
                # Threshold line for DSP channel
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
        bottom.addWidget(self._build_connection_panel(), stretch=1)
        bottom.addWidget(self._build_trigger_panel(), stretch=3)
        bottom.addWidget(self._build_display_panel(), stretch=1)
        root.addLayout(bottom, stretch=1)

    # ── Connection panel ─────────────────────────────────────────────────

    def _build_connection_panel(self) -> QGroupBox:
        grp = QGroupBox("Connection")
        lay = QVBoxLayout(grp)

        h = QHBoxLayout()
        h.addWidget(QLabel("COM Port:"))
        self.combo_port = QComboBox()
        self.combo_port.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        self.combo_port.installEventFilter(self)
        h.addWidget(self.combo_port)
        lay.addLayout(h)

        self.btn_connect = QPushButton("Connect")
        self.btn_connect.clicked.connect(self._on_connect)
        lay.addWidget(self.btn_connect)

        self.btn_disconnect = QPushButton("Disconnect")
        self.btn_disconnect.setEnabled(False)
        self.btn_disconnect.clicked.connect(self._on_disconnect)
        lay.addWidget(self.btn_disconnect)

        self.cb_log = QCheckBox("Log Data to File")
        lay.addWidget(self.cb_log)

        self.lbl_file = QLabel("")
        self.lbl_file.setWordWrap(True)
        lay.addWidget(self.lbl_file)

        lay.addStretch()
        return grp

    # ── Trigger/DSP panel ────────────────────────────────────────────────

    def _build_trigger_panel(self) -> QGroupBox:
        grp = QGroupBox("Trigger / DSP")
        lay = QGridLayout(grp)
        row = 0

        # Row 0: Enable + Ext override + DSP mode + DSP ID
        self.cb_enable = QCheckBox("Enable Trigger")
        self.cb_enable.toggled.connect(self._on_enable_trigger)
        lay.addWidget(self.cb_enable, row, 0)

        self.cb_ext_override = QCheckBox("External Trigger Override")
        lay.addWidget(self.cb_ext_override, row, 1)

        lay.addWidget(QLabel("DSP mode:"), row, 2)
        self.combo_dsp_mode = QComboBox()
        for label, _ in DSP_MODES:
            self.combo_dsp_mode.addItem(label)
        self.combo_dsp_mode.setCurrentIndex(1)
        lay.addWidget(self.combo_dsp_mode, row, 3)

        lay.addWidget(QLabel("DSP ID:"), row, 4)
        self.spin_dsp_id = QSpinBox()
        self.spin_dsp_id.setRange(0, 1)
        self.spin_dsp_id.valueChanged.connect(self._on_dsp_id_changed)
        lay.addWidget(self.spin_dsp_id, row, 5)

        # Row 1: Interval / Pulse width
        row += 1
        lay.addWidget(QLabel("Interval (ms):"), row, 0)
        self.spin_interval = QDoubleSpinBox()
        self.spin_interval.setRange(0, 65535)
        self.spin_interval.setDecimals(1)
        self.spin_interval.setValue(100)
        self.spin_interval.valueChanged.connect(self._on_stim_param_changed)
        lay.addWidget(self.spin_interval, row, 1)

        lay.addWidget(QLabel("Filter:"), row, 2)
        self.combo_filter = QComboBox()
        for f in FILTER_TYPES:
            self.combo_filter.addItem(f)
        self.combo_filter.setCurrentIndex(6)
        self.combo_filter.currentIndexChanged.connect(self._on_filter_changed)
        lay.addWidget(self.combo_filter, row, 3)

        # Row 2: Pulse width / MA Order
        row += 1
        lay.addWidget(QLabel("Pulse Width (ms):"), row, 0)
        self.spin_pw = QDoubleSpinBox()
        self.spin_pw.setRange(0, 65535)
        self.spin_pw.setDecimals(1)
        self.spin_pw.setValue(10)
        self.spin_pw.valueChanged.connect(self._on_stim_param_changed)
        lay.addWidget(self.spin_pw, row, 1)

        lay.addWidget(QLabel("MA Order:"), row, 2)
        self.spin_ma = QSpinBox()
        self.spin_ma.setRange(0, 512)
        self.spin_ma.setValue(50)
        lay.addWidget(self.spin_ma, row, 3)

        # Row 3: Pulse Cycles / Custom filter
        row += 1
        lay.addWidget(QLabel("Pulse Cycles:"), row, 0)
        self.spin_cyc = QSpinBox()
        self.spin_cyc.setRange(1, 65535)
        self.spin_cyc.setValue(1)
        self.spin_cyc.valueChanged.connect(self._on_stim_param_changed)
        lay.addWidget(self.spin_cyc, row, 1)

        self.btn_custom0 = QPushButton("Load Custom Filter 0")
        self.btn_custom0.clicked.connect(lambda: self._load_custom_filter(0))
        lay.addWidget(self.btn_custom0, row, 2)
        self.btn_custom1 = QPushButton("Load Custom Filter 1")
        self.btn_custom1.clicked.connect(lambda: self._load_custom_filter(1))
        lay.addWidget(self.btn_custom1, row, 3)

        # Row 4: Fixed delay / Formula
        row += 1
        lay.addWidget(QLabel("Fixed Delay (ms):"), row, 0)
        self.spin_delay = QDoubleSpinBox()
        self.spin_delay.setRange(0, 3000)
        self.spin_delay.setDecimals(1)
        self.spin_delay.valueChanged.connect(self._on_stim_param_changed)
        lay.addWidget(self.spin_delay, row, 1)

        lay.addWidget(QLabel("Formula:"), row, 2)
        self.combo_formula = QComboBox()
        for f in FORMULAS:
            self.combo_formula.addItem(f)
        lay.addWidget(self.combo_formula, row, 3)

        # Row 5: Random delay / Trigger threshold
        row += 1
        lay.addWidget(QLabel("Max Rnd Delay (ms):"), row, 0)
        self.spin_rnd_delay = QDoubleSpinBox()
        self.spin_rnd_delay.setRange(0, 3000)
        self.spin_rnd_delay.setDecimals(1)
        self.spin_rnd_delay.valueChanged.connect(self._on_stim_param_changed)
        lay.addWidget(self.spin_rnd_delay, row, 1)

        lay.addWidget(QLabel("Trigger Threshold:"), row, 2)
        self.spin_thresh = QDoubleSpinBox()
        self.spin_thresh.setRange(0.1, 65535)
        self.spin_thresh.setDecimals(1)
        self.spin_thresh.setValue(1000)
        self.spin_thresh.valueChanged.connect(self._on_thresh_changed)
        lay.addWidget(self.spin_thresh, row, 3)

        self.lbl_abs_thresh = QLabel("")
        lay.addWidget(self.lbl_abs_thresh, row, 4)

        # Row 6: Training / Trigger level
        row += 1
        lay.addWidget(QLabel("Training Delay (s):"), row, 0)
        self.spin_train_start = QSpinBox()
        self.spin_train_start.setRange(0, 65535)
        lay.addWidget(self.spin_train_start, row, 1)

        lay.addWidget(QLabel("Trigger Level:"), row, 2)
        self.spin_gain = QDoubleSpinBox()
        self.spin_gain.setRange(0.1, 100)
        self.spin_gain.setDecimals(1)
        self.spin_gain.setValue(3.0)
        self.spin_gain.setSingleStep(0.1)
        self.spin_gain.valueChanged.connect(self._on_gain_changed)
        lay.addWidget(self.spin_gain, row, 3)
        lay.addWidget(QLabel("× Std"), row, 4)

        # Row 7: Training duration / Trigger mode
        row += 1
        lay.addWidget(QLabel("Training Duration (s):"), row, 0)
        self.spin_train_dur = QSpinBox()
        self.spin_train_dur.setRange(0, 65535)
        self.spin_train_dur.setValue(5)
        lay.addWidget(self.spin_train_dur, row, 1)

        lay.addWidget(QLabel("Trigger Mode:"), row, 2)
        self.combo_trig_mode = QComboBox()
        self.combo_trig_mode.addItems(["First", "Last"])
        self.combo_trig_mode.currentIndexChanged.connect(self._on_trig_mode_changed)
        lay.addWidget(self.combo_trig_mode, row, 3)

        # Row 8: Random trigger range
        row += 1
        lay.addWidget(QLabel("Random Trigger Range (ms):"), row, 0)
        h_rand = QHBoxLayout()
        self.spin_rand_min = QDoubleSpinBox()
        self.spin_rand_min.setRange(0, 6553500)
        self.spin_rand_min.setDecimals(1)
        self.spin_rand_min.setSingleStep(100)
        self.spin_rand_min.setValue(500)
        h_rand.addWidget(self.spin_rand_min)
        h_rand.addWidget(QLabel("–"))
        self.spin_rand_max = QDoubleSpinBox()
        self.spin_rand_max.setRange(100, 6553500)
        self.spin_rand_max.setDecimals(1)
        self.spin_rand_max.setSingleStep(100)
        self.spin_rand_max.setValue(5000)
        h_rand.addWidget(self.spin_rand_max)
        lay.addLayout(h_rand, row, 1, 1, 2)

        # Row 9: Phase params + buttons
        row += 1
        lay.addWidget(QLabel("Phase degrees (HT mode):"), row, 0)
        h_phase = QHBoxLayout()
        self.spin_phase_lo = QDoubleSpinBox()
        self.spin_phase_lo.setRange(-180, 180)
        self.spin_phase_lo.setDecimals(1)
        self.spin_phase_lo.setValue(-180)
        self.spin_phase_lo.valueChanged.connect(self._on_phase_changed)
        h_phase.addWidget(self.spin_phase_lo)
        self.spin_phase_hi = QDoubleSpinBox()
        self.spin_phase_hi.setRange(-180, 180)
        self.spin_phase_hi.setDecimals(1)
        self.spin_phase_hi.setValue(180)
        self.spin_phase_hi.valueChanged.connect(self._on_phase_changed)
        h_phase.addWidget(self.spin_phase_hi)
        lay.addLayout(h_phase, row, 1, 1, 2)

        self.btn_download = QPushButton("Download Parameters")
        self.btn_download.clicked.connect(self._on_download_params)
        lay.addWidget(self.btn_download, row, 3)

        self.btn_force = QPushButton("Force Trigger")
        self.btn_force.clicked.connect(self._on_force_trigger)
        lay.addWidget(self.btn_force, row, 4)

        self.lbl_custom_filter = QLabel("")
        lay.addWidget(self.lbl_custom_filter, row + 1, 0, 1, 4)

        return grp

    # ── Display panel ────────────────────────────────────────────────────

    def _build_display_panel(self) -> QGroupBox:
        grp = QGroupBox("Display")
        lay = QVBoxLayout(grp)

        lay.addWidget(QLabel("Display time:"))
        self.combo_disp_len = QComboBox()
        for d in DISPLAY_LENGTHS:
            self.combo_disp_len.addItem(f"{d}s")
        self.combo_disp_len.setCurrentIndex(len(DISPLAY_LENGTHS) - 1)
        lay.addWidget(self.combo_disp_len)

        lay.addWidget(QLabel("Display gain (Input):"))
        self.combo_disp_gain = QComboBox()
        for label, _ in VOLTAGE_GAINS:
            self.combo_disp_gain.addItem(label)
        lay.addWidget(self.combo_disp_gain)

        lay.addWidget(QLabel("Display gain (DSP):"))
        self.combo_dsp_gain = QComboBox()
        for label, _ in VOLTAGE_GAINS:
            self.combo_dsp_gain.addItem(label)
        lay.addWidget(self.combo_dsp_gain)

        lay.addWidget(QLabel("Preview signal (DOUT):"))
        self.combo_dout = QComboBox()
        for label, _ in DOUT_SIGNALS:
            self.combo_dout.addItem(label)
        self.combo_dout.currentIndexChanged.connect(self._on_dout_changed)
        lay.addWidget(self.combo_dout)

        h = QHBoxLayout()
        h.addWidget(QLabel("DAC Gain:"))
        self.spin_dac_gain = QDoubleSpinBox()
        self.spin_dac_gain.setRange(0.1, 1000)
        self.spin_dac_gain.setDecimals(1)
        self.spin_dac_gain.setValue(5)
        self.spin_dac_gain.setSingleStep(0.1)
        self.spin_dac_gain.valueChanged.connect(self._on_dac_gain_changed)
        h.addWidget(self.spin_dac_gain)
        lay.addLayout(h)

        self.cb_remove_dc = QCheckBox("Remove DC for display")
        self.cb_remove_dc.setChecked(True)
        lay.addWidget(self.cb_remove_dc)

        lay.addStretch()
        return grp

    # =====================================================================
    #  Defaults
    # =====================================================================

    def _init_defaults(self) -> None:
        self._refresh_ports()

    def _refresh_ports(self) -> None:
        self.combo_port.clear()
        ports = list_serial_ports()
        for p in ports:
            self.combo_port.addItem(p)
        auto = find_cl02_port()
        if auto and auto in ports:
            self.combo_port.setCurrentText(auto)

    def eventFilter(self, obj, event) -> bool:
        # Refresh port list when combo is clicked
        from PyQt6.QtCore import QEvent
        if obj is self.combo_port and event.type() == QEvent.Type.MouseButtonPress:
            self._refresh_ports()
        return super().eventFilter(obj, event)

    # =====================================================================
    #  Parameter helpers
    # =====================================================================

    def _get_display_length(self) -> int:
        idx = self.combo_disp_len.currentIndex()
        return DISPLAY_LENGTHS[idx] if 0 <= idx < len(DISPLAY_LENGTHS) else 10

    def _get_input_gain(self) -> float:
        idx = self.combo_disp_gain.currentIndex()
        return VOLTAGE_GAINS[idx][1] if 0 <= idx < len(VOLTAGE_GAINS) else 1.0

    def _get_dsp_gain(self) -> float:
        idx = self.combo_dsp_gain.currentIndex()
        return VOLTAGE_GAINS[idx][1] if 0 <= idx < len(VOLTAGE_GAINS) else 1.0

    def _get_cl_mode(self) -> int:
        idx = self.combo_dsp_mode.currentIndex()
        return DSP_MODES[idx][1] if 0 <= idx < len(DSP_MODES) else 0

    def _ratio(self) -> int:
        return 10 * SAMPLE_RATE // 1000

    def _update_params(self) -> None:
        """Collect all GUI values into DeviceConfig (mirrors C# UpdParams)."""
        cl_mode = self._get_cl_mode()
        ratio = self._ratio()
        dsp_id = self.spin_dsp_id.value()

        common = dict(
            sampling_rate=1000,
            interval=int(self.spin_interval.value() * ratio),
            delay=int(self.spin_delay.value() * ratio),
            rnd_delay=int(self.spin_rnd_delay.value() * ratio),
            pw=int(self.spin_pw.value() * ratio),
            cyc=int(self.spin_cyc.value()),
            train_start=int(self.spin_train_start.value()),
            train_dur=int(self.spin_train_dur.value()),
            mode=cl_mode,
            stim_on=int(self.cb_enable.isChecked()),
            rand_min=int(self.spin_rand_min.value()),
            rand_max=int(self.spin_rand_max.value()),
            param1=float(self.spin_phase_lo.value() / 180 * math.pi),
            param2=float(self.spin_phase_hi.value() / 180 * math.pi),
        )
        # Channel 0 always overridden
        self.device.set_sys_params(0, gain=self.device.sys.trigger_gain[0], **common)
        # Current channel
        self.device.set_sys_params(dsp_id, gain=float(self.spin_gain.value()), **common)
        self.device.set_dsp_params(
            dsp_id,
            formula=self.combo_formula.currentIndex(),
            func1=self.combo_filter.currentIndex(),
            ma_ord=int(self.spin_ma.value()),
        )

    # =====================================================================
    #  Serial data callback (runs in reader thread)
    # =====================================================================

    def _on_serial_data(self, raw: bytes, thresholds: list[int]) -> None:
        """Called from the background reader thread with raw sample bytes."""
        n_samples = len(raw) // (2 * TRANS_CH)
        if n_samples == 0:
            return

        input_gain = self._get_input_gain()
        dsp_gain = self._get_dsp_gain()
        dsp_ch = self.spin_dsp_id.value() + 4
        remove_dc = self.cb_remove_dc.isChecked()

        out = np.zeros((N_DISPLAY_CH, n_samples), dtype=np.float64)

        for s in range(n_samples):
            base = s * 2 * TRANS_CH
            # CH1
            v = struct.unpack_from("<h", raw, base + 0)[0] * input_gain
            if remove_dc:
                v = self.dc_filters[0].process(v)
            out[0, s] = v
            # CH2
            v = struct.unpack_from("<h", raw, base + 2)[0] * input_gain
            if remove_dc:
                v = self.dc_filters[1].process(v)
            out[1, s] = v
            # DSP
            v = struct.unpack_from("<h", raw, base + 2 * dsp_ch)[0] * dsp_gain
            out[2, s] = v - DSP_OFFSET
            # DOUT
            digi = struct.unpack_from("<H", raw, base + 2 * 6)[0]
            bit = digi & self.dout_mask
            out[3, s] = (1.8 if bit else 0.0) - 0.9

        self.data.append(out)

        # Log raw bytes
        if self.log_writer:
            try:
                self.log_writer.write(raw[:n_samples * 2 * TRANS_CH])
            except Exception:
                pass

        # Update threshold display (cross-thread safe via property)
        self.thresh_value = thresholds[self.spin_dsp_id.value()]

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

        # Threshold line on DSP plot
        line = self.thresh_lines[2]
        if line is not None:
            dsp_gain_val = self._get_dsp_gain()
            dac_gain_val = self.spin_dac_gain.value()
            t = self.thresh_value * dac_gain_val * dsp_gain_val - DSP_OFFSET
            line.setValue(t)
            line.setVisible(True)

        self.lbl_abs_thresh.setText(str(self.thresh_value))

    # =====================================================================
    #  Connection handlers
    # =====================================================================

    def _on_connect(self) -> None:
        port = self.combo_port.currentText()
        if not port:
            QMessageBox.warning(self, "Error", "No COM port selected")
            return

        # Logging setup
        if self.cb_log.isChecked():
            path, _ = QFileDialog.getSaveFileName(self, "Save Data", "", "Binary File (*.dat)")
            if path:
                self.lbl_file.setText(path)
                self.log_file = open(path, "wb")
                self.log_writer = self.log_file
            else:
                return  # cancelled

        try:
            self.serial.open(port)
        except Exception as e:
            QMessageBox.warning(self, "Error", f"Failed to open {port}:\n{e}")
            return

        self._send_all_settings()

        # Start preview
        self.serial.start_preview()
        self.serial.start_reader()
        self._paint_timer.start()

        self.btn_connect.setEnabled(False)
        self.btn_disconnect.setEnabled(True)

    def _on_disconnect(self) -> None:
        self._paint_timer.stop()
        self.serial.stop_preview()
        self.serial.close()

        if self.log_file:
            self.log_file.close()
            self.log_file = None
            self.log_writer = None

        self.btn_connect.setEnabled(True)
        self.btn_disconnect.setEnabled(False)

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
        self.serial.set_dac_gain(int(self.spin_dac_gain.value()))

    def _send_trig_mode(self) -> None:
        self.serial.set_trig_mode(self.combo_trig_mode.currentIndex())

    # =====================================================================
    #  UI event handlers
    # =====================================================================

    def _on_enable_trigger(self, checked: bool) -> None:
        self.serial.set_stim(checked)

    def _on_stim_param_changed(self) -> None:
        if self.spin_pw.value() > self.spin_interval.value():
            self.spin_interval.setValue(self.spin_pw.value())
        self._send_stim_param()

    def _on_gain_changed(self) -> None:
        self.serial.set_gain(self.spin_dsp_id.value(), float(self.spin_gain.value()))

    def _on_thresh_changed(self) -> None:
        self.serial.set_gain_abs(self.spin_dsp_id.value(), float(self.spin_thresh.value()))

    def _on_filter_changed(self, idx: int) -> None:
        if idx < len(FILTER_FREQS):
            v = int(1.0 / FILTER_FREQS[idx] * SAMPLE_RATE * 3)
            v = min(v, self.spin_ma.maximum())
            self.spin_ma.setValue(v)
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
        self.dsp_id_curr = self.spin_dsp_id.value()
        self._load_params_to_ui()

    def _on_download_params(self) -> None:
        self._send_all_settings()

    def _on_force_trigger(self) -> None:
        self.serial.force_trigger()

    def _on_trig_mode_changed(self) -> None:
        self._send_trig_mode()

    def _on_dac_gain_changed(self) -> None:
        self._send_dac_gain()

    def _on_dout_changed(self, idx: int) -> None:
        if 0 <= idx < len(DOUT_SIGNALS):
            self.dout_mask = DOUT_SIGNALS[idx][1]

    def _on_phase_changed(self) -> None:
        self._update_params()
        self._send_cl_params()

    def _load_custom_filter(self, fid: int) -> None:
        path, _ = QFileDialog.getOpenFileName(self, "Load Custom Filter", "", "Filter Parameter (*.filter);;All Files (*)")
        if path and os.path.isfile(path):
            with open(path, "rb") as f:
                self.custom_filter_data[fid] = f.read(512)
            self.custom_filter_loaded[fid] = True
            self.lbl_custom_filter.setText(path)
            self._send_custom_filter(fid)

    def _load_params_to_ui(self) -> None:
        """Restore UI controls from DeviceConfig for the current DSP channel."""
        d = self.device
        sid = self.dsp_id_curr
        ratio = self._ratio()
        idx = max(0, d.sys.cl_mode)
        mode_values = [v for _, v in DSP_MODES]
        if idx in mode_values:
            self.combo_dsp_mode.setCurrentIndex(mode_values.index(idx))
        self.spin_interval.setValue(d.sys.stim_interval[0] / ratio if ratio else 0)
        self.spin_delay.setValue(d.sys.stim_delay[0] / ratio if ratio else 0)
        self.spin_rnd_delay.setValue(d.sys.stim_rnd_delay[0] / ratio if ratio else 0)
        self.spin_pw.setValue(d.sys.pulse_width[0] / ratio if ratio else 0)
        self.spin_cyc.setValue(d.sys.pulse_cyc[0])
        self.spin_gain.setValue(d.sys.trigger_gain[sid])
        self.combo_formula.setCurrentIndex(d.formula[sid])
        self.combo_filter.setCurrentIndex(d.func[sid])
        self.spin_ma.setValue(d.ma_ord[sid])
        self.spin_phase_lo.setValue(d.sys.cl_param1[sid] / math.pi * 180)
        self.spin_phase_hi.setValue(d.sys.cl_param2[sid] / math.pi * 180)

    # =====================================================================
    #  Cleanup
    # =====================================================================

    def closeEvent(self, event) -> None:
        self._on_disconnect()
        super().closeEvent(event)
