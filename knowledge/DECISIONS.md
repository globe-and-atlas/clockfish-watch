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

## 2026-10-22: Living aquarium motion and readable fish directions

Use Still, Gentle, and Lively rather than an on/off motion toggle. Gentle and Lively make short ambient bursts with bubbles, small fish, jellyfish, and occasional crab movement; Lively starts bursts more often. An occasional glide or tap alternates the clock fish between mirrored left and right sprites. Keep the live time text upright, and keep the battery fish anchored, because both readings matter more than rotating the artwork through every angle. Use the rounder Bitham face at 30px on Emery and 34px on Gabbro to give the time more interior space. The blue source fish uses a brighter cobalt body so its silhouette separates from dark kelp water.

## 2026-10-22: Calmer wrist motion

A three-second glance beginning at a uniformly random phase of the ambient cycle is the comparison model. With six redraw frames per burst, the old Gentle/Lively settings overlap an estimated 25%/39% of glances. Version 0.7.1 reduces those estimates to 18%/29% by spacing bursts to about 33/17 seconds. It also spaces automatic clock-fish glides to about 9.8/5.1 minutes. These are design estimates, not measured wearer attention, battery use, or physical-watch behavior. Keep tap glides available. A glide begins in the fish's current facing direction, turns at its outermost point, and returns with smaller horizontal and vertical travel. Keep the live time at a fixed body offset while the sprite turns; keep the battery fish stationary. Ambient bubble travel wraps across two 27-pixel spacings to maintain the alternating bubble pattern between bursts.

## 2026-10-23: Selectable time fonts

Use six Pebble system-font mappings for the live time so font selection adds no packaged font resources. Keep the 0.7.1 Bitham mapping at index 0 for saved-setting compatibility. Use target-specific sizes where needed: a 42-point bold Bitham fits Round 2, while Time 2 needs the smaller 34-point medium numeral font for the same choice. Select, validate, and persist the font independently of scene and motion. Native 23:58 captures on both targets define the width check; emulator restart confirms the stored choice survives relaunch.
