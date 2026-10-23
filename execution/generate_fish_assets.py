#!/usr/bin/env python3
"""Prepare native Pebble fish sprites from the approved concept-style cutout."""

from __future__ import annotations

import colorsys
from pathlib import Path

from PIL import Image, ImageOps

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "assets/source/fish-blue-generated.png"
CONTRAST_SOURCE = ROOT / "assets/source/fish-blue-contrast-generated.png"
OUTPUT = ROOT / "watchface/resources/images"
SIZES = {"emery": (184, 100), "gabbro": (216, 116)}
SCENES = ("castle", "kelp", "reef", "grayscale", "violet", "sea-green")


def pebble_level(value: int) -> int:
    return max(0, min(255, round(value / 85) * 85))


def recolor(pixel: tuple[int, int, int, int], scene: str) -> tuple[int, int, int, int]:
    red, green, blue, alpha = pixel
    if alpha < 32:
        return 0, 0, 0, 0
    if scene == "grayscale":
        gray = round(red * 0.299 + green * 0.587 + blue * 0.114)
        red = green = blue = gray
    elif scene in ("castle", "reef", "violet", "sea-green"):
        hue, saturation, value = colorsys.rgb_to_hsv(red / 255, green / 255, blue / 255)
        if saturation > 0.28 and blue > red * 1.15 and value > 0.12:
            new_hue = {
                "castle": 0.07, "reef": 0.52, "violet": 0.76, "sea-green": 0.39,
            }[scene]
            red, green, blue = (
                round(channel * 255)
                for channel in colorsys.hsv_to_rgb(new_hue, saturation, value)
            )
    return (
        pebble_level(red),
        pebble_level(green),
        pebble_level(blue),
        pebble_level(alpha),
    )


def load_cutout(path: Path) -> Image.Image:
    if not path.is_file():
        raise SystemExit(f"Missing generated fish cutout: {path}")
    with Image.open(path) as raw:
        source = raw.convert("RGBA")
    alpha = source.getchannel("A")
    visible = alpha.point(lambda value: 255 if value > 32 else 0).getbbox()
    if visible is None:
        raise SystemExit("Fish cutout is fully transparent")
    return source.crop(visible)


def fit_cutout(source: Image.Image, width: int, height: int) -> Image.Image:
    scale = min(width / source.width, height / source.height)
    size = (round(source.width * scale), round(source.height * scale))
    reduced = source.resize(size, Image.Resampling.BOX)
    canvas = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    canvas.alpha_composite(reduced, ((width - size[0]) // 2, (height - size[1]) // 2))
    return canvas


def prepare() -> None:
    OUTPUT.mkdir(parents=True, exist_ok=True)
    original = load_cutout(SOURCE)
    contrast = load_cutout(CONTRAST_SOURCE)
    for platform, (width, height) in SIZES.items():
        canvases = {
            "original": fit_cutout(original, width, height),
            "contrast": fit_cutout(contrast, width, height),
        }
        for scene in SCENES:
            canvas = canvases["contrast" if scene in ("kelp", "violet") else "original"]
            image = Image.new("RGBA", canvas.size)
            image.putdata([recolor(pixel, scene) for pixel in canvas.get_flattened_data()])
            target = OUTPUT / f"fish-{platform}-{scene}.png"
            image.save(target, optimize=True)
            left_target = OUTPUT / f"fish-{platform}-{scene}-left.png"
            ImageOps.mirror(image).save(left_target, optimize=True)
            if image.getbbox() is None:
                raise RuntimeError(f"Empty fish sprite: {target}")
            print(f"{target.relative_to(ROOT)}: {width}x{height}, {target.stat().st_size} bytes")
            print(f"{left_target.relative_to(ROOT)}: {width}x{height}, {left_target.stat().st_size} bytes")


if __name__ == "__main__":
    prepare()
