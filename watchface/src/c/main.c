#include <pebble.h>
#include "selection.h"
#include "motion.h"

enum { PERSIST_SCENE = 1, PERSIST_DATE, PERSIST_MOTION, PERSIST_MODE, PERSIST_BATTERY,
       PERSIST_FISH_COLOR, PERSIST_BACKGROUND, PERSIST_ANIMATION };

typedef struct {
  GColor water, deep, sand, plant, detail, fish, fin, numeral, bubble, battery;
} Palette;

static Window *s_window;
static Layer *s_face;
static GBitmap *s_fish_bitmap;
static GBitmap *s_background_bitmap;
static int s_fish_variant = -1;
static bool s_facing_left;
static bool s_loaded_left;
static int s_background_variant = -1;
static bool s_background_night;
static AppTimer *s_swim_timer;
static AppTimer *s_ambient_timer;
static int s_scene;
static int s_render_scene;
static int s_fish_color;
static int s_background;
static int s_date;
static int s_mode = 2;
static int s_motion = 1;
static bool s_battery_visible = true;
static int s_battery_percent = 100;
static int s_swim_step = -1;
static int s_ambient_frame = -1;
static int s_ambient_cycles;

static GColor color(uint32_t rgb) { return GColorFromHEX(rgb); }

static Palette palette(int scene, bool night) {
  Palette p;
  if (scene == 1) {
    p = (Palette){color(0x0A5262), color(0x073746), color(0x174E4B),
      color(0x2F996E), color(0x97CFC1), color(0x175A81), color(0x6DD6BD),
      color(0xFFFFFF), color(0xD5F6E5), color(0xE7B64E)};
  } else if (scene == 2) {
    p = (Palette){color(0x091536), color(0x050B21), color(0x242457),
      color(0x6544A2), color(0xEA5090), color(0x096A85), color(0x2BCDE5),
      color(0xFFFFFF), color(0x42D3EB), color(0xE5A137)};
  } else if (scene == 3) {
    p = (Palette){color(0xAAAAAA), color(0x555555), color(0xAAAAAA),
      color(0x555555), color(0x222222), color(0x222222), color(0x555555),
      color(0xFFFFFF), color(0xFFFFFF), color(0x555555)};
  } else {
    p = (Palette){color(0x39AEDD), color(0x267FAF), color(0xE9D19A),
      color(0x399951), color(0xDCE4D5), color(0xA34926), color(0xF1AC55),
      color(0xFFFFFF), color(0xFFFFFF), color(0xE7B64E)};
  }
  if (!night && scene == 2) {
    p.water = color(0x24568E);
    p.deep = color(0x143B70);
    p.sand = color(0x634C8C);
    p.plant = color(0x976BC0);
    p.detail = color(0xF385A9);
  } else if (night && scene == 3) {
    p.water = color(0x444444);
    p.deep = color(0x111111);
    p.sand = color(0x666666);
    p.plant = color(0xAAAAAA);
    p.detail = color(0xDDDDDD);
    p.fish = color(0x555555);
    p.fin = color(0x999999);
    p.bubble = GColorWhite;
  } else if (night && scene != 2) {
    p.water = color(0x092E49);
    p.deep = color(0x081E36);
    p.plant = color(0x256B61);
    p.sand = color(0x767B73);
    p.detail = color(0x9BC1C7);
  }
  return p;
}

static void fill_triangle(GContext *ctx, GPoint a, GPoint b, GPoint c) {
  GPoint points[] = {a, b, c};
  GPathInfo info = {.num_points = 3, .points = points};
  GPath *path = gpath_create(&info);
  if (path) { gpath_draw_filled(ctx, path); gpath_destroy(path); }
}

static void fill_polygon(GContext *ctx, GPoint *points, uint32_t count,
                         GColor fill, GColor edge) {
  GPathInfo info = {.num_points = count, .points = points};
  GPath *path = gpath_create(&info);
  if (!path) return;
  graphics_context_set_fill_color(ctx, fill);
  gpath_draw_filled(ctx, path);
  graphics_context_set_stroke_color(ctx, edge);
  graphics_context_set_stroke_width(ctx, 1);
  gpath_draw_outline(ctx, path);
  gpath_destroy(path);
}

static void line(GContext *ctx, GColor ink, int x1, int y1, int x2, int y2, int width) {
  graphics_context_set_stroke_color(ctx, ink);
  graphics_context_set_stroke_width(ctx, width);
  graphics_draw_line(ctx, GPoint(x1, y1), GPoint(x2, y2));
}

static void draw_plants(GContext *ctx, int w, int h, Palette p) {
  int bottom = h - (w >= 240 ? 50 : 25);
  int left = w / 12;
  int right = w - left;
  for (int side = 0; side < 2; side++) {
    int x = side ? right : left;
    for (int stem = 0; stem < (s_render_scene == 1 ? 4 : 3); stem++) {
      int root = x + (stem - 1) * (s_render_scene == 1 ? 12 : 10);
      int top = bottom - (s_render_scene == 1 ? 92 : 46) - stem * (s_render_scene == 1 ? 9 : 6);
      int tip = root + (side ? -8 : 8) + (stem - 1) * 3;
      line(ctx, p.deep, root + 1, bottom, tip + 1, top, s_render_scene == 1 ? 5 : 3);
      line(ctx, p.plant, root, bottom, tip, top, s_render_scene == 1 ? 3 : 2);
      for (int leaf = 0; leaf < (s_render_scene == 1 ? 4 : 2); leaf++) {
        int yy = bottom - 16 - leaf * (s_render_scene == 1 ? 19 : 18);
        if (yy <= top + 8) continue;
        int stem_x = root + (tip - root) * (bottom - yy) / (bottom - top);
        int spread = (leaf % 2 ? -1 : 1) * (side ? -1 : 1);
        int length = s_render_scene == 1 ? 23 : 13;
        int rise = s_render_scene == 1 ? 17 : 10;
        GPoint blade[] = {
          GPoint(stem_x, yy + 5),
          GPoint(stem_x + spread * (length / 2), yy - 7),
          GPoint(stem_x + spread * length, yy - rise),
          GPoint(stem_x + spread * (length - 6), yy - 4),
          GPoint(stem_x + spread * 4, yy + 7)
        };
        GColor blade_color = (s_render_scene == 1 && leaf % 2) ? p.detail : p.plant;
        fill_polygon(ctx, blade, 5, blade_color, p.deep);
        if (s_render_scene == 1) {
          line(ctx, p.deep, stem_x, yy + 5,
               stem_x + spread * (length - 5), yy - rise + 2, 1);
        }
      }
    }
  }
}

