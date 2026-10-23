# Decisions

## 2026-10-22: Native Pebble face

The template scaffold used `dashboard-static` because the template has no Pebble profile. The project profile is now `pebble-watch`; the deliverable is a native dual-platform PBW. Draw scene elements in C instead of including high-resolution raster concept art, which would not faithfully represent the watch displays. Trigger short animation bursts on tap and minute change, then rest to preserve battery. A separate battery fish includes an explicit percentage so its meaning is clear.

## 2026-10-22: Local fish visual preview

Build the fish silhouette from native polygons with a forked tail, fins, gill, and eye. Keep the time centered in the body and leave a small pixel margin at both swim extremes. Scene decoration uses native shapes so its colors survive the Time 2 and Round 2 palettes. Release 0.2.0 is a local visual candidate; the illustrated aquarium style still needs wearer review before store publication.

## 2026-10-22: Illustrated aquarium direction

The wearer chose an illustrated aquarium with a clearly shaped fish. Version 0.3.0 rounds the body, separates the head and fins, gives the castle fish a warm orange body, and draws broader alternating kelp leaves. Keep the numerals clear on the real 200x228 and 260x260 screens before adding more detail. The still images are native emulator captures; they do not prove the revised swim animation or physical display behavior.

## 2026-10-22: Fish proportions before art asset work

The wearer found 0.3.0 too angular and the time crowded. Version 0.4.0 increases body height, uses denser contour points for the body and fins, reduces the time font, and narrows its text box so the face retains open space around the eye. Treat this as a native proportion study. Matching the earlier concept renderings more closely likely requires authored curved artwork at native target sizes; those rendering source files are not in the project yet.
