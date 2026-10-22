# Clockfish

A Pebble Time 2 and Pebble Round 2 watchface. The clock fish swims briefly after a tap or minute change, then rests in the center. Aquarium scenes include a castle tank, kelp forest, midnight reef, and grayscale. A small fish displays battery percentage.

## Build

```sh
cd watchface
npm install
pebble build
```

The PBW is `watchface/build/watchface.pbw`; the checked-in copy is `release/Clockfish-0.1.0.pbw`. Its bundle contains emery and gabbro builds. Use `pebble install --emulator emery` or `pebble install --emulator gabbro` to inspect a native screen. `directives/build_watchface.md` has the complete verification path.

## Settings

- Scene: castle aquarium, kelp forest, midnight reef, grayscale
- Day/night: day, night, automatic at 19:00–07:00 local watch time
- Date: Off, YYYY-MM-DD, MM/DD/YYYY, DD/MM/YYYY
- Battery fish: on/off, with percentage and charge color
- Motion: short swims on tap and minute change, or still

This is a working first version. The concept renders in the discussion are artwork, not screenshots of this build. Actual emulator screenshots are in `media/native/`. The swim was observed on both emulators. Store publication and physical-device performance checks remain separate steps.
