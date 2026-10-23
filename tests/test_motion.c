#include <assert.h>

#include "../watchface/src/c/motion.h"

int main(void) {
  assert(motion_ambient_interval_ms(0) == 0);
  assert(motion_ambient_interval_ms(1) > motion_ambient_interval_ms(2));
  assert(motion_swim_every_cycles(1) > motion_swim_every_cycles(2));
  assert(motion_frame_ms(1) > motion_frame_ms(2));
  assert(motion_bubble_lift(1, -1) == 0);
  for (int frame = 0; frame < 5; frame++) {
    assert(motion_bubble_lift(1, frame + 1) > motion_bubble_lift(1, frame));
    assert(motion_bubble_lift(2, frame + 1) > motion_bubble_lift(1, frame + 1));
  }
  assert(motion_school_shift(2, 5) > motion_school_shift(1, 5));
  assert(motion_jelly_radius(2) > motion_jelly_radius(-1));
  assert(motion_crab_shift(1, 3, 4) > 0);
  assert(motion_crab_shift(1, 3, 5) == 0);
  assert(motion_crab_shift(2, 3, 2) > 0);
  return 0;
}
