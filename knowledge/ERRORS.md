# Errors

## 2026-10-22: Blue fish disappeared into kelp water

- **Error:** The blue fish body and dark kelp water had the same native Pebble RGB value.
- **Cause:** Quantizing the original blue cutout mapped both surfaces to (0,0,85).
- **Fix:** Use a brighter cobalt source for blue and violet fish sprites, then verify the body pixel and native emulator contrast.
- **Graduated to:** `execution/generate_fish_assets.py` and `tests/test_art_assets.py`.

## 2026-10-22: Date contrast on emulator

- **Error:** The Time 2 date was unreadable over some seabed colors.
- **Cause:** One date ink color was reused across scenes with different seabed brightness.
- **Fix:** Choose date ink by scene and day/night mode; inspect native screenshots for the castle, kelp, reef, and grayscale scenes.
- **Graduated to:** `directives/build_watchface.md` requires native scene/date inspection.

## 2026-10-22: Health check misread knowledge index

- **Error:** The health check warned that `knowledge/INDEX.md` had no entries even though its table contained several.
- **Cause:** The parser accepted list entries but not table rows.
- **Fix:** Accept both `- [` and `| [` entry prefixes.
- **Graduated to:** `scripts/health_check.py`.

## 2026-10-22: Direct Clay import in Node test

- **Error:** A direct test import of `src/pkjs/index.js` failed because Clay expected `tmp/config-page.html`.
- **Cause:** That generated file is supplied by Pebble's build workflow, not a plain Node `require`.
- **Fix:** Mock the Clay constructor in the settings unit test; separately run `pebble build` to verify the actual bundle.
- **Graduated to:** `knowledge/domain/deps.md`.

## 2026-10-22: Nested screenshot temp files staged

- **Error:** `git add -A` staged `watchface/.tmp/native/` screenshots.
- **Cause:** The template ignore rule covered only the root `.tmp/` directory.
- **Fix:** Ignore nested `.tmp/` directories and remove staged screenshots from the index.
- **Graduated to:** `.gitignore`.

## 2026-10-22: Stale Round 2 emulator passed screenshot size checks

- **Error:** A successful Clockfish install and 260x260 screenshots still showed a previous Way Back error screen or blank frame.
- **Cause:** The shared Gabbro emulator retained an invalid prior app state; dimensions alone could not identify the active face.
- **Fix:** Stop QEMU gracefully, preserve the Gabbro SDK state under ignored `.tmp/`, boot a fresh instance, reinstall, and visually inspect the first capture before trusting the batch. The capture script now names the target emulator explicitly.
- **Graduated to:** `directives/build_watchface.md` and `execution/capture_native.py`.

## 2026-10-22: Swim screenshots caught incomplete redraws

- **Error:** Some rapid screenshots during the 180 ms swim lacked the date or battery fish even though settled frames contained them.
- **Cause:** Emulator screenshots can arrive during the framebuffer redraw; a single mid-animation frame cannot establish the complete composition.
- **Fix:** Inspect several frames and a settled frame. Treat incomplete transient captures as capture artifacts until reproduced visually on a physical watch.
- **Graduated to:** `knowledge/procedural/native_capture.md`.

## 2026-10-22: Round 2 castle PNG exceeded decode heap

- **Error:** Gabbro castle day fell back to the old vector background while the other illustrated scenes loaded; emulator logs repeated `PNG memory allocation failed`.
- **Cause:** The detailed 260x260 castle plate needed more decode memory alongside the fish sprite than Gabbro could provide. The original generated PNG was about 16 KB.
- **Fix:** Generate the Gabbro castle plate with 2x native pixel clusters. Its day PNG is about 8.3 KB and the fresh native capture now shows the illustrated castle. Keep the generator's size regression test and inspect cold-launch captures.
- **Graduated to:** `execution/generate_background_assets.py`, `tests/test_art_assets.py`, and `knowledge/procedural/native_capture.md`.

## 2026-10-23: Font capture used a stale clock value

- **Error:** A few 0.8.0 font screenshots showed the emulator's current time while neighboring frames showed the requested 23:58; the first GIF assembly would jump during a font change.
- **Cause:** The time-setting command returned before the watch face had rendered the new minute.
- **Fix:** Wait after setting emulator time, inspect the six-frame sheets, and recapture any mismatched frame before assembling and uploading a GIF. Emery Compact bold and Gabbro Digital fine were recaptured at 23:58; both final public GIF assets were checked after upload.
- **Graduated to:** `execution/capture_fonts.py` and `knowledge/procedural/native_capture.md`.

## 2026-10-23: Tall light digits sat low in the fish

- **Error:** Tall light looked lower than the other time styles in native 23:58 previews.
- **Cause:** Bitham 42 Light has different vertical glyph metrics even when drawn in the same Pebble text rectangle.
- **Fix:** Shift only Tall light's bitmap-sprite time overlay up 7 px on Emery and 6 px on Gabbro. Compare visible white-pixel bounds with Rounded in native captures, then inspect the fish outline.
- **Graduated to:** `watchface/src/c/main.c` and `knowledge/procedural/native_capture.md`.
