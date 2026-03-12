"""
ce_core.py — Device parameter structs and binary serialization.

Mirrors the C# CE_core class and the MCU-side CE32_systemParam / CE32_dspParam
structs. All multi-byte fields are little-endian to match the STM32F3 MCU.
"""

from __future__ import annotations

import math
import struct
from dataclasses import dataclass, field
from typing import List

# ── Constants ────────────────────────────────────────────────────────────────

SYSTEM_PARAM_SIZE = 512  # bytes sent with CMD 0x01
DSP_PARAM_SIZE = 512     # bytes sent with CMD 0x02
DSP_COUNT = 2            # number of independent DSP channels

# ── System parameters ────────────────────────────────────────────────────────

@dataclass
class SystemParams:
    """Parameters shared across the whole system (sent as CMD 0x01)."""

    fs: int = 1000

    # Per-channel arrays (indexed by DSP id, up to 4 slots on the MCU)
    stim_interval: List[int] = field(default_factory=lambda: [0] * 4)
    stim_delay: List[int] = field(default_factory=lambda: [0] * 4)
    stim_rnd_delay: List[int] = field(default_factory=lambda: [0] * 4)
    pulse_width: List[int] = field(default_factory=lambda: [0] * 4)
    pulse_cyc: List[int] = field(default_factory=lambda: [0] * 4)
    trigger_gain: List[float] = field(default_factory=lambda: [0.0] * 4)
    stim_intensity: List[int] = field(default_factory=lambda: [0] * 4)
    stim_ch: List[int] = field(default_factory=lambda: [0] * 4)
    cl_param1: List[float] = field(default_factory=lambda: [0.0] * 4)
    cl_param2: List[float] = field(default_factory=lambda: [0.0] * 4)

    trigger_train_start: int = 0
    trigger_train_duration: int = 0
    cl_mode: int = 0
    stim_mode: int = 0
    led_pulse_cnt: int = 0
    preview_channel_bank: int = 0
    randtrig_min: int = 0
    randtrig_max: int = 0

    def set_sys(
        self,
        dsp_idx: int,
        sampling_rate: int,
        interval: int,
        delay: int,
        rnd_delay: int,
        pw: int,
        cyc: int,
        gain: float,
        train_start: int,
        train_dur: int,
        mode: int,
        stim_on: int,
        rand_min: int,
        rand_max: int,
        param1: float,
        param2: float,
    ) -> None:
        self.fs = sampling_rate
        self.stim_interval[dsp_idx] = interval
        self.stim_delay[dsp_idx] = delay
        self.stim_rnd_delay[dsp_idx] = rnd_delay
        self.pulse_width[dsp_idx] = pw
        self.pulse_cyc[dsp_idx] = cyc
        self.trigger_gain[dsp_idx] = gain
        self.trigger_train_start = train_start
        self.trigger_train_duration = train_dur
        self.cl_mode = mode
        self.stim_mode = stim_on
        self.randtrig_max = rand_max // 100
        self.randtrig_min = rand_min // 100
        self.cl_param1[dsp_idx] = param1
        self.cl_param2[dsp_idx] = param2

    def to_bytes(self) -> bytes:
        """Serialize to the exact 512-byte CE32_systemParam layout."""
        buf = bytearray(SYSTEM_PARAM_SIZE)
        le = "<"  # little-endian

        struct.pack_into(f"{le}I", buf, 0, self.fs)             # 0: fs
        # 4: AUX_mode (unused, 0)
        # 8: Nch (unused, 0)
        # 12..139: convCmd[64] — 128 bytes (unused)
        # 140: dispCH
        # 144: func1, 148: func2, 152: cmd_ch, 156: rec_ch  — unused here
        struct.pack_into(f"{le}I", buf, 160, self.stim_mode)    # 160: stim_mode
        struct.pack_into(f"{le}I", buf, 164, self.cl_mode)      # 164: cl_mode (was at +168 in C#; offsets from the struct def)

        # Offsets from C# struct definition
        # NOTE: the C# struct comments show cumulative byte offsets.  We use
        # the layout exactly as the C# StructureToPtr produces it.
        # Re-derive from the struct definition:
        #   fs(4) AUX_mode(4) Nch(4) convCmd(128) dispCH(4) func1(4)
        #   func2(4) cmd_ch(4) rec_ch(4) stim_mode(4) cl_mode(4)
        #   stim_interval[4](16) pulse_width[4](16) pulse_cnt[4](16)
        #   stim_delay[4](16) stim_RndDelay[4](16)
        #   trigger_trainStart(4) trigger_trainDuration(4)
        #   trigger_gain[4](16)
        #   SD_capacity(4) LED_pulse_CNT(4) preview_channel_bank(4)
        #   system_status(4) stim_intensity[4](16) stim_ch[4](16)
        #   randtrig_min(4) randtrig_max(4)
        #   cl_param1[4](16) cl_param2[4](16)
        #   unassigned[38](152)
        # Total = 512

        off = 0
        struct.pack_into(f"{le}I", buf, off, self.fs);            off += 4   # fs
        off += 4   # AUX_mode
        off += 4   # Nch
        off += 128 # convCmd
        off += 4   # dispCH
        off += 4   # func1
        off += 4   # func2
        off += 4   # cmd_ch
        off += 4   # rec_ch
        struct.pack_into(f"{le}I", buf, off, self.stim_mode);    off += 4
        struct.pack_into(f"{le}I", buf, off, self.cl_mode);      off += 4

        for i in range(4):
            struct.pack_into(f"{le}I", buf, off, self.stim_interval[i]); off += 4
        for i in range(4):
            struct.pack_into(f"{le}I", buf, off, self.pulse_width[i]); off += 4
        for i in range(4):
            struct.pack_into(f"{le}I", buf, off, self.pulse_cyc[i]); off += 4
        for i in range(4):
            struct.pack_into(f"{le}I", buf, off, self.stim_delay[i]); off += 4
        for i in range(4):
            struct.pack_into(f"{le}I", buf, off, self.stim_rnd_delay[i]); off += 4

        struct.pack_into(f"{le}I", buf, off, self.trigger_train_start); off += 4
        struct.pack_into(f"{le}I", buf, off, self.trigger_train_duration); off += 4

        for i in range(4):
            struct.pack_into(f"{le}f", buf, off, self.trigger_gain[i]); off += 4

        off += 4  # SD_capacity
        struct.pack_into(f"{le}I", buf, off, self.led_pulse_cnt); off += 4
        struct.pack_into(f"{le}I", buf, off, self.preview_channel_bank); off += 4
        off += 4  # system_status

        for i in range(4):
            struct.pack_into(f"{le}I", buf, off, self.stim_intensity[i]); off += 4
        for i in range(4):
            struct.pack_into(f"{le}I", buf, off, self.stim_ch[i]); off += 4

        struct.pack_into(f"{le}I", buf, off, self.randtrig_min); off += 4
        struct.pack_into(f"{le}I", buf, off, self.randtrig_max); off += 4

        for i in range(4):
            struct.pack_into(f"{le}f", buf, off, self.cl_param1[i]); off += 4
        for i in range(4):
            struct.pack_into(f"{le}f", buf, off, self.cl_param2[i]); off += 4

        # remainder is unassigned padding — already zeroed
        return bytes(buf)


