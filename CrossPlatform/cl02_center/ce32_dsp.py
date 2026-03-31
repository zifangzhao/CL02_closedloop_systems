"""
ce32_dsp.py — Client-side IIR filter for display (DC removal, etc.).

Mirrors the C# CE32_filter class.
"""

from __future__ import annotations

import numpy as np


class IIRFilter:
    """Direct-Form II transposed IIR filter (single section).

    Parameters
    ----------
    num : array-like
        Numerator (feedforward) coefficients [b0, b1, …, bN].
    den : array-like
        Denominator (feedback) coefficients  [1, a1, …, aN].
        den[0] is assumed to be 1 and is not used in the recurrence.
    """

    __slots__ = ("_b", "_a", "_order", "_state", "_ptr")

    def __init__(self, num: list[float], den: list[float]):
        self._b = np.asarray(num, dtype=np.float64)
        self._a = np.asarray(den, dtype=np.float64)
        assert len(self._b) == len(self._a), "num/den must be same length"
        self._order = len(self._b)
        # State buffer: pairs of (x, y) for each delay tap
        self._state = np.zeros(self._order * 2, dtype=np.float64)
        self._ptr = 0

    def process(self, x: float) -> float:
        """Feed one sample, return one filtered sample."""
        y = self._b[0] * x
        p = self._ptr
        for i in range(1, self._order):
            y += self._b[i] * self._state[2 * p] - self._a[i] * self._state[2 * p + 1]
            p -= 1
            if p < 0:
                p += self._order
        # Advance pointer
        self._ptr = (self._ptr + 1) % self._order
        self._state[2 * self._ptr] = x
        self._state[2 * self._ptr + 1] = y
        return y

    def process_block(self, block: np.ndarray) -> np.ndarray:
        """Vectorised convenience — processes an array sample-by-sample."""
        out = np.empty_like(block, dtype=np.float64)
        for i, x in enumerate(block):
            out[i] = self.process(float(x))
        return out

    def reset(self) -> None:
        self._state[:] = 0.0
        self._ptr = 0


# ── Default high-pass filter used for DC removal on the display ──────────

# Butterworth 2nd-order HPF, fc ≈ 0.45 Hz @ 1 kHz  (same coefficients as C#)
DC_REMOVAL_NUM = [0.997781024102941, -1.99556204820588, 0.997781024102941]
DC_REMOVAL_DEN = [1.0, -1.99555712434579, 0.995566972065975]


def make_dc_removal_filter() -> IIRFilter:
    """Return a fresh DC-removal high-pass filter instance."""
    return IIRFilter(DC_REMOVAL_NUM, DC_REMOVAL_DEN)
