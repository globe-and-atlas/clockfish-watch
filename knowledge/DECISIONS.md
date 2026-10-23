# Decisions

## 2026-10-22: Native Pebble face

The template scaffold used `dashboard-static` because the template has no Pebble profile. The project profile is now `pebble-watch`; the deliverable is a native dual-platform PBW. The first versions drew scene elements in C instead of including high-resolution raster concept art, which would not faithfully represent the watch displays. Trigger short animation bursts on tap and minute change, then rest to preserve battery. A separate battery fish includes an explicit percentage so its meaning is clear.

## 2026-10-22: Local fish visual preview

Build the fish silhouette from native polygons with a forked tail, fins, gill, and eye. Keep the time centered in the body and leave a small pixel margin at both swim extremes. Scene decoration uses native shapes so its colors survive the Time 2 and Round 2 palettes. Release 0.2.0 is a local visual candidate; the illustrated aquarium style still needs wearer review before store publication.

## 2026-10-22: Illustrated aquarium direction

The wearer chose an illustrated aquarium with a clearly shaped fish. Version 0.3.0 rounds the body, separates the head and fins, gives the castle fish a warm orange body, and draws broader alternating kelp leaves. Keep the numerals clear on the real 200x228 and 260x260 screens before adding more detail. The still images are native emulator captures; they do not prove the revised swim animation or physical display behavior.

## 2026-10-22: Fish proportions before art asset work

The wearer found 0.3.0 too angular and the time crowded. Version 0.4.0 increases body height, uses denser contour points for the body and fins, reduces the time font, and narrows its text box so the face retains open space around the eye. Treat this as a native proportion study. Matching the earlier concept renderings more closely requires authored artwork at native target sizes.

## 2026-10-22: Native pixel-art resources for the concept style

The original four-panel concept and motion storyboard were recovered from this conversation and saved in `assets/reference/`. Version 0.5.0 derives a transparent fish cutout and four fish-free aquarium plates from that art direction, then quantizes them to Pebble's four channel levels at each target resolution. This replaces the polygonal fish and sparse procedural scenery in the usual rendering path. Keep time, date, and battery as live native text and shapes so they remain accurate and configurable. Retain vector scenery only as a fallback if a bitmap fails to load.

## 2026-10-22: Independent color and scenery controls

Keep the old scene setting as the base preset. New fish and background controls default to Match aquarium, preserving existing appearances after an upgrade. Six fish colors and four aquarium backgrounds can be mixed. Separate salted hashes choose fish and background for each local calendar day when Random daily is selected; this gives variety without a distracting change on every minute tick. Draw small native sea creatures and bubbles over every background, including grayscale, so these elements remain visible regardless of the art plate.
