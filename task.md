# Clockfish watchface

## Battery-fish percentage fit — 2026-10-23

- [x] The 100% label has visible padding inside the battery fish on Time 2.
- [x] The 100% label has visible padding inside the battery fish on Round 2.
- [x] The battery fish remains within the Time 2 display boundary.
- [x] The battery fish remains within the Round 2 display boundary.
- [x] The battery label remains readable at 50% on Time 2.
- [x] The battery label remains readable at 50% on Round 2.
- [x] The battery label remains readable at 5% on Time 2.
- [x] The battery label remains readable at 5% on Round 2.
- [x] The battery fish remains distinct in the Time 2 grayscale scene.
- [x] The battery fish remains distinct in the Round 2 grayscale scene.
- [x] The native dual-target build succeeds.
- [x] The Time 2 store GIF shows the final battery fish.
- [x] The Round 2 store GIF shows the final battery fish.
- [x] The public Time 2 gallery leads with the 0.8.2 GIF.
- [x] The public Round 2 gallery leads with the 0.8.2 GIF.
- [x] RePebble lists version 0.8.2 for Time 2.
- [x] RePebble lists version 0.8.2 for Round 2.

## Tall-light time placement — 2026-10-23

- [x] Tall-light digits are vertically centered within the fish body on Time 2.
- [x] Tall-light digits are vertically centered within the fish body on Round 2.
- [x] Rounded retains its current placement.
- [x] Digital bold retains its current placement.
- [x] Digital fine retains its current placement.
- [x] Classic bold retains its current placement.
- [x] Compact bold retains its current placement.
- [x] The native dual-target build succeeds.
- [x] Native tall-light captures are inspected at each target resolution.
- [x] The 0.8.1 store GIF uses the corrected Time 2 tall-light capture.
- [x] The 0.8.1 store GIF uses the corrected Round 2 tall-light capture.
- [x] RePebble lists 0.8.1 for Time 2.
- [x] RePebble lists 0.8.1 for Round 2.

## RePebble gallery order — 2026-10-23

- [x] The Time 2 public gallery shows the 0.8.0 animated GIF first.
- [x] The Round 2 public gallery shows the 0.8.0 animated GIF first.
- [x] Both public galleries retain the castle, kelp, reef, and grayscale screenshots.

## Six time fonts — 2026-10-23

- [x] Phone settings offer six distinct time font choices.
- [x] Existing installs keep the current time font by default.
- [x] An invalid font value falls back to the current time font.
- [x] Each font renders an unclipped time inside the fish on Time 2.
- [x] Each font renders an unclipped time inside the fish on Round 2.
- [x] The selected font persists after app restart.
- [x] The release PBW contains both targets.
- [x] A fresh verifier inspects the final font captures.
- [x] RePebble lists the font-enabled release for Time 2.
- [x] RePebble lists the font-enabled release for Round 2.
- [x] The Time 2 public gallery leads with the updated animated GIF.
- [x] The Round 2 public gallery leads with the updated animated GIF.

## Calm wrist motion revision — 2026-10-22

- [x] A three-second glance model records the current ambient-motion exposure.
- [x] A three-second glance model records the proposed ambient-motion exposure.
- [x] Gentle ambient motion overlaps fewer modeled glances than version 0.7.0.
- [x] Lively ambient motion overlaps fewer modeled glances than version 0.7.0.
- [x] Gentle clock-fish turns occur no more than once per nine modeled minutes.
- [x] Lively clock-fish turns occur no more than once per five modeled minutes.
- [x] A clock-fish glide starts in the fish's current facing direction.
- [x] The fish reverses direction at the outer edge of a glide.
- [x] The time box stays at the same body-centered offset during the turn.
- [x] The battery percentage remains stationary during the glide.
- [x] Gentle small-fish movement stays within ten pixels per burst.
- [x] Lively crossing fish enter from outside the screen.
- [x] Lively crossing fish exit beyond the opposite screen edge.
- [x] Still mode schedules no ambient animation.
- [x] The revised face builds for Emery.
- [x] The revised face builds for Gabbro.
- [x] Native motion frames are inspected on Emery.
- [x] Native motion frames are inspected on Gabbro.
- [x] A fresh verifier reviews the motion revision.
- [x] RePebble lists the revised PBW version.

Use `watchface/src/c/motion.h` for timing and movement, `tests/test_motion.c` for bounds, and `directives/build_watchface.md` for native checks. The wrist-glance model is an estimate, not a physical battery test. Preserve the pre-existing `.tmp/.gitkeep` deletion. Version 0.7.1 and refreshed native galleries are public for Time 2 and Round 2; physical-wrist motion remains unverified.

## RePebble 0.7.0 publication — 2026-10-22

