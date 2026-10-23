---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-22T17:09:00-05:00"
---

# Dependencies

- Pebble SDK 4.33.1 is the local build environment used for the first dual-platform package. `pebble` CLI version is 5.0.40.
- `@rebble/clay` 1.1.0 is pinned in `watchface/package.json` for phone-side scene, date, motion, day/night, and battery controls. The checked-in lockfile records the resolved package.
- Direct Node imports of Clay expect its generated `tmp/config-page.html`, which is absent outside the Pebble webpack build. Unit tests mock the Clay constructor and test Clockfish's settings conversion separately; `pebble build` verifies packaging.
- Pillow 12.3.0 prepares the 24 native pixel-art PNG resources from the saved concept-derived source art. It is a development-time dependency only; Pebble does not run Python. The two asset scripts require Pillow before a rebuild of the artwork.
