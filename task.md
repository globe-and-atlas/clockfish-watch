# Clockfish watchface

## Contract
- Target: Pebble Time 2 (emery) and Pebble Round 2 (gabbro).
- Output: `release/Clockfish-0.1.0.pbw`.
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
