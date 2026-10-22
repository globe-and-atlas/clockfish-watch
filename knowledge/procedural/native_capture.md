---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-22T17:17:00-05:00"
---

# Native capture workflow

Build in `watchface/` with `pebble build`. Install one emulator target, confirm Clockfish is the foreground face, then run `python3 execution/capture_native.py emery` or `python3 execution/capture_native.py gabbro` from the project root. The script sends four scene/date settings and battery levels, captures exact palette PNGs, and validates target dimensions. It does not prove the foreground app is Clockfish; inspect the images before treating them as evidence. A tap screenshot taken immediately after `pebble emu-tap` can demonstrate movement; a later screenshot should show the centered resting position.
