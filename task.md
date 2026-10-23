# Clockfish watchface

## Curved fish and time fit — 2026-10-22

- [x] The fish body has a smoothly rounded silhouette at 200x228.
- [x] The fish body has a smoothly rounded silhouette at 260x260.
- [x] The clock digits stay inside the body boundary at 200x228.
- [x] The clock digits stay inside the body boundary at 260x260.
- [x] The eye has visible space beyond the final time digit on emery.
- [x] The eye has visible space beyond the final time digit on gabbro.
- [ ] The tail and fins use curved contours on both targets.
- [x] The date remains readable in the four scene captures on emery.
- [x] The date remains readable in the four scene captures on gabbro.
- [x] A fresh 0.4.0 PBW includes emery.
- [x] A fresh 0.4.0 PBW includes gabbro.
- [x] A fresh verifier inspects the final native captures.

This pass addresses the wearer's direct visual critique: the numerals must read as part of the fish's body, and the fish must feel less angular. The target is a local visual preview; physical motion and store publication remain separate checks.

The proportion and spacing checks pass. The fin/tail contour criterion remains open: native GPath polygons still show straight facets. The original concept render has now been located in this chat and will guide a bitmap sprite pass.

## Illustrated aquarium pass — 2026-10-22

- [x] The main fish has a rounded body silhouette at 200x228.
- [x] The main fish has a distinct tail at 200x228.
- [x] The main fish has a distinct eye at 200x228.
- [x] The time is readable inside the main fish on emery.
- [x] The time is readable inside the main fish on gabbro.
- [x] The kelp scene shows broad alternating leaves on emery.
- [x] The kelp scene shows broad alternating leaves on gabbro.
- [x] The castle scene retains a visible castle on emery.
- [x] The reef scene retains visible coral on emery.
- [x] The grayscale scene keeps readable time on both targets.
- [x] All four date choices remain in phone settings.
- [x] A 0.3.0 PBW contains emery and gabbro builds.
- [x] Fresh native screenshots show all four scenes on emery.
- [x] Fresh native screenshots show all four scenes on gabbro.
- [x] A fresh verifier reviews the illustrated aquarium pass.

This pass is a local visual candidate for the illustrated aquarium direction chosen by the wearer. Store publication remains a separate decision after native and physical watch review.

## Aquarium visual rebuild — 2026-10-22

- [x] The clock body has a tapered fish silhouette.
- [x] The clock fish has a forked tail.
- [x] The clock fish has a visible eye.
- [x] The clock fish has a visible gill.
- [x] The time remains readable inside the fish on emery.
- [x] The time remains readable inside the fish on gabbro.
- [x] The castle scene has visible masonry detail.
- [x] The kelp scene has layered leaf shapes.
- [x] The reef scene has distinct coral shapes.
- [x] The grayscale scene retains legible contrast.
- [x] The clock fish remains fully inside the emery screen during its swim.
- [x] The clock fish remains fully inside the gabbro screen during its swim.
- [x] The date remains legible in each of the four scenes on emery.
- [x] The date remains legible in each of the four scenes on gabbro.
- [x] Native emery screenshots are inspected at 200x228.
- [x] Native gabbro screenshots are inspected at 260x260.
- [x] A fresh verifier reviews the rebuilt face.

The release boundary for this pass is a local preview. The target is a native dual-platform PBW and updated screenshots in `media/native/`; store publication follows a visual review. The build path is `directives/build_watchface.md` and `execution/capture_native.py`. Generated scratch files stay in `.tmp/`.

The independent review accepted this as a local preview and noted that kelp still reads as sparse reeds at watch size. A richer aquarium art pass and physical watch check remain before a store release decision.

## Contract
- Target: Pebble Time 2 (emery) and Pebble Round 2 (gabbro).
- Output: `release/Clockfish-0.2.0.pbw`.
- Safety: transient outputs stay in `.tmp/`.

## Validation contract
- [x] The PBW contains an emery build.
- [x] The PBW contains a gabbro build.
- [x] The displayed time is readable within the clock fish.
- [x] The clock fish remains wholly visible during its swim.
- [x] The scene selector offers aquarium, kelp forest, midnight reef, and grayscale.
- [x] The date selector offers Off.
- [x] The date selector offers YYYY-MM-DD.
- [x] The date selector offers MM/DD/YYYY.
- [x] The date selector offers DD/MM/YYYY.
- [x] The battery fish shows a percentage.
- [x] A tap starts a bounded swim animation.
- [x] The face rests without continuous animation.
- [x] A fresh verifier inspects the output against this contract.
- [x] Native emery output is inspected.
- [x] Native gabbro output is inspected.

## Progress
- 2026-10-22: Project scaffold created; implementing first working face.
- 2026-10-22: Built the PBW for both platforms; inspected nine native screenshots, verified a tap shifts the fish and it settles, and received independent review with no concrete release-blocking defect.
