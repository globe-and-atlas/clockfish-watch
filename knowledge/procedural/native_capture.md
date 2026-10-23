---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-22T17:17:00-05:00"
---

# Native capture workflow

Build in `watchface/` with `pebble build`. Install one emulator target, confirm Clockfish is the foreground face, then run `python3 execution/capture_native.py emery` or `python3 execution/capture_native.py gabbro` from the project root. The script sends four scene/date settings and battery levels, captures exact palette PNGs, and validates target dimensions. It does not prove the foreground app is Clockfish; inspect the images before treating them as evidence. A tap screenshot taken immediately after `pebble emu-tap` can demonstrate movement; a later screenshot should show the centered resting position.

Rapid screenshots during the 180 ms swim can catch an incomplete redraw with missing static details. Capture more than one motion frame and a settled frame, then inspect them before inferring an animation defect. If a stale face or error screen appears after install, stop QEMU gracefully, preserve its state under `.tmp/`, boot once, wait for firmware, and reinstall before recapturing.

For the illustrated version, regenerate native art with `python3 execution/generate_fish_assets.py` and `python3 execution/generate_background_assets.py` before `pebble clean && pebble build`. Each generated RGB/RGBA channel uses only 0, 85, 170, or 255; `python3 -m unittest tests/test_art_assets.py` checks dimensions, palette, and fish transparency. The native screenshots, not the source illustrations, are the evidence for legibility and clipping.

On Gabbro, inspect the castle day scene immediately after install. A more detailed 260x260 castle plate failed to allocate PNG decode memory even though the PBW built successfully. The asset generator uses coarser pixel clusters for that plate, and its test guards the resulting resource size. If a scene falls back to vector art, inspect `pebble logs --emulator gabbro` for PNG allocation errors before trusting the capture.
