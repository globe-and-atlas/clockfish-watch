"""Check packaged native art constraints that the Pebble build cannot explain."""

from pathlib import Path
import unittest

from PIL import Image, ImageOps


ROOT = Path(__file__).resolve().parents[1]
IMAGES = ROOT / "watchface/resources/images"
SIZES = {"emery": (200, 228), "gabbro": (260, 260)}
SCENES = ("castle", "kelp", "reef", "grayscale")
FISH_COLORS = SCENES + ("violet", "sea-green")


class ArtAssetTests(unittest.TestCase):
    def test_backgrounds_fit_native_screens_and_palette(self) -> None:
        for platform, size in SIZES.items():
            for scene in SCENES:
                for mode in ("day", "night"):
                    path = IMAGES / f"background-{platform}-{scene}-{mode}.png"
                    with self.subTest(path=path), Image.open(path) as image:
                        self.assertEqual(image.size, size)
                        self.assertEqual(image.mode, "RGB")
                        self.assertTrue(all(
                            channel in (0, 85, 170, 255)
                            for pixel in image.get_flattened_data()
                            for channel in pixel
                        ))

    def test_fish_sprites_have_transparency_and_native_palette(self) -> None:
        for platform, size in (("emery", (184, 100)), ("gabbro", (216, 116))):
            for scene in FISH_COLORS:
                path = IMAGES / f"fish-{platform}-{scene}.png"
                with self.subTest(path=path), Image.open(path) as image:
                    self.assertEqual(image.size, size)
                    self.assertEqual(image.mode, "RGBA")
                    alphas = image.getchannel("A").getextrema()
                    self.assertEqual(alphas[0], 0)
                    self.assertEqual(alphas[1], 255)
                    self.assertTrue(all(
                        channel in (0, 85, 170, 255)
                        for pixel in image.get_flattened_data()
                        for channel in pixel
                    ))

    def test_left_facing_fish_sprites_mirror_right_facing_art(self) -> None:
        for platform in SIZES:
            for scene in FISH_COLORS:
                right = IMAGES / f"fish-{platform}-{scene}.png"
                left = IMAGES / f"fish-{platform}-{scene}-left.png"
                with self.subTest(platform=platform, scene=scene), \
                     Image.open(right) as right_image, Image.open(left) as left_image:
                    self.assertEqual(left_image.size, right_image.size)
                    self.assertEqual(left_image.tobytes(), ImageOps.mirror(right_image).tobytes())

    def test_round_castle_plate_stays_within_decode_budget(self) -> None:
        # A 12 KB castle plate failed on Gabbro with PNG allocation errors.
        path = IMAGES / "background-gabbro-castle-day.png"
        self.assertLess(path.stat().st_size, 9_000)

    def test_blue_body_separates_from_dark_kelp_water(self) -> None:
        for platform, center in (("emery", (92, 50)), ("gabbro", (108, 58))):
            path = IMAGES / f"fish-{platform}-kelp.png"
            with self.subTest(platform=platform), Image.open(path) as image:
                body = image.convert("RGBA").getpixel(center)
                self.assertEqual(body[3], 255)
                self.assertNotEqual(body[:3], (0, 0, 85))
                self.assertGreaterEqual(body[1] + body[2], 170)


if __name__ == "__main__":
    unittest.main()
