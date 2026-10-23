# Decisions

## 2026-10-22: Native Pebble face

The template scaffold used `dashboard-static` because the template has no Pebble profile. The project profile is now `pebble-watch`; the deliverable is a native dual-platform PBW. Draw scene elements in C instead of including high-resolution raster concept art, which would not faithfully represent the watch displays. Trigger short animation bursts on tap and minute change, then rest to preserve battery. A separate battery fish includes an explicit percentage so its meaning is clear.

## 2026-10-22: Local fish visual preview

Build the fish silhouette from native polygons with a forked tail, fins, gill, and eye. Keep the time centered in the body and leave a small pixel margin at both swim extremes. Scene decoration uses native shapes so its colors survive the Time 2 and Round 2 palettes. Release 0.2.0 is a local visual candidate; the illustrated aquarium style still needs wearer review before store publication.
