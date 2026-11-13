#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
"""The mobile session uses its own light or dark global theme, kept apart from the desktop's."""
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

OVERLAY = "plasma-mobile"
SESSION_RC = "plasmamobilerc"
PLATFORM = "phone"
PACKAGES = {"light": "lookandfeel", "dark": "lookandfeel-dark"}
USER_FILES = ("kdeglobals", "plasmarc", "kcminputrc", "ksplashrc", "kwinrc")

SCHEME = """[Colors:Window]
BackgroundNormal={bg}
ForegroundNormal={fg}

[Colors:Header]
BackgroundNormal={bg}

[Colors:Header][Inactive]
BackgroundNormal={bg}

[General]
ColorScheme={name}
Name={name}

[WM]
activeBackground={bg}
"""


def parse(path):
    """KDE ini file as {(group, path...): {key: (value, locked)}}."""
    groups, group = {}, ()
    for line in Path(path).read_text(encoding="utf-8").splitlines():
        if line.startswith("[") and line.endswith("]") and "=" not in line:
            group = tuple(line[1:-1].split("]["))
            groups.setdefault(group, {})
        elif "=" in line and not line.startswith("#"):
            key, value = line.split("=", 1)
            locked = key.endswith("[$i]")
            groups.setdefault(group, {})[key.removesuffix("[$i]")] = (value, locked)
    return groups


class TestEnvmanagerTheme(unittest.TestCase):

    def setUp(self):
        self.envmanager, source = sys.argv[1], Path(sys.argv[2])
        self.home = tempfile.TemporaryDirectory()
        self.config = Path(self.home.name) / ".config"
        self.data = Path(self.home.name) / "data"
        self.config.mkdir()
        self.ids = {}
        for mode, folder in PACKAGES.items():
            package = source / folder
            self.ids[mode] = json.loads((package / "metadata.json").read_text(encoding="utf-8"))["KPlugin"]["Id"]
            shutil.copytree(package, self.data / "plasma" / "look-and-feel" / self.ids[mode])
        schemes = self.data / "color-schemes"
        schemes.mkdir()
        (schemes / "SilverLight.colors").write_text(SCHEME.format(name="SilverLight", bg="239,240,241", fg="35,38,41"), encoding="utf-8")
        (schemes / "SilverDark.colors").write_text(SCHEME.format(name="SilverDark", bg="32,35,38", fg="252,252,252"), encoding="utf-8")

    def tearDown(self):
        self.home.cleanup()

    def apply_settings(self, config=None):
        env = {
            "PATH": os.environ.get("PATH", "/usr/bin:/bin"),
            "LD_LIBRARY_PATH": os.environ.get("LD_LIBRARY_PATH", ""),
            "HOME": self.home.name,
            "XDG_CONFIG_HOME": str(config or self.config),
            "XDG_DATA_HOME": str(self.data),
            "XDG_DATA_DIRS": str(self.data),
            "XDG_CACHE_HOME": str(Path(self.home.name) / "cache"),
            "XDG_STATE_HOME": str(Path(self.home.name) / "state"),
            "DBUS_SESSION_BUS_ADDRESS": "unix:path=" + str(Path(self.home.name) / "no-bus"),
            "QT_QPA_PLATFORM": "offscreen",
            "PLASMA_PLATFORM": PLATFORM,
        }
        subprocess.run([self.envmanager, "--apply-settings"], env=env, check=True, timeout=30)

    def desktop(self, scheme, file="kdeglobals"):
        (self.config / file).parent.mkdir(parents=True, exist_ok=True)
        (self.config / file).write_text(f"[General]\nColorScheme={scheme}\n", encoding="utf-8")

    def set_dark_mode(self, dark, config=None):
        rc = (config or self.config) / SESSION_RC
        text = rc.read_text(encoding="utf-8") if rc.exists() else ""
        text = "\n".join(line for line in text.splitlines() if not line.startswith("darkMode="))
        if "[General]" not in text:
            text += "\n[General]\n"
        rc.write_text(text.replace("[General]", f"[General]\ndarkMode={'true' if dark else 'false'}", 1), encoding="utf-8")

    def session_theme(self, config=None):
        return parse((config or self.config) / OVERLAY / "kdeglobals")[("KDE",)]["LookAndFeelPackage"]

    def overlay_files(self, config):
        root = config / OVERLAY
        return {p.relative_to(root): parse(p) for p in sorted(root.rglob("*")) if p.is_file()}

    def test_first_session_follows_desktop(self):
        # A global theme's own values are in kdedefaults; the user's choices in kdeglobals.
        cases = [(scheme, mode, file) for scheme, mode in (("SilverLight", "light"), ("SilverDark", "dark"))
                 for file in ("kdeglobals", "kdedefaults/kdeglobals")]
        for scheme, mode, file in cases:
            with self.subTest(scheme=scheme, file=file):
                shutil.rmtree(self.config)
                self.config.mkdir()
                self.desktop(scheme, file)
                self.apply_settings()
                self.assertEqual(self.session_theme(), (self.ids[mode], True))

    def test_switch_wins_over_desktop(self):
        self.desktop("SilverLight")
        self.set_dark_mode(True)
        self.apply_settings()
        self.assertEqual(self.session_theme(), (self.ids["dark"], True))

    def test_every_theme_setting_is_locked_in_the_overlay(self):
        for mode, folder in PACKAGES.items():
            with self.subTest(mode=mode):
                self.set_dark_mode(mode == "dark")
                self.apply_settings()
                defaults = parse(self.data / "plasma" / "look-and-feel" / self.ids[mode] / "contents" / "defaults")
                for group, entries in defaults.items():
                    if group[0] == "Wallpaper":
                        continue
                    overlay = parse(self.config / OVERLAY / group[0])
                    for key, (value, _) in entries.items():
                        self.assertEqual(overlay[group[1:]][key], (value, True), f"{group} {key}")
                scheme = parse(self.data / "color-schemes" / (defaults[("kdeglobals", "General")]["ColorScheme"][0] + ".colors"))
                kdeglobals = parse(self.config / OVERLAY / "kdeglobals")
                for group, entries in scheme.items():
                    for key, (value, _) in entries.items():
                        if key != "Name":
                            self.assertEqual(kdeglobals[group][key], (value, True), f"{group} {key}")

    def test_desktop_files_are_never_written(self):
        for name in USER_FILES:
            (self.config / name).write_text(f"[Desktop]\nmine={name}\n", encoding="utf-8")
        self.desktop("SilverDark")
        before = {name: hashlib.sha256((self.config / name).read_bytes()).hexdigest() for name in USER_FILES}
        self.apply_settings()
        self.set_dark_mode(False)
        self.apply_settings()
        after = {name: hashlib.sha256((self.config / name).read_bytes()).hexdigest() for name in USER_FILES}
        self.assertEqual(before, after)

    def test_switching_leaves_nothing_of_the_other_theme(self):
        for first, second in (("light", "dark"), ("dark", "light")):
            with self.subTest(first=first, second=second):
                switched, fresh = Path(self.home.name) / f"switched-{first}", Path(self.home.name) / f"fresh-{second}"
                for config in (switched, fresh):
                    config.mkdir()
                self.set_dark_mode(first == "dark", switched)
                self.apply_settings(switched)
                self.set_dark_mode(second == "dark", switched)
                self.apply_settings(switched)
                self.set_dark_mode(second == "dark", fresh)
                self.apply_settings(fresh)
                self.assertEqual(self.overlay_files(switched), self.overlay_files(fresh))


if __name__ == "__main__":
    unittest.main(argv=sys.argv[:1])