static void draw_castle(GContext *ctx, int w, int h, Palette p) {
  int x = w >= 240 ? w / 2 + 30 : w - 55;
  int y = h - (w >= 240 ? 100 : 61);
  graphics_context_set_fill_color(ctx, color(0x9CB2BA));
  graphics_fill_rect(ctx, GRect(x, y + 14, 44, 36), 1, GCornerNone);
  graphics_fill_rect(ctx, GRect(x - 5, y + 5, 14, 45), 1, GCornerNone);
  graphics_fill_rect(ctx, GRect(x + 35, y, 14, 50), 1, GCornerNone);
  for (int i = 0; i < 3; i++) {
    graphics_fill_rect(ctx, GRect(x - 5 + 5 * i, y, 4, 8), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(x + 35 + 5 * i, y - 5, 4, 8), 0, GCornerNone);
  }
  graphics_context_set_fill_color(ctx, p.deep);
  graphics_fill_rect(ctx, GRect(x + 16, y + 32, 12, 18), 5, GCornersTop);
  graphics_fill_rect(ctx, GRect(x, y + 18, 5, 8), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(x + 39, y + 15, 5, 8), 0, GCornerNone);
  line(ctx, p.deep, x + 6, y + 26, x + 38, y + 26, 1);
  line(ctx, p.deep, x + 5, y + 36, x + 15, y + 36, 1);
  line(ctx, p.deep, x + 29, y + 36, x + 39, y + 36, 1);
  line(ctx, p.deep, x + 12, y + 18, x + 12, y + 25, 1);
  line(ctx, p.deep, x + 30, y + 27, x + 30, y + 35, 1);
  line(ctx, p.detail, x - 5, y + 4, x + 9, y + 4, 2);
  line(ctx, p.detail, x + 35, y - 1, x + 49, y - 1, 2);
  line(ctx, p.detail, x + 43, y - 6, x + 43, y - 17, 1);
  graphics_context_set_fill_color(ctx, p.fin);
  fill_triangle(ctx, GPoint(x + 43, y - 17), GPoint(x + 54, y - 14),
                GPoint(x + 43, y - 11));
}

