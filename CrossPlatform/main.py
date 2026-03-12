#!/usr/bin/env python3
"""
CL02 Closed-Loop System — Cross-Platform Control Center

Usage:
    python -m cl02_center          # from the CrossPlatform/ directory
  or
    python main.py                 # directly
"""

from __future__ import annotations

import sys
import logging

from qtpy.QtWidgets import QApplication
from cl02_center.main_window import MainWindow, TriggerDSPPanel, ConnectionPanel, DisplayPanel


def main() -> None:
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s [%(levelname)s] %(name)s: %(message)s",
    )

    app = QApplication(sys.argv)
    app.setApplicationName("CL02 Closed-Loop System")

    trigger_panel = TriggerDSPPanel()
    connection_panel = ConnectionPanel()
    display_panel = DisplayPanel()
    window = MainWindow(
        trigger_panel=trigger_panel,
        connection_panel=connection_panel,
        display_panel=display_panel,
    )
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
