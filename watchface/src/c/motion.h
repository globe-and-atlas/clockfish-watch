#ifndef CLOCKFISH_MOTION_H
#define CLOCKFISH_MOTION_H

#include <stdbool.h>

#define CLOCKFISH_SWIM_STEPS 11
#define CLOCKFISH_TURN_FRAME 5

// Brief animation bursts keep the watch readable between moments of movement.
static inline int motion_ambient_interval_ms(int level) {
  return level == 2 ? 15000 : level == 1 ? 30000 : 0;
}

static inline int motion_swim_every_cycles(int level) {
  return level == 2 || level == 1 ? 18 : 0;
}

static inline int motion_frame_ms(int level) {
  return level == 2 ? 320 : 460;
}

static inline int motion_bubble_lift(int level, int frame, int cycle) {
  if (level == 0) return 0;
  int step = level == 2 ? 4 : 2;
  int completed = cycle - (frame >= 0 ? 1 : 0);
  if (completed < 0) completed = 0;
  // Two bubble spacings preserve alternating bubble size and x-position on wrap.
  return (completed * step * 5 + (frame >= 0 ? frame * step : 0)) % 54;
}

static inline int motion_school_shift(int level, int frame) {
  static const int drift[] = {0, 1, 2, 3, 2, 0};
  return level == 0 || frame < 0 || frame >= 6 ? 0 :
         drift[frame] * (level == 2 ? 2 : 1);
}

static inline bool motion_crossing_active(int level, int frame, int cycle) {
  return level == 2 && frame >= 0 && frame < 6 && cycle % 3 == 0;
}

static inline int motion_crossing_x(int width, int frame, bool moving_left) {
  int x = -8 + frame * (width + 16) / 5;
  return moving_left ? width - x : x;
}

static inline int motion_jelly_radius(int frame) {
  static const int pulse[] = {0, 0, 1, 1, 0, 0};
  return frame < 0 || frame >= 6 ? 7 : 7 + pulse[frame];
}

static inline int motion_crab_shift(int level, int frame, int cycle) {
  static const int scuttle[] = {0, 2, 4, 5, 2, 0};
  int cadence = level == 2 ? 2 : 4;
  return frame < 0 || frame >= 6 || cycle % cadence != 0 ? 0 : scuttle[frame];
}

static inline int motion_fish_shift(int width, int step, bool started_left) {
  static const int excursion[CLOCKFISH_SWIM_STEPS] = {0, 2, 5, 8, 10, 12, 10, 8, 5, 2, 0};
  if (step < 0 || step >= CLOCKFISH_SWIM_STEPS) return 0;
  int value = excursion[step] * (started_left ? -1 : 1);
  return width >= 240 ? value * 2 / 3 : value / 2;
}

static inline int motion_fish_bob(int step) {
  static const int bob[CLOCKFISH_SWIM_STEPS] = {0, -1, -2, -2, -3, -3, -3, -2, -2, -1, 0};
  return step < 0 || step >= CLOCKFISH_SWIM_STEPS ? 0 : bob[step];
}

#endif
