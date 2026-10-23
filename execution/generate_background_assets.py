#!/usr/bin/env python3
"""Prepare original-concept aquarium plates for the two native Pebble screens."""

from __future__ import annotations

from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "assets/source"
OUTPUT = ROOT / "watchface/resources/images"
SCENES = ("castle", "kelp", "reef", "grayscale")
PLATFORMS = {"emery": (200, 228), "gabbro": (260, 260)}


def level(channel: float) -> int:
    return max(0, min(255, round(channel / 85) * 85))


def transform(pixel: tuple[int, int, int], scene: str, night: bool) -> tuple[int, int, int]:
    red, green, blue = pixel
    if scene == "grayscale":
        value = red * 0.299 + green * 0.587 + blue * 0.114
        value *= 0.45 if night else 1.0
        gray = level(value)
        return gray, gray, gray
    if night:
        factors = {
            "castle": (0.38, 0.48, 0.58),
            "kelp": (0.50, 0.59, 0.66),
            "reef": (0.60, 0.65, 0.76),
        }[scene]
        red, green, blue = (red * factors[0], green * factors[1], blue * factors[2])
    return level(red), level(green), level(blue)


def fit_scene(source: Image.Image, width: int, height: int) -> Image.Image:
    square = source.resize((height, height), Image.Resampling.BOX)
    left = (height - width) // 2
    return square.crop((left, 0, left + width, height))


def prepare() -> None:
    OUTPUT.mkdir(parents=True, exist_ok=True)
    for scene in SCENES:
        path = SOURCE / f"background-{scene}.png"
        if not path.is_file():
            raise SystemExit(f"Missing illustrated scene: {path}")
        with Image.open(path) as raw:
            source = raw.convert("RGB")
        for platform, (width, height) in PLATFORMS.items():
            fitted = fit_scene(source, width, height)
            if platform == "gabbro" and scene == "castle":
                # The detailed castle plate exceeds Gabbro's PNG decode heap
                # alongside the fish sprite. Coarser native pixel clusters
                # preserve the look while reducing compressed resource size.
                fitted = fitted.resize((130, 130), Image.Resampling.BOX)
                fitted = fitted.resize((width, height), Image.Resampling.NEAREST)
            for mode in ("day", "night"):
                image = Image.new("RGB", fitted.size)
                image.putdata(
                    [transform(pixel, scene, mode == "night")
                     for pixel in fitted.get_flattened_data()]
                )
                target = OUTPUT / f"background-{platform}-{scene}-{mode}.png"
                image.save(target, optimize=True)
                print(f"{target.relative_to(ROOT)}: {target.stat().st_size} bytes")


if __name__ == "__main__":
    prepare()
