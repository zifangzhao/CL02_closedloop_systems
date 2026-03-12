"""
data_source.py — Ring-buffer backed multi-channel data store.

Replaces the C# DataSource class with a NumPy-backed circular buffer that
is much faster for real-time append and slice operations.
"""

from __future__ import annotations

import numpy as np


class DataSource:
    """Fixed-size circular buffer for *n_channels* of float64 samples.

    Parameters
    ----------
    n_channels : int
        Number of parallel data channels.
    max_samples : int
        Maximum number of samples stored per channel (capacity).
    sample_rate : int
        Nominal sample rate in Hz — stored for convenience.
    """

    __slots__ = ("n_channels", "capacity", "sample_rate", "_buf", "_write_pos", "_total_written")

    def __init__(self, n_channels: int, max_samples: int, sample_rate: int = 1000):
        self.n_channels = n_channels
        self.capacity = max_samples
        self.sample_rate = sample_rate
        self._buf = np.zeros((n_channels, max_samples), dtype=np.float64)
        self._write_pos = 0      # next index to write
        self._total_written = 0  # cumulative sample count (never resets)

    # ── Write ────────────────────────────────────────────────────────────

    def append(self, data: np.ndarray) -> None:
        """Append *data* shaped ``(n_channels, n_samples)`` into the ring buffer."""
        n_ch, n_samps = data.shape
        assert n_ch == self.n_channels

        end = self._write_pos + n_samps
        if end <= self.capacity:
            self._buf[:, self._write_pos:end] = data
        else:
            # Wrap around
            first = self.capacity - self._write_pos
            self._buf[:, self._write_pos:] = data[:, :first]
            remainder = n_samps - first
            self._buf[:, :remainder] = data[:, first:]

        self._write_pos = end % self.capacity
        self._total_written += n_samps

    # ── Read ─────────────────────────────────────────────────────────────

    @property
    def available(self) -> int:
        """Number of valid samples currently stored (up to *capacity*)."""
        return min(self._total_written, self.capacity)

    def get_latest(self, n_samples: int) -> np.ndarray:
        """Return the most recent *n_samples* as ``(n_channels, n_samples)``."""
        n = min(n_samples, self.available)
        if n == 0:
            return np.zeros((self.n_channels, 0), dtype=np.float64)

        start = (self._write_pos - n) % self.capacity
        if start + n <= self.capacity:
            return self._buf[:, start:start + n].copy()
        else:
            first = self.capacity - start
            return np.hstack([self._buf[:, start:], self._buf[:, :n - first]]).copy()

    def reset(self) -> None:
        self._buf[:] = 0.0
        self._write_pos = 0
        self._total_written = 0
