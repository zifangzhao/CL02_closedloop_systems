"""Entry point for `python -m cl02_center`."""
from __future__ import annotations

import sys
import logging

from PyQt6.QtWidgets import QApplication
from .main_window import MainWindow


def main() -> None:
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s [%(levelname)s] %(name)s: %(message)s",
    )
    app = QApplication(sys.argv)
    app.setApplicationName("CL02 Closed-Loop System")
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
