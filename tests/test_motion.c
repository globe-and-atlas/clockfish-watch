#include <assert.h>

#include "../watchface/src/c/motion.h"

int main(void) {
  assert(motion_ambient_interval_ms(0) == 0);
  assert(motion_ambient_interval_ms(1) > motion_ambient_interval_ms(2));
  assert(motion_ambient_interval_ms(1) == 30000);
  assert(motion_ambient_interval_ms(2) == 15000);
  assert(motion_swim_every_cycles(1) == 18);
  assert(motion_swim_every_cycles(2) == 18);
  assert(motion_frame_ms(1) > motion_frame_ms(2));
  assert(motion_bubble_lift(0, -1, 0) == 0);
  for (int level = 1; level <= 2; level++) {
    assert(motion_bubble_lift(level, 0, 1) == motion_bubble_lift(level, -1, 0));
    assert(motion_bubble_lift(level, 5, 1) == motion_bubble_lift(level, -1, 1));
  }
  assert(motion_school_shift(1, 3) <= 10);
  assert(motion_school_shift(2, 3) > motion_school_shift(1, 3));
  assert(motion_school_shift(1, 5) == 0);
  assert(!motion_crossing_active(1, 2, 3));
  assert(!motion_crossing_active(2, 2, 2));
  assert(motion_crossing_active(2, 2, 3));
  assert(motion_crossing_x(200, 0, false) < 0);
  assert(motion_crossing_x(200, 5, false) > 200);
  assert(motion_crossing_x(200, 0, true) > 200);
  assert(motion_crossing_x(200, 5, true) < 0);
  assert(motion_jelly_radius(2) > motion_jelly_radius(-1));
  assert(motion_crab_shift(1, 3, 4) > 0);
  assert(motion_crab_shift(1, 3, 5) == 0);
  assert(motion_crab_shift(2, 3, 2) > 0);
  assert(CLOCKFISH_TURN_FRAME == 5);
  for (int width = 200; width <= 260; width += 60) {
    for (int step = 0; step < CLOCKFISH_SWIM_STEPS; step++) {
      int right = motion_fish_shift(width, step, false);
      int left = motion_fish_shift(width, step, true);
      assert(right == -left);
      assert(right >= 0 && right <= 8);
      assert(motion_fish_bob(step) >= -3 && motion_fish_bob(step) <= 0);
      if (step > 0 && step <= CLOCKFISH_TURN_FRAME) {
        assert(right >= motion_fish_shift(width, step - 1, false));
      } else if (step > CLOCKFISH_TURN_FRAME) {
        assert(right <= motion_fish_shift(width, step - 1, false));
      }
    }
  }
  assert(motion_fish_shift(200, 0, false) == 0);
  assert(motion_fish_shift(200, 10, false) == 0);
  return 0;
}