static void draw_scene_details(GContext *ctx, int w, int h, Palette p) {
  int floor = h - (w >= 240 ? 50 : 27);
  if (s_render_scene == 0) {
    draw_castle(ctx, w, h, p);
    graphics_context_set_fill_color(ctx, p.detail);
    for (int i = 0; i < 8; i++) {
      int x = 12 + ((i * 47) % (w - 24));
      graphics_fill_circle(ctx, GPoint(x, floor + 8 + (i % 2) * 7), 1 + i % 2);
    }
    line(ctx, p.bubble, 14, 44, 52, 44, 1);
    line(ctx, p.bubble, w - 52, 45, w - 18, 45, 1);
    graphics_context_set_fill_color(ctx, p.fin);
    fill_triangle(ctx, GPoint(w / 4, floor + 6), GPoint(w / 4 - 6, floor + 14),
                  GPoint(w / 4 + 5, floor + 12));
    fill_triangle(ctx, GPoint(w / 4, floor + 6), GPoint(w / 4 + 8, floor + 6),
                  GPoint(w / 4 + 5, floor + 12));
    int crab_x = w / 3, crab_y = floor + 13;
    graphics_context_set_fill_color(ctx, p.fin);
    graphics_fill_circle(ctx, GPoint(crab_x, crab_y), 5);
    line(ctx, p.fin, crab_x - 3, crab_y + 3, crab_x - 8, crab_y + 7, 1);
    line(ctx, p.fin, crab_x + 3, crab_y + 3, crab_x + 8, crab_y + 7, 1);
    line(ctx, p.fin, crab_x - 3, crab_y - 3, crab_x - 9, crab_y - 9, 2);
    line(ctx, p.fin, crab_x + 3, crab_y - 3, crab_x + 9, crab_y - 9, 2);
    graphics_context_set_fill_color(ctx, p.deep);
    graphics_fill_circle(ctx, GPoint(crab_x - 2, crab_y - 1), 1);
    graphics_fill_circle(ctx, GPoint(crab_x + 2, crab_y - 1), 1);
  } else if (s_render_scene == 1) {
    int jelly_x = w * 3 / 4, jelly_y = h / 5;
    graphics_context_set_fill_color(ctx, p.detail);
    graphics_fill_circle(ctx, GPoint(jelly_x, jelly_y), 9);
    graphics_context_set_fill_color(ctx, p.water);
    graphics_fill_rect(ctx, GRect(jelly_x - 9, jelly_y, 18, 9), 0, GCornerNone);
    for (int i = -1; i <= 1; i++)
      line(ctx, p.detail, jelly_x + i * 6, jelly_y + 1,
           jelly_x + i * 8, jelly_y + 19, 1);
    line(ctx, p.bubble, jelly_x - 6, jelly_y, jelly_x + 6, jelly_y, 1);
    for (int i = 0; i < 5; i++) {
      int x = 8 + i * (w - 16) / 4;
      line(ctx, p.plant, x, floor, x + (i % 2 ? 6 : -6), floor - 30 - i * 3, 2);
    }
    graphics_context_set_fill_color(ctx, p.detail);
    graphics_fill_circle(ctx, GPoint(w / 2 - 45, floor + 7), 6);
    graphics_fill_circle(ctx, GPoint(w / 2 + 42, floor + 11), 4);
  } else if (s_render_scene == 2) {
    GPoint left_rock[] = {
      GPoint(0, floor + 5), GPoint(0, floor - 8),
      GPoint(12, floor - 18), GPoint(31, floor - 14),
      GPoint(47, floor - 3), GPoint(57, floor + 5)
    };
    GPoint right_rock[] = {
      GPoint(w - 58, floor + 5), GPoint(w - 43, floor - 9),
      GPoint(w - 24, floor - 17), GPoint(w - 7, floor - 12),
      GPoint(w, floor - 5), GPoint(w, floor + 5)
    };
    fill_polygon(ctx, left_rock, 6, p.plant, p.deep);
    fill_polygon(ctx, right_rock, 6, p.plant, p.deep);
    for (int side = 0; side < 2; side++) {
      int x = side ? w - 27 : 27;
      line(ctx, p.detail, x, floor, x, floor - 34, 5);
      line(ctx, p.detail, x, floor - 13, x + (side ? -13 : 13), floor - 34, 4);
      line(ctx, p.detail, x, floor - 21, x + (side ? 12 : -12), floor - 46, 4);
      line(ctx, p.detail, x + (side ? -13 : 13), floor - 34,
           x + (side ? -18 : 18), floor - 43, 3);
      line(ctx, p.plant, x + (side ? -15 : 15), floor,
           x + (side ? -20 : 20), floor - 22, 5);
      for (int branch = 0; branch < 3; branch++) {
        int bx = x + (side ? -1 : 1) * (branch * 5);
        int by = floor - 10 - branch * 10;
        graphics_context_set_fill_color(ctx, p.detail);
        graphics_fill_circle(ctx, GPoint(bx, by), 4);
      }
      graphics_context_set_fill_color(ctx, p.bubble);
      graphics_fill_circle(ctx, GPoint(x + (side ? 11 : -11), floor - 41), 2);
    }
    graphics_context_set_fill_color(ctx, p.bubble);
    for (int i = 0; i < 6; i++)
      graphics_fill_circle(ctx, GPoint(15 + ((i * 37) % (w - 30)), 20 + (i * 17) % 45), 1);
    int anemone_x = w / 2 + 43;
    for (int i = -2; i <= 2; i++) {
      line(ctx, p.plant, anemone_x + i * 3, floor + 4,
           anemone_x + i * 6, floor - 18 - (i % 2) * 5, 3);
      graphics_context_set_fill_color(ctx, p.detail);
      graphics_fill_circle(ctx, GPoint(anemone_x + i * 6,
                                      floor - 18 - (i % 2) * 5), 3);
    }
  } else {
    GPoint left_stones[] = {
      GPoint(0, floor + 9), GPoint(0, floor - 6),
      GPoint(19, floor - 17), GPoint(37, floor - 12),
      GPoint(52, floor + 3)
    };
    GPoint right_stones[] = {
      GPoint(w - 56, floor + 8), GPoint(w - 41, floor - 7),
      GPoint(w - 19, floor - 15), GPoint(w, floor - 5),
      GPoint(w, floor + 9)
    };
    fill_polygon(ctx, left_stones, 5, p.plant, p.detail);
    fill_polygon(ctx, right_stones, 5, p.plant, p.detail);
    line(ctx, p.bubble, 16, floor - 9, 27, floor - 12, 1);
    line(ctx, p.bubble, w - 36, floor - 9, w - 22, floor - 12, 1);
    line(ctx, p.detail, w / 2 + 35, 62, w / 2 + 67, 62, 1);
    for (int i = 0; i < 5; i++) {
      int x = w / 2 + 23 + i * 10;
      graphics_context_set_fill_color(ctx, i % 2 ? p.plant : p.detail);
      graphics_fill_circle(ctx, GPoint(x, floor + 6 + i % 2 * 5), 3 + i % 3);
    }
  }
}

static void draw_bubbles(GContext *ctx, int w, int h, Palette p) {
  graphics_context_set_stroke_color(ctx, p.bubble);
  graphics_context_set_stroke_width(ctx, 1);
  int shift = s_swim_step < 0 ? 0 : s_swim_step % 4;
  for (int i = 0; i < 6; i++) {
    int x = w / 2 + w / 3 + (i % 2) * 6;
    int y = h - 46 - i * 26 - shift * 2;
    graphics_draw_circle(ctx, GPoint(x, y), 2 + i % 2);
  }
}

static void draw_small_fish(GContext *ctx, int x, int y, int size,
                            GColor body, GColor eye, bool left) {
  graphics_context_set_fill_color(ctx, body);
  graphics_fill_circle(ctx, GPoint(x, y), size);
  int sign = left ? 1 : -1;
  fill_triangle(ctx, GPoint(x + sign * (size - 1), y),
                GPoint(x + sign * size * 2, y - size),
                GPoint(x + sign * size * 2, y + size));
  graphics_context_set_fill_color(ctx, eye);
  graphics_fill_circle(ctx, GPoint(x - sign * size / 2, y - 1), 1);
}

