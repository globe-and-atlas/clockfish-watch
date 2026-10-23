# Clockfish

A Pebble Time 2 and Pebble Round 2 watchface. Live time digits sit inside an illustrated pixel-art fish. It swims briefly after a tap or minute change, then rests in the center. Aquarium scenes include a castle tank, a leafy kelp forest, midnight reef, and grayscale. Small fish, a jellyfish, a crab, and bubbles appear around the tank. A separate small fish displays battery percentage.

## Build

```sh
cd watchface
npm install
pebble build
```

The PBW is `watchface/build/watchface.pbw`; the latest local preview is `release/Clockfish-0.6.0.pbw`. Its bundle contains emery and gabbro builds. Use `pebble install --emulator emery` or `pebble install --emulator gabbro` to inspect a native screen. `directives/build_watchface.md` has the complete verification path.

## Settings

- Scene: castle aquarium, kelp forest, midnight reef, grayscale
- Clock fish color: match aquarium, orange, blue, turquoise, grayscale, violet, sea green, random daily
- Background: match aquarium, castle, kelp forest, reef, grayscale, random daily
- Day/night: day, night, automatic at 19:00–07:00 local watch time
- Date: Off, YYYY-MM-DD, MM/DD/YYYY, DD/MM/YYYY
- Battery fish: on/off, with percentage and charge color
- Motion: short swims on tap and minute change, or still

Version 0.6.0 lets the fish and background be selected independently. Match aquarium keeps existing saved scenes looking the same after an upgrade. Random daily chooses each independently from the local calendar date and holds the choice until the next day. Live sea-life details remain visible in every aquarium. The numeral font remains live and fits within the broad fish body. Actual emulator screenshots are in `media/native/`; the original concept renders in `assets/reference/` are design references. Source art lives in `assets/source/`, and `python3 execution/generate_fish_assets.py` plus `python3 execution/generate_background_assets.py` prepare the 28 bundled Pebble images. Store publication and physical-device performance checks remain separate steps.
