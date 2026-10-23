#!/usr/bin/env python3
"""Capture Clockfish scene/date combinations from an already running Pebble emulator."""

from __future__ import annotations

import argparse
import json
import subprocess
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WATCHFACE = ROOT / "watchface"
SCENES = ("castle", "kelp", "reef", "grayscale")
BATTERY = (74, 42, 18, 100)


def run(*args: str) -> None:
    result = subprocess.run(
        ["pebble", *args], cwd=WATCHFACE, capture_output=True, text=True, check=False
    )
    if result.returncode:
        raise RuntimeError(f"pebble {' '.join(args)} failed: {result.stderr.strip()}")


def capture(platform: str) -> None:
    keys = json.loads((WATCHFACE / "build/js/message_keys.json").read_text())
    output = ROOT / "media/native"
    output.mkdir(parents=True, exist_ok=True)
    for index, scene in enumerate(SCENES):
        run("emu-battery", "--emulator", platform, "--percent", str(BATTERY[index]))
        run("send-app-message", "--emulator", platform, "--int",
            f"{keys['SETTING_SCENE']}={index}", f"{keys['SETTING_DATE']}={index}",
            f"{keys['SETTING_MODE']}={1 if index == 2 else 0}",
            f"{keys['SETTING_FISH_COLOR']}=0", f"{keys['SETTING_BACKGROUND']}=0")
        time.sleep(1.0)
        target = output / f"{platform}-{scene}.png"
        run("screenshot", "--emulator", platform, "--no-open", "--no-correction", str(target))
        signature = target.read_bytes()[:24]
        expected = (200, 228) if platform == "emery" else (260, 260)
        size = (int.from_bytes(signature[16:20], "big"), int.from_bytes(signature[20:24], "big"))
        if signature[:8] != b"\x89PNG\r\n\x1a\n" or size != expected:
            raise RuntimeError(f"Unexpected screenshot format or dimensions: {target}: {size}")
        print(f"{target}: {size[0]}x{size[1]}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("platform", choices=("emery", "gabbro"))
    capture(parser.parse_args().platform)