static void draw_scene_life(GContext *ctx, int w, int h, Palette p) {
  int drift = (s_swim_step < 0 ? 0 : (s_swim_step - 5) / 3) +
              motion_school_shift(s_motion, s_ambient_frame);
  GColor accent = s_render_scene == 3 ? GColorBlack : p.fin;
  GColor outline = s_render_scene == 3 ? GColorWhite : p.deep;
  int school_y = w >= 240 ? 64 : 47;
  draw_small_fish(ctx, 41 + drift, school_y, 4, accent, outline, false);
  draw_small_fish(ctx, 29 + drift, school_y + 12, 3, p.detail, outline, true);
  if (s_ambient_frame >= 0) {
    bool moving_left = s_ambient_cycles % 2 == 0;
    int crossing_x = 18 + s_ambient_frame * (w - 36) / 5;
    if (moving_left) crossing_x = w - crossing_x;
    draw_small_fish(ctx, crossing_x, h / 2 + 37, 3, accent, outline, moving_left);
  }

  int jelly_x = w - 42, jelly_y = w >= 240 ? 66 : 45;
  GColor jelly = s_render_scene == 3 ? GColorWhite : color(0xF2A8C9);
  graphics_context_set_fill_color(ctx, jelly);
  int jelly_radius = motion_jelly_radius(s_ambient_frame);
  graphics_fill_circle(ctx, GPoint(jelly_x, jelly_y), jelly_radius);
  graphics_context_set_fill_color(ctx, outline);
  graphics_fill_circle(ctx, GPoint(jelly_x - 2, jelly_y), 1);
  graphics_fill_circle(ctx, GPoint(jelly_x + 2, jelly_y), 1);
  line(ctx, jelly, jelly_x - 5, jelly_y + jelly_radius - 2,
       jelly_x - 5, jelly_y + jelly_radius + 7, 1);
  line(ctx, jelly, jelly_x, jelly_y + jelly_radius,
       jelly_x, jelly_y + jelly_radius + 9, 1);
  line(ctx, jelly, jelly_x + 5, jelly_y + jelly_radius - 2,
       jelly_x + 5, jelly_y + jelly_radius + 7, 1);

  // A small crab stays beside the seabed, clear of the battery fish.
  int crab_x = w - (w >= 240 ? 50 : 32) -
               motion_crab_shift(s_motion, s_ambient_frame, s_ambient_cycles);
  int crab_y = h - (w >= 240 ? 50 : 36);
  GColor crab_color = s_render_scene == 3 ? GColorWhite :
                      s_render_scene == 0 ? color(0xD44E6A) : color(0xFFAA33);
  GColor crab_edge = s_render_scene == 3 ? GColorBlack : color(0x102C44);
  graphics_context_set_fill_color(ctx, crab_edge);
  graphics_fill_circle(ctx, GPoint(crab_x, crab_y), 8);
  graphics_context_set_fill_color(ctx, crab_color);
  graphics_fill_circle(ctx, GPoint(crab_x, crab_y), 6);
  line(ctx, crab_color, crab_x - 5, crab_y + 4, crab_x - 11, crab_y + 9, 2);
  line(ctx, crab_color, crab_x + 5, crab_y + 4, crab_x + 11, crab_y + 9, 2);
  line(ctx, crab_color, crab_x - 5, crab_y - 2, crab_x - 11, crab_y - 8, 2);
  line(ctx, crab_color, crab_x + 5, crab_y - 2, crab_x + 11, crab_y - 8, 2);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(crab_x - 2, crab_y - 2), 1);
  graphics_fill_circle(ctx, GPoint(crab_x + 2, crab_y - 2), 1);

  // Bubbles sit at the screen edge and rise slightly during a swim.
  graphics_context_set_stroke_color(ctx, s_render_scene == 3 ? GColorBlack : GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  for (int i = 0; i < 5; i++) {
    int x = w - 15 - (i % 2) * 7;
    int y = h - 49 - i * 27 - motion_bubble_lift(s_motion, s_ambient_frame) -
            (s_swim_step < 0 ? 0 : s_swim_step);
    graphics_draw_circle(ctx, GPoint(x, y), 2 + i % 2);
  }
}

static void draw_light_rays(GContext *ctx, int w, int h, bool night) {
  if (night || s_render_scene != 0) return;
  GColor ray = color(0x57B9DC);
  GPoint left[] = {
    GPoint(14, 0), GPoint(29, 0), GPoint(w / 3, h / 2 + 20),
    GPoint(w / 3 - 13, h / 2 + 20)
  };
  GPoint right[] = {
    GPoint(w - 35, 0), GPoint(w - 20, 0),
    GPoint(w - 51, h / 2 + 20), GPoint(w - 66, h / 2 + 20)
  };
  fill_polygon(ctx, left, 4, ray, ray);
  fill_polygon(ctx, right, 4, ray, ray);
}

static int fish_shift(int w) {
  static const int sequence[] = {0, 3, 6, 9, 12, 15, 12, 9, 6, 3, 0};
  if (s_swim_step < 0) return 0;
  int value = sequence[s_swim_step] * (s_facing_left ? -1 : 1);
  // Keep the fuller tail on-screen throughout the swim.
  return w >= 240 ? value * 2 / 3 : value / 2;
}

static int fish_bob(void) {
  static const int sequence[] = {0, -2, -4, -6, -8, -4, 0, 4, 8, 4, 0};
  return s_swim_step < 0 ? 0 : sequence[s_swim_step];
}

