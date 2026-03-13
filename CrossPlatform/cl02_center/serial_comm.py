"""
serial_comm.py — Serial communication layer for the CL02 MCU.

Handles:
  • Port discovery (cross-platform: Linux, macOS, Windows)
  • Packet framing  (<cmd_id payload>)
  • Background reader thread that parses incoming data packets
  • All command helpers matching the C# Send*/Set* methods
"""

from __future__ import annotations

import struct
import threading
import time
import logging
from typing import Callable, Optional

import numpy as np
import serial
import serial.tools.list_ports

from .ce_core import DeviceConfig, SYSTEM_PARAM_SIZE, DSP_PARAM_SIZE, DSP_COUNT

logger = logging.getLogger(__name__)

# ── Protocol constants ───────────────────────────────────────────────────────

HEADER = 0x3C  # '<'
TAIL = 0x3E  # '>'
DATA_HEADER2 = 0xAD  # second header byte of a data packet
TRANS_CH = 8  # channels per sample in a data packet
PACKAGE_SIZE = 256  # bytes of raw sample data per packet

# ── Port discovery ───────────────────────────────────────────────────────────


def find_cl02_port() -> Optional[str]:
    """Auto-detect the CL02 USB-CDC serial port (cross-platform)."""
    for info in serial.tools.list_ports.comports():
        desc = (info.description or "").lower()
        if "usb serial device" in desc or "cdc" in desc or "cl02" in desc:
            return info.device
    # Fallback: return the first available port
    ports = serial.tools.list_ports.comports()
    return ports[0].device if ports else None


def list_serial_ports() -> list[str]:
    """Return a list of all serial port device paths."""
    return [p.device for p in serial.tools.list_ports.comports()]


# ── Command builder helpers ──────────────────────────────────────────────────


def _frame(cmd_id: int, payload: bytes = b"") -> bytes:
    """Build a framed packet: <cmd_id payload>."""
    return bytes([HEADER, cmd_id]) + payload + bytes([TAIL])


# ── Main serial manager ─────────────────────────────────────────────────────


