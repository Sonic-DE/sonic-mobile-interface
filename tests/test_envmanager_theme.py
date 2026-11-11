#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
"""The mobile session keeps the user's own global theme: the environment manager never sets one."""
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

OVERLAY = Path("plasma-mobile") / "kdeglobals"


class TestEnvmanagerTheme(unittest.TestCase):

    def setUp(self):
        self.envmanager = sys.argv[1]
        self.home = tempfile.TemporaryDirectory()
        self.config = Path(self.home.name) / ".config"
        self.config.mkdir()

    def tearDown(self):
        self.home.cleanup()

    def apply_settings(self):
        env = {
            "PATH": os.environ.get("PATH", "/usr/bin:/bin"),
            "LD_LIBRARY_PATH": os.environ.get("LD_LIBRARY_PATH", ""),
            "HOME": self.home.name,
            "XDG_CONFIG_HOME": str(self.config),
            "XDG_DATA_HOME": str(Path(self.home.name) / "data"),
            "XDG_CACHE_HOME": str(Path(self.home.name) / "cache"),
            "XDG_STATE_HOME": str(Path(self.home.name) / "state"),
            "DBUS_SESSION_BUS_ADDRESS": "unix:path=" + str(Path(self.home.name) / "no-bus"),
            "QT_QPA_PLATFORM": "offscreen",
            "PLASMA_PLATFORM": "phone",
        }
        subprocess.run([self.envmanager, "--apply-settings"], env=env, check=True, timeout=30)

    def assert_no_theme(self):
        for path in (self.config / OVERLAY, self.config / "kdeglobals"):
            if path.exists():
                self.assertNotIn("LookAndFeelPackage", path.read_text(encoding="utf-8"), str(path))
        # The overlay is still written, so the settings were really applied.
        self.assertIn("BrowserApplication", (self.config / OVERLAY).read_text(encoding="utf-8"))

    def test_fresh_user(self):
        self.apply_settings()
        self.assert_no_theme()

    def test_removes_theme_forced_by_older_versions(self):
        (self.config / OVERLAY).parent.mkdir()
        (self.config / OVERLAY).write_text("[KDE]\nLookAndFeelPackage[$i]=org.kde.silver.mobile\n", encoding="utf-8")
        self.apply_settings()
        self.assert_no_theme()


if __name__ == "__main__":
    unittest.main(argv=sys.argv[:1])