static void draw_clock_fish_vector(GContext *ctx, int w, int h, Palette p, const struct tm *now) {
  int body_w = w >= 240 ? 178 : 146;
  int body_h = w >= 240 ? 80 : 72;
  // Center the numeral area; the tail extends farther left than the snout extends right.
  int x = (w - body_w) / 2 + 11 + fish_shift(w);
  int y = h / 2 - body_h / 2 - 10 + fish_bob();
  int middle = y + body_h / 2;
  int tail = w >= 240 ? 30 : 25;
  int flap = s_swim_step >= 0 && s_swim_step % 2 ? 3 : 0;
  GColor edge = s_fish_variant == 3 ? p.detail : p.deep;
  GPoint tail_points[] = {
    GPoint(x + 13, middle - 10), GPoint(x + 2, middle - 14),
    GPoint(x - tail + 7, y + 8), GPoint(x - tail - flap, y + 5),
    GPoint(x - tail + 1, y + 16), GPoint(x - tail + 8, middle - 8),
    GPoint(x - tail + 12, middle), GPoint(x - tail + 8, middle + 8),
    GPoint(x - tail + 1, y + body_h - 16),
    GPoint(x - tail - flap, y + body_h - 5),
    GPoint(x - tail + 7, y + body_h - 8),
    GPoint(x + 2, middle + 14), GPoint(x + 13, middle + 10)
  };
  fill_polygon(ctx, tail_points, 13, p.fin, edge);
  line(ctx, edge, x + 6, middle, x - tail + 12, middle, 1);
  line(ctx, edge, x - tail + 13, middle - 4, x - tail + 2, y + 9, 1);
  line(ctx, edge, x - tail + 13, middle + 4,
       x - tail + 2, y + body_h - 9, 1);
  GPoint dorsal[] = {
    GPoint(x + 35, y + 11), GPoint(x + 47, y + 2),
    GPoint(x + 60, y - 6), GPoint(x + 70, y - 10),
    GPoint(x + 82, y - 10), GPoint(x + 93, y - 7),
    GPoint(x + 105, y + 7)
  };
  fill_polygon(ctx, dorsal, 7, p.fin, edge);
  line(ctx, edge, x + 58, y + 5, x + 70, y - 7, 1);
  line(ctx, edge, x + 75, y + 3, x + 84, y - 7, 1);
  GPoint ventral[] = {
    GPoint(x + 45, y + body_h - 10),
    GPoint(x + 56, y + body_h + 2),
    GPoint(x + 66, y + body_h + 9),
    GPoint(x + 78, y + body_h + 10),
    GPoint(x + 94, y + body_h - 8)
  };
  fill_polygon(ctx, ventral, 5, p.fin, edge);
  GPoint body[] = {
    GPoint(x + 3, middle), GPoint(x + 5, middle - 12),
    GPoint(x + 10, y + 17), GPoint(x + 19, y + 10),
    GPoint(x + 30, y + 5), GPoint(x + 46, y + 2),
    GPoint(x + body_w / 2, y + 1),
    GPoint(x + body_w - 44, y + 3),
    GPoint(x + body_w - 29, y + 8),
    GPoint(x + body_w - 16, y + 16),
    GPoint(x + body_w - 6, y + 27),
    GPoint(x + body_w, middle - 5),
    GPoint(x + body_w, middle + 3),
    GPoint(x + body_w - 7, y + body_h - 25),
    GPoint(x + body_w - 19, y + body_h - 13),
    GPoint(x + body_w - 34, y + body_h - 6),
    GPoint(x + body_w / 2, y + body_h - 1),
    GPoint(x + 45, y + body_h - 2),
    GPoint(x + 29, y + body_h - 6),
    GPoint(x + 18, y + body_h - 12),
    GPoint(x + 10, y + body_h - 20),
    GPoint(x + 5, middle + 12)
  };
  fill_polygon(ctx, body, 22, p.fish, edge);
  GPoint highlight[] = {
    GPoint(x + 20, y + 19), GPoint(x + 31, y + 10),
    GPoint(x + 49, y + 6), GPoint(x + body_w / 2, y + 5),
    GPoint(x + body_w - 42, y + 8),
    GPoint(x + body_w - 28, y + 16),
    GPoint(x + body_w - 47, y + 14), GPoint(x + 52, y + 15)
  };
  fill_polygon(ctx, highlight, 8, p.fin, p.fin);
  GPoint pectoral[] = {
    GPoint(x + body_w - 32, middle + 5),
    GPoint(x + body_w - 39, middle + 14),
    GPoint(x + body_w - 53, middle + 22),
    GPoint(x + body_w - 51, middle + 7),
    GPoint(x + body_w - 43, middle + 3)
  };
  fill_polygon(ctx, pectoral, 5, p.fin, edge);
  line(ctx, edge, x + body_w - 30, middle - 11,
       x + body_w - 35, middle - 3, 1);
  line(ctx, edge, x + body_w - 35, middle - 3,
       x + body_w - 32, middle + 10, 1);

  char time_text[8];
  strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%H:%M" : "%I:%M", now);
  if (!clock_is_24h_style() && time_text[0] == '0') {
    for (int i = 0; time_text[i]; i++) time_text[i] = time_text[i + 1];
  }
  graphics_context_set_text_color(ctx, p.numeral);
  graphics_draw_text(ctx, time_text,
                     fonts_get_system_font(w >= 240 ? FONT_KEY_LECO_36_BOLD_NUMBERS :
                                                        FONT_KEY_LECO_32_BOLD_NUMBERS),
                     GRect(x + 10, middle - 23, body_w - 45, 48),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  graphics_context_set_fill_color(ctx, p.numeral);
  graphics_fill_circle(ctx, GPoint(x + body_w - 18, middle - 8), 6);
  graphics_context_set_fill_color(ctx, edge);
  graphics_fill_circle(ctx, GPoint(x + body_w - 17, middle - 8), 3);
  graphics_context_set_fill_color(ctx, p.numeral);
  graphics_fill_circle(ctx, GPoint(x + body_w - 16, middle - 9), 1);
  line(ctx, edge, x + body_w - 11, middle + 7,
       x + body_w - 4, middle + 8, 1);
}

static uint32_t fish_resource_id(int width, int scene, bool left) {
  static const uint32_t emery[] = {
    RESOURCE_ID_FISH_EMERY_CASTLE, RESOURCE_ID_FISH_EMERY_KELP,
    RESOURCE_ID_FISH_EMERY_REEF, RESOURCE_ID_FISH_EMERY_GRAYSCALE,
    RESOURCE_ID_FISH_EMERY_VIOLET, RESOURCE_ID_FISH_EMERY_SEA_GREEN
  };
  static const uint32_t gabbro[] = {
    RESOURCE_ID_FISH_GABBRO_CASTLE, RESOURCE_ID_FISH_GABBRO_KELP,
    RESOURCE_ID_FISH_GABBRO_REEF, RESOURCE_ID_FISH_GABBRO_GRAYSCALE,
    RESOURCE_ID_FISH_GABBRO_VIOLET, RESOURCE_ID_FISH_GABBRO_SEA_GREEN
  };
  static const uint32_t emery_left[] = {
    RESOURCE_ID_FISH_EMERY_CASTLE_LEFT, RESOURCE_ID_FISH_EMERY_KELP_LEFT,
    RESOURCE_ID_FISH_EMERY_REEF_LEFT, RESOURCE_ID_FISH_EMERY_GRAYSCALE_LEFT,
    RESOURCE_ID_FISH_EMERY_VIOLET_LEFT, RESOURCE_ID_FISH_EMERY_SEA_GREEN_LEFT
  };
  static const uint32_t gabbro_left[] = {
    RESOURCE_ID_FISH_GABBRO_CASTLE_LEFT, RESOURCE_ID_FISH_GABBRO_KELP_LEFT,
    RESOURCE_ID_FISH_GABBRO_REEF_LEFT, RESOURCE_ID_FISH_GABBRO_GRAYSCALE_LEFT,
    RESOURCE_ID_FISH_GABBRO_VIOLET_LEFT, RESOURCE_ID_FISH_GABBRO_SEA_GREEN_LEFT
  };
  int index = scene >= 0 && scene < 6 ? scene : 0;
  return width >= 240 ? (left ? gabbro_left[index] : gabbro[index]) :
                        (left ? emery_left[index] : emery[index]);
}

static void reload_fish_bitmap(int variant) {
  if (!s_face) return;
  if (s_fish_bitmap && s_fish_variant == variant && s_loaded_left == s_facing_left) return;
  if (s_fish_bitmap) gbitmap_destroy(s_fish_bitmap);
  int width = layer_get_bounds(s_face).size.w;
  s_fish_bitmap = gbitmap_create_with_resource(fish_resource_id(width, variant, s_facing_left));
  s_fish_variant = variant;
  s_loaded_left = s_facing_left;
}

static uint32_t background_resource_id(int width, int scene, bool night) {
  static const uint32_t emery[4][2] = {
    {RESOURCE_ID_BACKGROUND_EMERY_CASTLE_DAY, RESOURCE_ID_BACKGROUND_EMERY_CASTLE_NIGHT},
    {RESOURCE_ID_BACKGROUND_EMERY_KELP_DAY, RESOURCE_ID_BACKGROUND_EMERY_KELP_NIGHT},
    {RESOURCE_ID_BACKGROUND_EMERY_REEF_DAY, RESOURCE_ID_BACKGROUND_EMERY_REEF_NIGHT},
    {RESOURCE_ID_BACKGROUND_EMERY_GRAYSCALE_DAY, RESOURCE_ID_BACKGROUND_EMERY_GRAYSCALE_NIGHT}
  };
  static const uint32_t gabbro[4][2] = {
    {RESOURCE_ID_BACKGROUND_GABBRO_CASTLE_DAY, RESOURCE_ID_BACKGROUND_GABBRO_CASTLE_NIGHT},
    {RESOURCE_ID_BACKGROUND_GABBRO_KELP_DAY, RESOURCE_ID_BACKGROUND_GABBRO_KELP_NIGHT},
    {RESOURCE_ID_BACKGROUND_GABBRO_REEF_DAY, RESOURCE_ID_BACKGROUND_GABBRO_REEF_NIGHT},
    {RESOURCE_ID_BACKGROUND_GABBRO_GRAYSCALE_DAY, RESOURCE_ID_BACKGROUND_GABBRO_GRAYSCALE_NIGHT}
  };
  int index = scene >= 0 && scene < 4 ? scene : 0;
  return width >= 240 ? gabbro[index][night] : emery[index][night];
}

static void reload_background_bitmap(int variant, bool night) {
  if (!s_face) return;
  if (s_background_bitmap && s_background_variant == variant &&
      s_background_night == night) return;
  if (s_background_bitmap) gbitmap_destroy(s_background_bitmap);
  int width = layer_get_bounds(s_face).size.w;
  s_background_bitmap = gbitmap_create_with_resource(
      background_resource_id(width, variant, night));
  s_background_variant = variant;
  s_background_night = night;
}

static void draw_clock_fish(GContext *ctx, int w, int h, Palette p, const struct tm *now) {
  if (!s_fish_bitmap) {
    draw_clock_fish_vector(ctx, w, h, p, now);
    return;
  }
  int sprite_w = w >= 240 ? 216 : 184;
  int sprite_h = w >= 240 ? 116 : 100;
  int x = (w - sprite_w) / 2 + fish_shift(w);
  int y = h / 2 - sprite_h / 2 - 10 + fish_bob();
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_fish_bitmap, GRect(x, y, sprite_w, sprite_h));
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  char time_text[8];
  strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%H:%M" : "%I:%M", now);
  if (!clock_is_24h_style() && time_text[0] == '0') {
    for (int i = 0; time_text[i]; i++) time_text[i] = time_text[i + 1];
  }
  graphics_context_set_text_color(ctx, s_fish_variant == 3 ? GColorWhite : color(0xFFF1C6));
  graphics_draw_text(ctx, time_text,
                     fonts_get_system_font(w >= 240 ? FONT_KEY_BITHAM_34_MEDIUM_NUMBERS :
                                                        FONT_KEY_BITHAM_30_BLACK),
                     GRect(x + (w >= 240 ? (s_facing_left ? 45 : 52) :
                                             (s_facing_left ? 39 : 45)),
                           h / 2 - 28 + fish_bob(), w >= 240 ? 118 : 102, 42),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void draw_battery_fish(GContext *ctx, int w, int h, Palette p, bool night) {
  if (!s_battery_visible) return;
  int x = w / 2 - 23, y = h - 50;
  GColor level = s_render_scene == 3 ? (night ? GColorWhite : GColorBlack) :
                 (s_battery_percent <= 20 ? color(0xDF4848) :
                  (s_battery_percent <= 50 ? color(0xE8AB40) : color(0x52C578)));
  GPoint tail[] = {
    GPoint(x + 3, y + 11), GPoint(x - 13, y + 1),
    GPoint(x - 9, y + 11), GPoint(x - 13, y + 22)
  };
  fill_polygon(ctx, tail, 4, level, p.deep);
  GPoint body[] = {
    GPoint(x, y + 11), GPoint(x + 5, y + 4), GPoint(x + 17, y + 1),
    GPoint(x + 35, y + 3), GPoint(x + 46, y + 11),
    GPoint(x + 35, y + 21), GPoint(x + 17, y + 23), GPoint(x + 5, y + 18)
  };
  fill_polygon(ctx, body, 8, level, p.deep);
  char label[6];
  snprintf(label, sizeof(label), "%d%%", s_battery_percent);
  graphics_context_set_text_color(ctx, s_render_scene == 3 && !night ? GColorWhite : GColorBlack);
  graphics_draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                     GRect(x + 2, y - 1, 40, 23), GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);
  graphics_context_set_fill_color(ctx, s_render_scene == 3 && !night ? GColorWhite : GColorBlack);
  graphics_fill_circle(ctx, GPoint(x + 40, y + 6), 1);
}

static void draw_date(GContext *ctx, int w, int h, Palette p, const struct tm *now, bool night) {
  if (s_date == 0) return;
  char date_text[16];
  const char *format = s_date == 1 ? "%Y-%m-%d" :
                       (s_date == 2 ? "%m/%d/%Y" : "%d/%m/%Y");
  strftime(date_text, sizeof(date_text), format, now);
  int top = w >= 240 ? 30 : 8;
  bool pale = s_render_scene == 3 && !night;
  graphics_context_set_fill_color(ctx, pale ? GColorWhite : color(0x082747));
  graphics_fill_rect(ctx, GRect(w / 2 - 52, top + 2, 104, 18), 5, GCornersAll);
  GColor ink = pale ? GColorBlack : GColorWhite;
  graphics_context_set_text_color(ctx, ink);
  graphics_draw_text(ctx, date_text, fonts_get_system_font(FONT_KEY_GOTHIC_18),
                     GRect(w / 2 - 60, top, 120, 23), GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);
}

static void face_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int w = bounds.size.w, h = bounds.size.h;
  time_t epoch = time(NULL);
  struct tm *now = localtime(&epoch);
  bool night = s_mode == 1 || (s_mode == 2 && (now->tm_hour >= 19 || now->tm_hour < 7));
  uint32_t day_key = (uint32_t)(now->tm_year * 366 + now->tm_yday);
  int fish_variant = selection_resolve(s_fish_color, s_scene, 6, day_key, 0x51f15e0a);
  int background_variant = selection_resolve(s_background, s_scene, 4, day_key, 0x9b8d28ca);
  s_render_scene = background_variant;
  Palette p = palette(background_variant, night);
  reload_fish_bitmap(fish_variant);
  reload_background_bitmap(background_variant, night);
  if (s_background_bitmap) {
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
    graphics_draw_bitmap_in_rect(ctx, s_background_bitmap, bounds);
  } else {
    graphics_context_set_fill_color(ctx, p.water);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
    draw_light_rays(ctx, w, h, night);
    graphics_context_set_fill_color(ctx, p.deep);
    graphics_fill_rect(ctx, GRect(0, h / 2 + 22, w, h / 2), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, p.sand);
    graphics_fill_rect(ctx, GRect(0, h - (w >= 240 ? 50 : 27), w, 50), 0, GCornerNone);
    draw_plants(ctx, w, h, p);
    draw_scene_details(ctx, w, h, p);
    draw_bubbles(ctx, w, h, p);
    int school_shift = s_swim_step < 0 ? 0 : (s_swim_step - 5) / 2;
    draw_small_fish(ctx, w / 4 + school_shift, h / 4, 5, p.fin, p.numeral, false);
    draw_small_fish(ctx, w / 4 - 17 + school_shift, h / 4 + 11, 3, p.detail, p.fish, true);
    draw_small_fish(ctx, w * 3 / 4 - school_shift, h / 3, 4, p.detail, p.fish, true);
  }
  draw_scene_life(ctx, w, h, p);
  draw_clock_fish(ctx, w, h, palette(fish_variant, night), now);
  draw_battery_fish(ctx, w, h, p, night);
  draw_date(ctx, w, h, p, now, night);
}

