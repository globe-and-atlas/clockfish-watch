#include <assert.h>
#include <stdint.h>

#include "../watchface/src/c/selection.h"

int main(void) {
  for (int scene = 0; scene < 4; scene++) {
    assert(selection_resolve(0, scene, 6, 100, 1) == scene);
  }
  for (int choice = 1; choice <= 6; choice++) {
    assert(selection_resolve(choice, 0, 6, 100, 1) == choice - 1);
  }
  int fish_changes = 0;
  int background_changes = 0;
  int independent = 0;
  for (uint32_t day = 100; day < 114; day++) {
    int fish = selection_resolve(7, 0, 6, day, 0x51f15e0a);
    int background = selection_resolve(5, 0, 4, day, 0x9b8d28ca);
    assert(fish >= 0 && fish < 6);
    assert(background >= 0 && background < 4);
    assert(fish == selection_resolve(7, 3, 6, day, 0x51f15e0a));
    assert(background == selection_resolve(5, 3, 4, day, 0x9b8d28ca));
    if (day > 100) {
      fish_changes += fish != selection_resolve(7, 0, 6, day - 1, 0x51f15e0a);
      background_changes += background != selection_resolve(5, 0, 4, day - 1, 0x9b8d28ca);
    }
    independent += fish != background;
  }
  assert(fish_changes > 0);
  assert(background_changes > 0);
  assert(independent > 0);
  return 0;
}
