---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-22T17:09:00-05:00"
---

# Build and inspect Clockfish

1. From `watchface/`, run `npm install` and `pebble build`.
2. Inspect `build/watchface.pbw` for `emery/` and `gabbro/` payloads.
3. Install one emulator target at a time. Capture with `pebble screenshot --no-open --no-correction`.
4. Confirm the active face is Clockfish in the captured image. An emulator may report install success yet show a stale face. If so, gracefully `pebble kill`, stop lingering `pypkjs` and `qemu-pebble` processes without SIGKILL, boot once, wait for firmware, and reinstall.
5. Run `python3 execution/capture_native.py <target>` for four scene/date combinations and battery levels. The script reads the generated key map from `watchface/build/js/message_keys.json`.
6. Check date and scene legibility at each native resolution before release.

Never commit `.lock-waf*`; those files can contain the complete shell environment.