static void swim_frame(void *context) {
  s_swim_timer = NULL;
  if (++s_swim_step >= 11) s_swim_step = -1;
  layer_mark_dirty(s_face);
  if (s_swim_step >= 0) s_swim_timer = app_timer_register(180, swim_frame, NULL);
}

static void start_swim(void) {
  if (s_motion == 0 || s_swim_timer) return;
  s_facing_left = !s_facing_left;
  s_swim_step = 0;
  layer_mark_dirty(s_face);
  s_swim_timer = app_timer_register(180, swim_frame, NULL);
}

static void ambient_begin(void *context);

static void schedule_ambient(uint32_t delay_ms) {
  if (s_motion == 0) return;
  s_ambient_timer = app_timer_register(delay_ms, ambient_begin, NULL);
}

static void ambient_frame_tick(void *context) {
  s_ambient_timer = NULL;
  if (s_motion == 0) return;
  if (s_ambient_frame < 5) {
    s_ambient_frame++;
    layer_mark_dirty(s_face);
    s_ambient_timer = app_timer_register(motion_frame_ms(s_motion), ambient_frame_tick, NULL);
  } else {
    s_ambient_frame = -1;
    layer_mark_dirty(s_face);
    schedule_ambient(motion_ambient_interval_ms(s_motion));
  }
}

