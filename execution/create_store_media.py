#!/usr/bin/env python3
"""Assemble native emulator captures into RePebble preview GIFs."""

import argparse
from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
NATIVE = ROOT / "media/native"
OUTPUT = ROOT / "media/store"
SIZES = {"emery": (200, 228), "gabbro": (260, 260)}
FRAMES_070 = (
    ("rounded-right", 900),
    ("rounded-left", 900),
    ("ambient-color-0", 180),
    ("ambient-color-1", 180),
    ("ambient-color-0", 180),
    ("kelp", 900),
    ("reef", 900),
    ("grayscale", 900),
)
FRAMES_071 = (
    ("calm-rest", 900),
    ("calm-early", 400),
    ("calm-turn", 400),
    ("calm-return", 400),
    ("calm-settled", 800),
    ("kelp", 900),
    ("reef", 900),
    ("grayscale", 900),
)
VERSIONS = {"0.7.0": FRAMES_070, "0.7.1": FRAMES_071}


def create(platform: str, version: str) -> Path:
    expected = SIZES[platform]
    frames_spec = VERSIONS[version]
    frames = []
    for suffix, _ in frames_spec:
        source = NATIVE / f"{platform}-{suffix}.png"
        with Image.open(source) as image:
            if image.size != expected:
                raise ValueError(f"{source} is {image.size}, expected {expected}")
            frames.append(image.convert("RGB"))
    OUTPUT.mkdir(parents=True, exist_ok=True)
    target = OUTPUT / f"{platform}-clockfish-{version}.gif"
    frames[0].save(
        target,
        save_all=True,
        append_images=frames[1:],
        duration=[duration for _, duration in frames_spec],
        loop=0,
        optimize=True,
        disposal=2,
    )
    with Image.open(target) as preview:
        if preview.size != expected or preview.n_frames < 6:
            raise ValueError(f"Invalid store GIF: {target}")
    if target.stat().st_size >= 4_400_000:
        raise ValueError(f"Store GIF exceeds the RePebble 4.4 MB limit: {target}")
    return target


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", choices=tuple(VERSIONS), default="0.7.1")
    args = parser.parse_args()
    for platform_name in SIZES:
        result = create(platform_name, args.version)
        print(f"{result.relative_to(ROOT)}: {result.stat().st_size} bytes")
