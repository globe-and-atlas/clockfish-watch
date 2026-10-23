# Clockfish

A Pebble Time 2 and Pebble Round 2 watchface. Live time digits sit inside an illustrated pixel-art fish. It swims briefly after a tap or minute change, then rests in the center. Aquarium scenes include a castle tank, a leafy kelp forest, midnight reef, and grayscale. A small fish displays battery percentage.

## Build

```sh
cd watchface
npm install
pebble build
```

The PBW is `watchface/build/watchface.pbw`; the latest local preview is `release/Clockfish-0.5.0.pbw`. Its bundle contains emery and gabbro builds. Use `pebble install --emulator emery` or `pebble install --emulator gabbro` to inspect a native screen. `directives/build_watchface.md` has the complete verification path.

## Settings

- Scene: castle aquarium, kelp forest, midnight reef, grayscale
- Day/night: day, night, automatic at 19:00–07:00 local watch time
- Date: Off, YYYY-MM-DD, MM/DD/YYYY, DD/MM/YYYY
- Battery fish: on/off, with percentage and charge color
- Motion: short swims on tap and minute change, or still

Version 0.5.0 replaces the angular vector fish and sparse scenery with native-sized pixel-art sprites and aquarium plates based on the original concept renders in `assets/reference/`. The numeral font remains live and fits within the broad fish body. Actual emulator screenshots are in `media/native/`; the concept renders are design references. Source art lives in `assets/source/`, and `python3 execution/generate_fish_assets.py` plus `python3 execution/generate_background_assets.py` prepare the 24 bundled Pebble images. The swim was observed on both emulators in the prior 0.2.0 pass; 0.5.0 artwork was checked in static native captures. Store publication and physical-device performance checks remain separate steps.