static void ambient_begin(void *context) {
  s_ambient_timer = NULL;
  if (s_motion == 0) return;
  s_ambient_frame = 0;
  s_ambient_cycles++;
  int glide_every = motion_swim_every_cycles(s_motion);
  if (glide_every > 0 && s_ambient_cycles % glide_every == 0) start_swim();
  layer_mark_dirty(s_face);
  s_ambient_timer = app_timer_register(motion_frame_ms(s_motion), ambient_frame_tick, NULL);
}

static void set_motion_level(int level) {
  if (s_ambient_timer) app_timer_cancel(s_ambient_timer);
  if (s_swim_timer) app_timer_cancel(s_swim_timer);
  s_ambient_timer = NULL;
  s_swim_timer = NULL;
  s_ambient_frame = -1;
  s_swim_step = -1;
  s_ambient_cycles = 0;
  s_motion = level;
  layer_mark_dirty(s_face);
  if (level > 0) schedule_ambient(4000);
}

static void tap_handler(AccelAxisType axis, int32_t direction) { start_swim(); }
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_face);
}
static void battery_handler(BatteryChargeState state) {
  s_battery_percent = state.charge_percent;
  layer_mark_dirty(s_face);
}

static void inbox(DictionaryIterator *iterator, void *context) {
  Tuple *tuple;
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_SCENE))) {
    int value = tuple->value->int32;
    if (value >= 0 && value <= 3) {
      s_scene = value;
      persist_write_int(PERSIST_SCENE, value);
    }
  }
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_FISH_COLOR))) {
    int value = tuple->value->int32;
    if (value >= 0 && value <= 7) {
      s_fish_color = value;
      persist_write_int(PERSIST_FISH_COLOR, value);
    }
  }
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_BACKGROUND))) {
    int value = tuple->value->int32;
    if (value >= 0 && value <= 5) {
      s_background = value;
      persist_write_int(PERSIST_BACKGROUND, value);
    }
  }
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_DATE))) {
    int value = tuple->value->int32;
    if (value >= 0 && value <= 3) { s_date = value; persist_write_int(PERSIST_DATE, value); }
  }
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_MODE))) {
    int value = tuple->value->int32;
    if (value >= 0 && value <= 2) { s_mode = value; persist_write_int(PERSIST_MODE, value); }
  }
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_MOTION))) {
    int value = tuple->value->int32;
    if (value >= 0 && value <= 2) {
      set_motion_level(value);
      persist_write_int(PERSIST_ANIMATION, value);
    }
  }
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_BATTERY))) {
    s_battery_visible = tuple->value->int32 != 0;
    persist_write_bool(PERSIST_BATTERY, s_battery_visible);
  }
  layer_mark_dirty(s_face);
}

