# Clockfish

A Pebble Time 2 and Pebble Round 2 watchface. Live rounded time digits sit inside an illustrated pixel-art fish. The fish alternates between facing left and right during brief glides; the digits stay upright. Aquarium scenes include a castle tank, a leafy kelp forest, midnight reef, and grayscale. Small fish, a jellyfish, a crab, and bubbles move in short ambient bursts. A separate stationary fish displays battery percentage.

## Build

```sh
cd watchface
npm install
pebble build
```

The PBW is `watchface/build/watchface.pbw`; the latest local preview is `release/Clockfish-0.7.0.pbw`. Its bundle contains emery and gabbro builds. Use `pebble install --emulator emery` or `pebble install --emulator gabbro` to inspect a native screen. `directives/build_watchface.md` has the complete verification path.

## Settings

- Scene: castle aquarium, kelp forest, midnight reef, grayscale
- Clock fish color: match aquarium, orange, blue, turquoise, grayscale, violet, sea green, random daily
- Background: match aquarium, castle, kelp forest, reef, grayscale, random daily
- Day/night: day, night, automatic at 19:00–07:00 local watch time
- Date: Off, YYYY-MM-DD, MM/DD/YYYY, DD/MM/YYYY
- Battery fish: on/off, with percentage and charge color
- Motion: Still, Gentle, or Lively. Gentle and Lively animate aquarium life in short bursts; Lively does so more often. Both modes allow tap glides and occasional automatic clock-fish glides.

Version 0.7.0 keeps the independent fish/background choices and daily random selection from 0.6.0. It adds short ambient motion, alternating left/right clock-fish sprites, and a rounder live time face. The blue fish has a brighter body against dark kelp water. Existing Motion Off settings become Still; Motion On becomes Gentle. Actual emulator screenshots are in `media/native/`; the original concept renders in `assets/reference/` are design references. Source art lives in `assets/source/`, and `python3 execution/generate_fish_assets.py` plus `python3 execution/generate_background_assets.py` prepare the bundled Pebble images. Store publication and physical-device performance checks remain separate steps.
