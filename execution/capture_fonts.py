#!/usr/bin/env python3
"""Capture the six Clockfish time fonts at a wide 24-hour time on one emulator."""

from __future__ import annotations

import argparse
import json
import subprocess
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WATCHFACE = ROOT / "watchface"
OUTPUT = ROOT / "media/native"
NAMES = ("rounded", "digital-bold", "digital-fine", "classic-bold", "compact-bold", "tall-light")
SIZES = {"emery": (200, 228), "gabbro": (260, 260)}


def run(*args: str) -> None:
    result = subprocess.run(["pebble", *args], cwd=WATCHFACE, capture_output=True, text=True)
    if result.returncode:
        raise RuntimeError(f"pebble {' '.join(args)} failed: {result.stderr.strip()}")


def capture(platform: str) -> None:
    keys = json.loads((WATCHFACE / "build/js/message_keys.json").read_text())
    OUTPUT.mkdir(parents=True, exist_ok=True)
    run("emu-time-format", "--emulator", platform, "--format", "24h")
    for index, name in enumerate(NAMES):
        run("send-app-message", "--emulator", platform, "--int",
            f"{keys['SETTING_SCENE']}=0", f"{keys['SETTING_DATE']}=1",
            f"{keys['SETTING_MODE']}=0", f"{keys['SETTING_FISH_COLOR']}=0",
            f"{keys['SETTING_BACKGROUND']}=0", f"{keys['SETTING_MOTION']}=0",
            f"{keys['SETTING_TIME_FONT']}={index}")
        time.sleep(0.3)
        run("emu-set-time", "--emulator", platform, "23:58:00")
        time.sleep(1.2)
        target = OUTPUT / f"{platform}-font-{name}.png"
        run("screenshot", "--emulator", platform, "--no-open", "--no-correction", str(target))
        signature = target.read_bytes()[:24]
        size = (int.from_bytes(signature[16:20], "big"), int.from_bytes(signature[20:24], "big"))
        if signature[:8] != b"\x89PNG\r\n\x1a\n" or size != SIZES[platform]:
            raise RuntimeError(f"Unexpected capture: {target}: {size}")
        print(target.relative_to(ROOT))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("platform", choices=tuple(SIZES))
    capture(parser.parse_args().platform)