static void init(void) {
  s_scene = persist_read_int(PERSIST_SCENE);
  if (s_scene < 0 || s_scene > 3) s_scene = 0;
  if (persist_exists(PERSIST_FISH_COLOR)) s_fish_color = persist_read_int(PERSIST_FISH_COLOR);
  if (persist_exists(PERSIST_BACKGROUND)) s_background = persist_read_int(PERSIST_BACKGROUND);
  if (s_fish_color < 0 || s_fish_color > 7) s_fish_color = 0;
  if (s_background < 0 || s_background > 5) s_background = 0;
  s_date = persist_read_int(PERSIST_DATE);
  if (persist_exists(PERSIST_MODE)) s_mode = persist_read_int(PERSIST_MODE);
  if (persist_exists(PERSIST_ANIMATION)) s_motion = persist_read_int(PERSIST_ANIMATION);
  else if (persist_exists(PERSIST_MOTION)) s_motion = persist_read_bool(PERSIST_MOTION) ? 1 : 0;
  if (s_motion < 0 || s_motion > 2) s_motion = 1;
  if (persist_exists(PERSIST_BATTERY)) s_battery_visible = persist_read_bool(PERSIST_BATTERY);
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  s_face = layer_create(layer_get_bounds(window_get_root_layer(s_window)));
  layer_set_update_proc(s_face, face_update);
  layer_add_child(window_get_root_layer(s_window), s_face);
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
  battery_state_service_subscribe(battery_handler);
  battery_handler(battery_state_service_peek());
  app_message_register_inbox_received(inbox);
  app_message_open(128, 32);
  set_motion_level(s_motion);
}

static void deinit(void) {
  if (s_swim_timer) app_timer_cancel(s_swim_timer);
  if (s_ambient_timer) app_timer_cancel(s_ambient_timer);
  accel_tap_service_unsubscribe();
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks();
  if (s_fish_bitmap) gbitmap_destroy(s_fish_bitmap);
  if (s_background_bitmap) gbitmap_destroy(s_background_bitmap);
  layer_destroy(s_face);
  window_destroy(s_window);
}

int main(void) { init(); app_event_loop(); deinit(); }
