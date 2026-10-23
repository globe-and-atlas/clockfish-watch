# Release

Build from `watchface/`, inspect fresh emery and gabbro native screenshots, then upload the PBW and appropriately sized media to RePebble when publication is requested. A successful compile alone is not release evidence.

Clockfish 0.8.0 is listed at https://apps.repebble.com/40292564f42e4f4e8763b9a1. The first gallery image for each platform is the native-size GIF made with `python3 execution/create_store_media.py --version 0.8.0`; the remaining screenshots show kelp, reef, grayscale, and castle. The public page reports both Time 2 and Round 2 with version 0.8.0. Keep physical-watch motion and battery verification separate from the appstore listing.

The 0.8.0 gallery uses a twelve-frame GIF to show motion, all six time fonts, and scene changes. RePebble’s Round 2 public gallery shows the updated GIF first. The Time 2 editor has the new GIF first, but a fresh public check still served the previous GIF asset immediately after saving; confirm its public asset ID before treating propagation as complete.
