#!/usr/bin/env python3
"""Capture one Clockfish glide from an already running Pebble emulator."""

from __future__ import annotations

import argparse
import json
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WATCHFACE = ROOT / "watchface"
OUTPUT = ROOT / "media/native"
SIZES = {"emery": (200, 228), "gabbro": (260, 260)}


def run(*args: str) -> None:
    result = subprocess.run(
        ["pebble", *args], cwd=WATCHFACE, capture_output=True, text=True, check=False
    )
    if result.returncode:
        raise RuntimeError(f"pebble {' '.join(args)} failed: {result.stderr.strip()}")


def screenshot(platform: str, pose: str) -> Path:
    target = OUTPUT / f"{platform}-calm-{pose}.png"
    run("screenshot", "--emulator", platform, "--no-open", "--no-correction", str(target))
    signature = target.read_bytes()[:24]
    size = (int.from_bytes(signature[16:20], "big"), int.from_bytes(signature[20:24], "big"))
    if signature[:8] != b"\x89PNG\r\n\x1a\n" or size != SIZES[platform]:
        raise RuntimeError(f"Unexpected native capture: {target}: {size}")
    return target


def capture(platform: str) -> None:
    keys = json.loads((WATCHFACE / "build/js/message_keys.json").read_text())
    OUTPUT.mkdir(parents=True, exist_ok=True)
    run(
        "send-app-message", "--emulator", platform, "--int",
        f"{keys['SETTING_SCENE']}=0", f"{keys['SETTING_DATE']}=1",
        f"{keys['SETTING_MODE']}=0", f"{keys['SETTING_FISH_COLOR']}=0",
        f"{keys['SETTING_BACKGROUND']}=0", f"{keys['SETTING_MOTION']}=0",
    )
    print(screenshot(platform, "rest").relative_to(ROOT))
    run("send-app-message", "--emulator", platform, "--int", f"{keys['SETTING_MOTION']}=1")
    run("emu-tap", "--emulator", platform)
    for pose in ("early", "turn", "return", "settled"):
        print(screenshot(platform, pose).relative_to(ROOT))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("platform", choices=tuple(SIZES))
    capture(parser.parse_args().platform)
