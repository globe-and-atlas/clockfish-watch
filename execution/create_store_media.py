#!/usr/bin/env python3
"""Assemble native emulator captures into RePebble preview GIFs."""

from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
NATIVE = ROOT / "media/native"
OUTPUT = ROOT / "media/store"
SIZES = {"emery": (200, 228), "gabbro": (260, 260)}
FRAMES = (
    ("rounded-right", 900),
    ("rounded-left", 900),
    ("ambient-color-0", 180),
    ("ambient-color-1", 180),
    ("ambient-color-0", 180),
    ("kelp", 900),
    ("reef", 900),
    ("grayscale", 900),
)


def create(platform: str) -> Path:
    expected = SIZES[platform]
    frames = []
    for suffix, _ in FRAMES:
        source = NATIVE / f"{platform}-{suffix}.png"
        with Image.open(source) as image:
            if image.size != expected:
                raise ValueError(f"{source} is {image.size}, expected {expected}")
            frames.append(image.convert("RGB"))
    OUTPUT.mkdir(parents=True, exist_ok=True)
    target = OUTPUT / f"{platform}-clockfish-0.7.0.gif"
    frames[0].save(
        target,
        save_all=True,
        append_images=frames[1:],
        duration=[duration for _, duration in FRAMES],
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
    for platform_name in SIZES:
        result = create(platform_name)
        print(f"{result.relative_to(ROOT)}: {result.stat().st_size} bytes")