class SerialManager:
    """Manages the serial connection to the CL02 MCU."""

    def __init__(self):
        self._port: Optional[serial.Serial] = None
        self._reader_thread: Optional[threading.Thread] = None
        self._running = False

        # Callback: receives (data_array: np.ndarray shape (4, n), thresholds: list[int])
        self.on_data: Optional[Callable] = None

        self._lock = threading.Lock()

    # ── Connection ───────────────────────────────────────────────────────

    @property
    def is_open(self) -> bool:
        return self._port is not None and self._port.is_open

    def open(self, port_name: str, baudrate: int = 1_192_000) -> None:
        if self.is_open:
            self.close()
        self._port = serial.Serial(
            port=port_name,
            baudrate=baudrate,
            timeout=0.05,
            write_timeout=1.0,
        )
        self._port.reset_input_buffer()

    def close(self) -> None:
        self.stop_reader()
        if self._port and self._port.is_open:
            self._port.close()
        self._port = None

    # ── Background reader ────────────────────────────────────────────────

    def start_reader(self) -> None:
        if self._running:
            return
        self._running = True
        self._reader_thread = threading.Thread(target=self._reader_loop, daemon=True)
        self._reader_thread.start()

    def stop_reader(self) -> None:
        self._running = False
        if self._reader_thread:
            self._reader_thread.join(timeout=2.0)
            self._reader_thread = None

    def _reader_loop(self) -> None:
        """Continuously read and parse data packets from the MCU."""
        while self._running and self.is_open:
            try:
                self._read_packets()
            except serial.SerialException:
                logger.warning("Serial read error — stopping reader")
                self._running = False
            except Exception:
                logger.exception("Unexpected error in reader loop")
                time.sleep(0.01)

    def _read_packets(self) -> None:
        """Parse as many complete data packets as are available."""
        port = self._port
        if port is None or not port.is_open:
            return

        buf = bytearray()
        thresholds = [0, 0]

        while port.in_waiting > PACKAGE_SIZE + 6:
            # Sync to header byte 0x3C
            b = port.read(1)
            if not b or b[0] != HEADER:
                continue
            # Sync to second header byte 0xAD
            b = port.read(1)
            if not b or b[0] != DATA_HEADER2:
                continue

            # Read 4 bytes of threshold info (2 × int16)
            info = port.read(4)
            if len(info) < 4:
                continue
            thresholds[0] = struct.unpack_from("<h", info, 0)[0]
            thresholds[1] = struct.unpack_from("<h", info, 2)[0]

            # Read sample payload
            chunk = port.read(PACKAGE_SIZE)
            if len(chunk) < PACKAGE_SIZE:
                continue
            buf.extend(chunk)

            # Discard trailing 0x3E
            port.read(1)

            # Stop if we've accumulated enough
            if len(buf) + PACKAGE_SIZE > PACKAGE_SIZE * 16:
                break

        if buf and self.on_data:
            self.on_data(bytes(buf), thresholds)

    # ── Command senders ──────────────────────────────────────────────────

    def _write(self, data: bytes) -> None:
        with self._lock:
            if self.is_open:
                self._port.write(data)

    # -- System Config (CMD 0x01) --
    def send_sys_settings(self, cfg: DeviceConfig) -> None:
        payload = cfg.save_sys_param()
        self._write(_frame(0x01, payload))

    # -- DSP Config (CMD 0x02) --
    def send_dsp_settings(self, cfg: DeviceConfig, idx: int = 0) -> None:
        payload = cfg.save_dsp_param(idx)
        self._write(_frame(0x02, payload))

    # -- Custom filter (CMD 0x06) --
    def send_custom_filter(self, filter_id: int, data: bytes) -> None:
        padded = bytearray(512)
        padded[: len(data)] = data[:512]
        self._write(
            bytes([HEADER, 0x06, filter_id & 0xFF]) + bytes(padded) + bytes([TAIL])
        )

    # -- Gain update (CMD 0x10) --
    def set_gain(self, dsp_ch: int, gain: float) -> None:
        payload = bytes([dsp_ch & 0xFF]) + struct.pack("<f", gain)
        self._write(_frame(0x10, payload))

    # -- Stim on/off (CMD 0x11) --
    def set_stim(self, on: bool) -> None:
        self._write(_frame(0x11, bytes([1 if on else 0])))

    # -- Trigger mode (CMD 0x12) --
    def set_trig_mode(self, mode: int) -> None:
        self._write(_frame(0x12, bytes([mode & 0xFF])))

    # -- Absolute threshold (CMD 0x14) --
    def set_gain_abs(self, dsp_ch: int, value: float) -> None:
        payload = bytes([dsp_ch & 0xFF]) + struct.pack("<f", value)
        self._write(_frame(0x14, payload))

    # -- CL phase params (CMD 0x15) --
    def set_cl_params(self, param1: list[float], param2: list[float]) -> None:
        payload = struct.pack("<4f4f", *param1, *param2)
        self._write(_frame(0x15, payload))

    # -- Stim timing params (CMD 0x20) --
    def set_stim_param(
        self,
        dsp_id: int,
        delay: int,
        rnd_delay: int,
        duration: int,
        interval: int,
        cycle: int,
    ) -> None:
        payload = bytes([dsp_id & 0xFF]) + struct.pack(
            "<5I", delay, rnd_delay, duration, interval, cycle
        )
        self._write(_frame(0x20, payload))

    # -- DSP settings (CMD 0x21) --
    def set_dsp_param(
        self, dsp_id: int, ma_ord: int, filter_type: int, formula: int
    ) -> None:
        payload = bytes([dsp_id & 0xFF]) + struct.pack(
            "<3I", ma_ord, filter_type, formula
        )
        self._write(_frame(0x21, payload))

    # -- Start preview (CMD 0x40) --
    def start_preview(self) -> None:
        self._write(bytes([HEADER, 0x40, TAIL]))

    # -- Stop preview (CMD 0x41) --
    def stop_preview(self) -> None:
        self._write(bytes([HEADER, 0x41, TAIL]))

    # -- DAC gain (CMD 0x60) --
    def set_dac_gain(self, value: int) -> None:
        self._write(_frame(0x60, struct.pack("<I", value * 10)))

    # -- Force trigger (CMD 0x83) --
    def force_trigger(self) -> None:
        self._write(_frame(0x83, b"\x00"))
