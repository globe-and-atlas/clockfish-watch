#include <pebble.h>

enum { KEY_SCENE = 1, KEY_DATE, KEY_MOTION, KEY_MODE, KEY_BATTERY };
enum { PERSIST_SCENE = 1, PERSIST_DATE, PERSIST_MOTION, PERSIST_MODE, PERSIST_BATTERY };

typedef struct {
  GColor water, deep, sand, plant, detail, fish, fin, numeral, bubble, battery;
} Palette;

static Window *s_window;
static Layer *s_face;
static AppTimer *s_swim_timer;
static int s_scene;
static int s_date;
static int s_mode = 2;
static bool s_motion = true;
static bool s_battery_visible = true;
static int s_battery_percent = 100;
static int s_swim_step = -1;

static GColor color(uint32_t rgb) { return GColorFromHEX(rgb); }

static Palette palette(bool night) {
  Palette p;
  if (s_scene == 1) {
    p = (Palette){color(0x0A5262), color(0x073746), color(0x174E4B),
      color(0x2F996E), color(0x97CFC1), color(0x175A81), color(0x6DD6BD),
      color(0xFFFFFF), color(0xD5F6E5), color(0xE7B64E)};
  } else if (s_scene == 2) {
    p = (Palette){color(0x091536), color(0x050B21), color(0x242457),
      color(0x6544A2), color(0xEA5090), color(0x096A85), color(0x2BCDE5),
      color(0xFFFFFF), color(0x42D3EB), color(0xE5A137)};
  } else if (s_scene == 3) {
    p = (Palette){color(0xAAAAAA), color(0x555555), color(0xAAAAAA),
      color(0x555555), color(0x222222), color(0x222222), color(0x555555),
      color(0xFFFFFF), color(0xFFFFFF), color(0x555555)};
  } else {
    p = (Palette){color(0x39AEDD), color(0x267FAF), color(0xE9D19A),
      color(0x399951), color(0xDCE4D5), color(0xA34926), color(0xF1AC55),
      color(0xFFFFFF), color(0xFFFFFF), color(0xE7B64E)};
  }
  if (!night && s_scene == 2) {
    p.water = color(0x24568E);
    p.deep = color(0x143B70);
    p.sand = color(0x634C8C);
    p.plant = color(0x976BC0);
    p.detail = color(0xF385A9);
  } else if (night && s_scene == 3) {
    p.water = color(0x444444);
    p.deep = color(0x111111);
    p.sand = color(0x666666);
    p.plant = color(0xAAAAAA);
    p.detail = color(0xDDDDDD);
    p.fish = color(0x555555);
    p.fin = color(0x999999);
    p.bubble = GColorWhite;
  } else if (night && s_scene != 2) {
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
    for (int stem = 0; stem < (s_scene == 1 ? 4 : 3); stem++) {
      int root = x + (stem - 1) * (s_scene == 1 ? 12 : 10);
      int top = bottom - (s_scene == 1 ? 92 : 46) - stem * (s_scene == 1 ? 9 : 6);
      int tip = root + (side ? -8 : 8) + (stem - 1) * 3;
      line(ctx, p.deep, root + 1, bottom, tip + 1, top, s_scene == 1 ? 5 : 3);
      line(ctx, p.plant, root, bottom, tip, top, s_scene == 1 ? 3 : 2);
      for (int leaf = 0; leaf < (s_scene == 1 ? 4 : 2); leaf++) {
        int yy = bottom - 16 - leaf * (s_scene == 1 ? 19 : 18);
        if (yy <= top + 8) continue;
        int stem_x = root + (tip - root) * (bottom - yy) / (bottom - top);
        int spread = (leaf % 2 ? -1 : 1) * (side ? -1 : 1);
        int length = s_scene == 1 ? 23 : 13;
        int rise = s_scene == 1 ? 17 : 10;
        GPoint blade[] = {
          GPoint(stem_x, yy + 5),
          GPoint(stem_x + spread * (length / 2), yy - 7),
          GPoint(stem_x + spread * length, yy - rise),
          GPoint(stem_x + spread * (length - 6), yy - 4),
          GPoint(stem_x + spread * 4, yy + 7)
        };
        GColor blade_color = (s_scene == 1 && leaf % 2) ? p.detail : p.plant;
        fill_polygon(ctx, blade, 5, blade_color, p.deep);
        if (s_scene == 1) {
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
  if (s_scene == 0) {
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
  } else if (s_scene == 1) {
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
  } else if (s_scene == 2) {
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

static void draw_small_fish(GContext *ctx, int x, int y, int size, GColor body, GColor eye) {
  graphics_context_set_fill_color(ctx, body);
  graphics_fill_circle(ctx, GPoint(x, y), size);
  fill_triangle(ctx, GPoint(x - size + 1, y), GPoint(x - size * 2, y - size),
                GPoint(x - size * 2, y + size));
  graphics_context_set_fill_color(ctx, eye);
  graphics_fill_circle(ctx, GPoint(x + size / 2, y - 1), 1);
}

static void draw_light_rays(GContext *ctx, int w, int h, bool night) {
  if (night || s_scene != 0) return;
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
  static const int sequence[] = {0, -5, -10, -15, -10, 0, 10, 15, 10, 5, 0};
  if (s_swim_step < 0) return 0;
  int value = sequence[s_swim_step];
  // Keep the fuller tail on-screen throughout the swim.
  return w >= 240 ? value * 2 / 3 : value / 2;
}

static int fish_bob(void) {
  static const int sequence[] = {0, -2, -4, -6, -8, -4, 0, 4, 8, 4, 0};
  return s_swim_step < 0 ? 0 : sequence[s_swim_step];
}

static void draw_clock_fish(GContext *ctx, int w, int h, Palette p, const struct tm *now) {
  int body_w = w >= 240 ? 178 : 146;
  int body_h = w >= 240 ? 80 : 72;
  // Center the numeral area; the tail extends farther left than the snout extends right.
  int x = (w - body_w) / 2 + 11 + fish_shift(w);
  int y = h / 2 - body_h / 2 - 10 + fish_bob();
  int middle = y + body_h / 2;
  int tail = w >= 240 ? 30 : 25;
  int flap = s_swim_step >= 0 && s_swim_step % 2 ? 3 : 0;
  GColor edge = s_scene == 3 ? p.detail : p.deep;
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

static void draw_battery_fish(GContext *ctx, int w, int h, Palette p, bool night) {
  if (!s_battery_visible) return;
  int x = w / 2 - 23, y = h - 50;
  GColor level = s_scene == 3 ? (night ? GColorWhite : GColorBlack) :
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
  graphics_context_set_text_color(ctx, s_scene == 3 && !night ? GColorWhite : GColorBlack);
  graphics_draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                     GRect(x + 2, y - 1, 40, 23), GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);
  graphics_context_set_fill_color(ctx, s_scene == 3 && !night ? GColorWhite : GColorBlack);
  graphics_fill_circle(ctx, GPoint(x + 40, y + 6), 1);
}

static void draw_date(GContext *ctx, int w, int h, Palette p, const struct tm *now, bool night) {
  if (s_date == 0) return;
  char date_text[16];
  const char *format = s_date == 1 ? "%Y-%m-%d" :
                       (s_date == 2 ? "%m/%d/%Y" : "%d/%m/%Y");
  strftime(date_text, sizeof(date_text), format, now);
  GColor ink = w >= 240 ? (s_scene == 3 && !night ? p.fish : p.numeral) :
               (night || s_scene == 2 || s_scene == 1 ? p.numeral : p.fish);
  graphics_context_set_text_color(ctx, ink);
  graphics_draw_text(ctx, date_text, fonts_get_system_font(FONT_KEY_GOTHIC_18),
                     GRect(w / 2 - 60, w >= 240 ? 30 : 8, 120, 23), GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);
}

static void face_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int w = bounds.size.w, h = bounds.size.h;
  time_t epoch = time(NULL);
  struct tm *now = localtime(&epoch);
  bool night = s_mode == 1 || (s_mode == 2 && (now->tm_hour >= 19 || now->tm_hour < 7));
  Palette p = palette(night);
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
  draw_small_fish(ctx, w / 4 + school_shift, h / 4, 5, p.fin, p.numeral);
  draw_small_fish(ctx, w / 4 - 17 + school_shift, h / 4 + 11, 3, p.detail, p.fish);
  draw_small_fish(ctx, w * 3 / 4 - school_shift, h / 3, 4, p.detail, p.fish);
  draw_clock_fish(ctx, w, h, p, now);
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
  if (!s_motion || s_swim_timer) return;
  s_swim_step = 0;
  layer_mark_dirty(s_face);
  s_swim_timer = app_timer_register(180, swim_frame, NULL);
}

static void tap_handler(AccelAxisType axis, int32_t direction) { start_swim(); }
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_face);
  start_swim();
}
static void battery_handler(BatteryChargeState state) {
  s_battery_percent = state.charge_percent;
  layer_mark_dirty(s_face);
}

static void inbox(DictionaryIterator *iterator, void *context) {
  Tuple *tuple;
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_SCENE))) {
    int value = tuple->value->int32;
    if (value >= 0 && value <= 3) { s_scene = value; persist_write_int(PERSIST_SCENE, value); }
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
    s_motion = tuple->value->int32 != 0;
    persist_write_bool(PERSIST_MOTION, s_motion);
  }
  if ((tuple = dict_find(iterator, MESSAGE_KEY_SETTING_BATTERY))) {
    s_battery_visible = tuple->value->int32 != 0;
    persist_write_bool(PERSIST_BATTERY, s_battery_visible);
  }
  layer_mark_dirty(s_face);
}

static void init(void) {
  s_scene = persist_read_int(PERSIST_SCENE);
  s_date = persist_read_int(PERSIST_DATE);
  if (persist_exists(PERSIST_MODE)) s_mode = persist_read_int(PERSIST_MODE);
  if (persist_exists(PERSIST_MOTION)) s_motion = persist_read_bool(PERSIST_MOTION);
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
}

static void deinit(void) {
  if (s_swim_timer) app_timer_cancel(s_swim_timer);
  accel_tap_service_unsubscribe();
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks();
  layer_destroy(s_face);
  window_destroy(s_window);
}

int main(void) { init(); app_event_loop(); deinit(); }
