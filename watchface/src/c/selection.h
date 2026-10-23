#ifndef CLOCKFISH_SELECTION_H
#define CLOCKFISH_SELECTION_H

#include <stdint.h>

// Choice 0 follows the scene, 1-count fixes a variant, then random daily.
static inline int selection_resolve(int choice, int scene, int count,
                                    uint32_t day_key, uint32_t salt) {
  if (choice >= 1 && choice <= count) return choice - 1;
  if (choice != count + 1) return scene;
  uint32_t hash = day_key ^ salt;
  hash ^= hash >> 16;
  hash *= 0x7feb352d;
  hash ^= hash >> 15;
  hash *= 0x846ca68b;
  hash ^= hash >> 16;
  return hash % count;
}

#endif