# ── DSP parameters ───────────────────────────────────────────────────────────

@dataclass
class DspParams:
    """Per-channel DSP parameters (sent as CMD 0x02)."""

    ch_ord: List[int] = field(default_factory=lambda: [0] * 128)  # byte array
    formula: int = 0
    func1: int = 0
    func2: int = 0
    ma_ord: int = 0

    def to_bytes(self) -> bytes:
        buf = bytearray(DSP_PARAM_SIZE)
        for i, v in enumerate(self.ch_ord[:128]):
            buf[i] = v & 0xFF
        off = 128
        struct.pack_into("<I", buf, off, self.formula); off += 4
        struct.pack_into("<I", buf, off, self.func1);   off += 4
        struct.pack_into("<I", buf, off, self.func2);   off += 4
        struct.pack_into("<I", buf, off, self.ma_ord);  off += 4
        return bytes(buf)


# ── Convenience wrapper (mirrors CE_core class) ─────────────────────────────

class DeviceConfig:
    """High-level device configuration that aggregates system + DSP params."""

    def __init__(self, ch_num: int = 2):
        self.sys = SystemParams()
        self.dsp: List[DspParams] = [DspParams() for _ in range(DSP_COUNT)]
        # Convenience aliases matching C# field names
        self.formula = [0] * 4
        self.func = [0] * 4
        self.ma_ord = [0] * 4
        self.ch_ord: List[int] = [0] * ch_num

    # ── Setters ──────────────────────────────────────────────────────────

    def set_sys_params(self, dsp_idx: int, **kw) -> None:
        self.sys.set_sys(dsp_idx, **kw)

    def set_dsp_params(self, dsp_idx: int, formula: int, func1: int, ma_ord: int) -> None:
        self.formula[dsp_idx] = formula
        self.func[dsp_idx] = func1
        self.ma_ord[dsp_idx] = ma_ord

    # ── Serializers ──────────────────────────────────────────────────────

    def save_sys_param(self) -> bytes:
        return self.sys.to_bytes()

    def save_dsp_param(self, idx: int) -> bytes:
        dsp = DspParams()
        dsp.formula = self.formula[idx]
        dsp.func1 = self.func[idx]
        dsp.ma_ord = self.ma_ord[idx]
        for i, v in enumerate(self.ch_ord):
            dsp.ch_ord[i] = v
        return dsp.to_bytes()
