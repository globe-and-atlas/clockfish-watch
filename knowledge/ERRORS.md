# Errors

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
