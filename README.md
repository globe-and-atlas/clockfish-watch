# Clockfish

A Pebble Time 2 and Pebble Round 2 watchface. Live time digits sit inside an illustrated pixel-art fish, with six selectable numeral styles. The fish alternates between facing left and right during brief glides; the digits stay upright. Aquarium scenes include a castle tank, a leafy kelp forest, midnight reef, and grayscale. Small fish, a jellyfish, a crab, and bubbles move in short ambient bursts. A separate stationary fish displays battery percentage.

## Build

```sh
cd watchface
npm install
pebble build
```

Clockfish is [listed on the Pebble Appstore](https://apps.repebble.com/40292564f42e4f4e8763b9a1) for Pebble Time 2 and Pebble Round 2. The latest local PBW is `release/Clockfish-0.8.2.pbw`; the build output is `watchface/build/watchface.pbw`. Use `pebble install --emulator emery` or `pebble install --emulator gabbro` to inspect a native screen. `directives/build_watchface.md` has the complete verification path.

## Settings

- Scene: castle aquarium, kelp forest, midnight reef, grayscale
- Clock fish color: match aquarium, orange, blue, turquoise, grayscale, violet, sea green, random daily
- Background: match aquarium, castle, kelp forest, reef, grayscale, random daily
- Time digits: Rounded, Digital bold, Digital fine, Classic bold, Compact bold, Tall light. Rounded remains the default for existing installs.
- Day/night: day, night, automatic at 19:00–07:00 local watch time
- Date: Off, YYYY-MM-DD, MM/DD/YYYY, DD/MM/YYYY
- Battery fish: on/off, with percentage and charge color
- Motion: Still, Gentle, or Lively. Gentle and Lively animate aquarium life in short bursts; Lively does so more often. Both modes allow tap glides and occasional automatic clock-fish glides.

Version 0.8.2 gives the battery percentage more room inside its fish. Version 0.8.1 centers the Tall light digits in the main fish. Version 0.8.0 adds six time-digit styles. Version 0.7.1 slowed ambient bursts and makes each clock-fish glide follow its facing direction, turn near the edge, and return. The time remains centered in the fish body during the turn. Gentle runs a brief ambient burst roughly every 33 seconds and an automatic glide roughly every ten minutes; Lively runs a burst roughly every 17 seconds and a glide roughly every five minutes. These timings are modeled, not physical power measurements. Existing Motion Off settings become Still; Motion On becomes Gentle. Actual emulator screenshots are in `media/native/`; store GIFs are in `media/store/`. The original concept renders in `assets/reference/` are design references. Source art lives in `assets/source/`, and `python3 execution/generate_fish_assets.py` plus `python3 execution/generate_background_assets.py` prepare the bundled Pebble images. Physical-device motion and battery performance remain unverified.
