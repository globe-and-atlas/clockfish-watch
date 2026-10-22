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
      color(0x2F996E), color(0x97CFC1), color(0x103E56), color(0x52A9AC),
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
      color(0x399951), color(0xDCE4D5), color(0x145577), color(0xE98943),
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
    for (int stem = 0; stem < 3; stem++) {
      int root = x + (stem - 1) * 9;
      int top = bottom - 42 - stem * 13;
      int bend = side ? -6 : 6;
      line(ctx, p.plant, root, bottom, root + bend, top, 3);
      line(ctx, p.plant, root + bend / 2, bottom - 21, root + bend + (side ? -9 : 9), bottom - 35, 3);
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
    line(ctx, p.bubble, 14, 21, 62, 21, 1);
    line(ctx, p.bubble, w - 69, 28, w - 18, 28, 1);
  } else if (s_scene == 1) {
    int jelly_x = w * 3 / 4, jelly_y = h / 5;
    graphics_context_set_fill_color(ctx, p.detail);
    graphics_fill_circle(ctx, GPoint(jelly_x, jelly_y), 9);
    graphics_context_set_fill_color(ctx, p.water);
    graphics_fill_rect(ctx, GRect(jelly_x - 9, jelly_y, 18, 9), 0, GCornerNone);
    for (int i = -1; i <= 1; i++)
      line(ctx, p.detail, jelly_x + i * 6, jelly_y + 1,
           jelly_x + i * 8, jelly_y + 19, 1);
    for (int i = 0; i < 5; i++) {
      int x = 8 + i * (w - 16) / 4;
      line(ctx, p.plant, x, floor, x + (i % 2 ? 6 : -6), floor - 30 - i * 3, 2);
    }
  } else if (s_scene == 2) {
    for (int side = 0; side < 2; side++) {
      int x = side ? w - 27 : 27;
      line(ctx, p.detail, x, floor, x, floor - 27, 4);
      line(ctx, p.detail, x, floor - 13, x + (side ? -11 : 11), floor - 29, 3);
      line(ctx, p.detail, x, floor - 18, x + (side ? 10 : -10), floor - 36, 3);
      line(ctx, p.plant, x + (side ? -15 : 15), floor,
           x + (side ? -20 : 20), floor - 17, 3);
    }
    graphics_context_set_fill_color(ctx, p.bubble);
    for (int i = 0; i < 6; i++)
      graphics_fill_circle(ctx, GPoint(15 + ((i * 37) % (w - 30)), 20 + (i * 17) % 45), 1);
  } else {
    graphics_context_set_fill_color(ctx, p.plant);
    for (int i = 0; i < 3; i++) {
      int x = 14 + i * 9;
      graphics_fill_circle(ctx, GPoint(x, floor + 2), 5 + i % 2);
    }
    line(ctx, p.detail, w / 2 + 35, 29, w / 2 + 67, 29, 1);
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

static int fish_shift(int w) {
  static const int sequence[] = {0, -5, -10, -15, -10, 0, 10, 15, 10, 5, 0};
  if (s_swim_step < 0) return 0;
  int value = sequence[s_swim_step];
  return value;
}

static int fish_bob(void) {
  static const int sequence[] = {0, -2, -4, -6, -8, -4, 0, 4, 8, 4, 0};
  return s_swim_step < 0 ? 0 : sequence[s_swim_step];
}

static void draw_clock_fish(GContext *ctx, int w, int h, Palette p, const struct tm *now) {
  int body_w = w >= 240 ? 178 : 146;
  int body_h = w >= 240 ? 66 : 58;
  int x = (w - body_w) / 2 + fish_shift(w);
  int y = h / 2 - body_h / 2 - 10 + fish_bob();
  int tail = w >= 240 ? 17 : 12;
  graphics_context_set_fill_color(ctx, p.fin);
  fill_triangle(ctx, GPoint(x + 3, y + body_h / 2), GPoint(x - tail, y + 2),
                GPoint(x - tail, y + body_h - 2));
  fill_triangle(ctx, GPoint(x + body_w / 2 - 8, y + 6),
                GPoint(x + body_w / 2 + 6, y - 15),
                GPoint(x + body_w / 2 + 22, y + 6));
  if (s_swim_step >= 0 && s_swim_step % 2) {
    fill_triangle(ctx, GPoint(x + body_w / 2, y + body_h - 2),
                  GPoint(x + body_w / 2 + 15, y + body_h + 12),
                  GPoint(x + body_w / 2 + 24, y + body_h - 2));
  }
  graphics_context_set_fill_color(ctx, p.fish);
  graphics_fill_rect(ctx, GRect(x, y, body_w, body_h), body_h / 2, GCornersAll);
  graphics_context_set_stroke_color(ctx, p.detail);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_round_rect(ctx, GRect(x, y, body_w, body_h), body_h / 2);

  char time_text[8];
  strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%H:%M" : "%I:%M", now);
  if (!clock_is_24h_style() && time_text[0] == '0') {
    for (int i = 0; time_text[i]; i++) time_text[i] = time_text[i + 1];
  }
  graphics_context_set_text_color(ctx, p.numeral);
  graphics_draw_text(ctx, time_text, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS),
                     GRect(x + 2, y + body_h / 2 - 28, body_w - 19, 56),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  graphics_context_set_fill_color(ctx, p.numeral);
  graphics_fill_circle(ctx, GPoint(x + body_w - 13, y + body_h / 2 - 8), 3);
  graphics_context_set_stroke_color(ctx, p.numeral);
  graphics_draw_line(ctx, GPoint(x + body_w - 12, y + body_h / 2 + 8),
                     GPoint(x + body_w - 6, y + body_h / 2 + 8));
}

static void draw_battery_fish(GContext *ctx, int w, int h, Palette p, bool night) {
  if (!s_battery_visible) return;
  int x = w / 2 - 23, y = h - 50;
  GColor level = s_scene == 3 ? (night ? GColorWhite : GColorBlack) :
                 (s_battery_percent <= 20 ? color(0xDF4848) :
                  (s_battery_percent <= 50 ? color(0xE8AB40) : color(0x52C578)));
  graphics_context_set_fill_color(ctx, level);
  fill_triangle(ctx, GPoint(x + 2, y + 11), GPoint(x - 13, y + 1), GPoint(x - 13, y + 21));
  graphics_fill_rect(ctx, GRect(x, y, 46, 23), 11, GCornersAll);
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
                     GRect(w / 2 - 60, w >= 240 ? 35 : h - 27, 120, 23), GTextOverflowModeTrailingEllipsis,
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
  graphics_context_set_fill_color(ctx, p.deep);
  graphics_fill_rect(ctx, GRect(0, h / 2 + 22, w, h / 2), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, p.sand);
  graphics_fill_rect(ctx, GRect(0, h - (w >= 240 ? 50 : 27), w, 50), 0, GCornerNone);
  draw_plants(ctx, w, h, p);
  draw_scene_details(ctx, w, h, p);
  draw_bubbles(ctx, w, h, p);
  draw_small_fish(ctx, w / 4, h / 4, 5, p.fin, p.numeral);
  draw_small_fish(ctx, w * 3 / 4, h / 3, 4, p.detail, p.fish);
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
