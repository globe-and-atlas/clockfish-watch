# Decisions

## 2026-10-22: Native Pebble face

The template scaffold used `dashboard-static` because the template has no Pebble profile. The project profile is now `pebble-watch`; the deliverable is a native dual-platform PBW. Draw scene elements in C instead of including high-resolution raster concept art, which would not faithfully represent the watch displays. Trigger short animation bursts on tap and minute change, then rest to preserve battery. A separate battery fish includes an explicit percentage so its meaning is clear.
