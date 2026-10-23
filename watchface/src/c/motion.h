#ifndef CLOCKFISH_MOTION_H
#define CLOCKFISH_MOTION_H

// Brief animation bursts keep the watch readable between moments of movement.
static inline int motion_ambient_interval_ms(int level) {
  return level == 2 ? 10000 : level == 1 ? 20000 : 0;
}

static inline int motion_swim_every_cycles(int level) {
  return level == 2 ? 6 : level == 1 ? 8 : 0;
}

static inline int motion_frame_ms(int level) {
  return level == 2 ? 260 : 420;
}

static inline int motion_bubble_lift(int level, int frame) {
  return frame < 0 ? 0 : frame * (level == 2 ? 7 : 4);
}

static inline int motion_school_shift(int level, int frame) {
  return frame < 0 ? 0 : frame * (level == 2 ? 8 : 5) - 10;
}

static inline int motion_jelly_radius(int frame) {
  static const int pulse[] = {0, 1, 2, 1, 0, 0};
  return frame < 0 || frame >= 6 ? 7 : 7 + pulse[frame];
}

static inline int motion_crab_shift(int level, int frame, int cycle) {
  static const int scuttle[] = {0, 2, 4, 5, 2, 0};
  int cadence = level == 2 ? 2 : 4;
  return frame < 0 || frame >= 6 || cycle % cadence != 0 ? 0 : scuttle[frame];
}

#endif