- [x] RePebble accepts the 0.7.0 PBW for Emery.
- [x] RePebble accepts the 0.7.0 PBW for Gabbro.
- [x] Emery has a native animated GIF in the listing.
- [x] Gabbro has a native animated GIF in the listing.
- [x] Emery has four native scene screenshots in the listing.
- [x] Gabbro has four native scene screenshots in the listing.
- [x] The listing description names the date choices.
- [x] The public listing shows version 0.7.0.
- [x] The public listing opens from the appstore.

The source bundle is `release/Clockfish-0.7.0.pbw`. Native inputs are in `media/native/`; generated store GIFs are in `media/store/`. Leave the pre-existing `.tmp/.gitkeep` deletion untouched.

## Living aquarium motion — 2026-10-22

- [x] Motion settings offer Still, Gentle, and Lively.
- [x] An existing saved Motion Off choice remains Still after upgrade.
- [x] An existing saved Motion On choice becomes Gentle after upgrade.
- [x] Still schedules no ambient animation.
- [x] Gentle animates bubbles during brief ambient bursts.
- [x] Lively animates bubbles more often than Gentle.
- [x] Small fish move during ambient bursts.
- [x] The jellyfish pulses during ambient bursts.
- [x] The crab occasionally scuttles during ambient bursts.
- [x] The clock fish stays inside the screen during automatic glides on Emery.
- [x] The clock fish stays inside the screen during automatic glides on Gabbro.
- [x] The clock digits move with the clock fish.
- [x] The clock fish can face left during a glide.
- [x] The clock fish can face right during a glide.
- [x] Clock digits stay upright in both fish directions.
- [x] The time uses rounded numerals within the clock fish on Emery.
- [x] The time uses rounded numerals within the clock fish on Gabbro.
- [x] The first and last time digits clear the fish outline on Emery.
- [x] The first and last time digits clear the fish outline on Gabbro.
- [x] Battery percentage stays upright in both fish directions.
- [x] A tap starts a clock-fish glide when motion is enabled.
- [x] Still prevents a tap-triggered glide.
- [x] The date remains readable during motion.
- [x] The blue fish body differs from dark kelp water at native size.
- [x] The blue fish clock digits remain readable at native size.
- [x] Mixed fish/background choices retain a visible fish silhouette.
- [x] A dual-target PBW contains the motion setting.
- [x] Native motion frames are captured on Emery.
- [x] Native motion frames are captured on Gabbro.
- [x] A fresh verifier reviews the motion behavior.

Keep animation bursts short to limit redraws and preserve time legibility. Package a local preview; physical battery and RePebble state require separate checks.

## Independent fish and aquarium choices — 2026-10-22

- [x] Existing scene presets remain the default after an upgrade.
- [x] Fish color offers Match aquarium, Orange, Blue, Turquoise, Grayscale, Violet, Sea green, and Random daily.
- [x] Background offers Match scene, Castle, Kelp, Reef, Grayscale, and Random daily.
- [x] Random fish color is stable within one local calendar day.
- [x] Random background is stable within one local calendar day.
- [x] Fish color can differ from background theme on Emery.
- [x] Fish color can differ from background theme on Gabbro.
- [x] Every background shows at least one small sea creature outside the clock fish.
- [x] Every background shows visible bubbles outside the clock fish.
- [x] Time digits remain readable in mixed color combinations on Emery.
- [x] Time digits remain readable in mixed color combinations on Gabbro.
- [x] Date choices remain Off, YYYY-MM-DD, MM/DD/YYYY, and DD/MM/YYYY.
- [x] A dual-target PBW contains the new settings.
- [x] Fresh native screenshots show a mixed combination on each target.
- [x] A fresh verifier reviews the settings and native screenshots.

Deliver a local preview; physical-device behavior and RePebble publication require separate verification.

## Pixel illustration sprite pass — 2026-10-22

- [x] The original aquarium concept images are saved in the project as references.
- [x] A transparent fish sprite is generated from the motion storyboard style.
- [x] The sprite body has an empty region for live time digits.
- [x] An emery sprite renders without clipping at 200x228.
- [x] A gabbro sprite renders without clipping at 260x260.
- [x] The time is fully inside the sprite body on emery.
- [x] The time is fully inside the sprite body on gabbro.
- [x] A grayscale sprite preserves the grayscale scene.
- [x] Scene changes load the corresponding fish colorway.
- [x] Four illustrated background scenes render at 200x228.
- [x] Four illustrated background scenes render at 260x260.
- [x] A night background renders at 200x228.
- [x] A night background renders at 260x260.
- [x] The PBW includes emery and gabbro builds.
- [x] Both targets have fresh native captures for four scenes.
- [x] A fresh verifier compares the final fish to the concept reference.

Use the original concept storyboard as the visual reference. The fish sprite provides richer pixel art while the digits remain live text. This is a local preview; physical motion and store publication are separate.

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
