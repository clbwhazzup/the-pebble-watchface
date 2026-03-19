#include <pebble.h>
#define SETTINGS_KEY 1

// App Settings for watchface
typedef struct ClaySettings {
  GColor BackgroundColor;
  GColor TextColor;
  char TemperatureUnit[8]; // false = Celsius, true = Fahrenheit
  bool ShowDate;
  bool ShowCity;
  bool ShowHealth;
  bool ShowConditions;
  bool ShowHighLow;
  bool ShowTemp;
  bool ShowRiseSet;
  char PercentChoice[8]; // hide, humid, precip
  char SecondsChoice[8]; // hide, tap, show
  char BatteryChoice[8]; // bar, percent, both, hide
  int SecondsLimit; // 0-60
} ClaySettings;
static ClaySettings settings;


// Global vars
static Window* s_window;
Layer* window_layer;
static TextLayer* s_time_layer;
static TextLayer* s_seconds_layer;
static TextLayer* s_weather_layer;
static TextLayer* s_conditions_layer;
static TextLayer* s_humid_layer;
static TextLayer* s_date_layer;
static TextLayer* s_hl_layer;
static TextLayer* s_rise_layer;
static TextLayer* s_set_layer;
static TextLayer* s_city_layer;
static TextLayer* s_steps_layer;
static TextLayer* s_hr_layer;
static TextLayer* s_batt_percent_layer;
static Layer* s_battery_layer;

static GFont s_font_time;
static GFont s_font_medium;
static GFont s_font_small;
static GFont s_font_weather;

static GRect bounds;
static AppTimer *s_seconds_timer;

static char steps_buffer[16] = "------";
static char hr_buffer[16] = "---";

int seconds_choice;
int time_y;
int date_y;
int hl_y;
int weather_y;
int condition_y;
int humid_y;
int rise_set_y;
int city_y;
int health_y;
bool show_seconds_now;
bool health_available;
int battery_level;
char batt[8];


// Settings handling
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.TextColor = GColorWhite;
  strcpy(settings.TemperatureUnit, "false");
  strcpy(settings.PercentChoice, "humid");
  settings.ShowDate = true;
  settings.ShowCity = true;
  settings.ShowHealth = false;
  settings.ShowConditions = true;
  settings.ShowHighLow = true;
  settings.ShowTemp = true;
  settings.ShowRiseSet = true;
  strcpy(settings.PercentChoice, "humid");
  strcpy(settings.SecondsChoice, "show");
  strcpy(settings.BatteryChoice, "both");
  settings.SecondsLimit = 5000;
}
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}
static void prv_load_settings() {
  prv_default_settings();
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}
static void prv_update_display() {
  window_set_background_color(s_window, settings.BackgroundColor);
  text_layer_set_text_color(s_time_layer, settings.TextColor);
  text_layer_set_text_color(s_date_layer, settings.TextColor);
  text_layer_set_text_color(s_weather_layer, settings.TextColor);
  text_layer_set_text_color(s_seconds_layer, settings.TextColor);
  text_layer_set_text_color(s_conditions_layer, settings.TextColor);
  text_layer_set_text_color(s_hl_layer, settings.TextColor);
  text_layer_set_text_color(s_rise_layer, settings.TextColor);
  text_layer_set_text_color(s_set_layer, settings.TextColor);
  text_layer_set_text_color(s_city_layer, settings.TextColor);
  text_layer_set_text_color(s_steps_layer, settings.TextColor);
  text_layer_set_text_color(s_hr_layer, settings.TextColor);
  text_layer_set_text_color(s_humid_layer, settings.TextColor);

  layer_set_hidden(text_layer_get_layer(s_date_layer), !settings.ShowDate);
  layer_set_hidden(text_layer_get_layer(s_city_layer), !settings.ShowCity);
  layer_set_hidden(text_layer_get_layer(s_hr_layer), !settings.ShowHealth);
  layer_set_hidden(text_layer_get_layer(s_steps_layer), !settings.ShowHealth);
  layer_set_hidden(text_layer_get_layer(s_conditions_layer), !settings.ShowConditions);
  layer_set_hidden(text_layer_get_layer(s_hl_layer), !settings.ShowHighLow);
  layer_set_hidden(text_layer_get_layer(s_weather_layer), !settings.ShowTemp);
  layer_set_hidden(text_layer_get_layer(s_rise_layer), !settings.ShowRiseSet);
  layer_set_hidden(text_layer_get_layer(s_set_layer), !settings.ShowRiseSet);
  if (strncmp(settings.SecondsChoice, "hide", 4) == 0) {
    show_seconds_now = false;
    layer_set_hidden(text_layer_get_layer(s_seconds_layer), true);
  } else if (strncmp(settings.SecondsChoice, "tap", 3) == 0) {
    show_seconds_now = false;
    layer_set_hidden(text_layer_get_layer(s_seconds_layer), false);
    text_layer_set_text(s_seconds_layer, "--");
  } else if (strncmp(settings.SecondsChoice, "show", 4) == 0) {
    show_seconds_now = true;
    layer_set_hidden(text_layer_get_layer(s_seconds_layer), false);
  }

  if (strncmp(settings.PercentChoice, "hide", 4) == 0){
      layer_set_hidden(text_layer_get_layer(s_humid_layer), true);
  } else {
    layer_set_hidden(text_layer_get_layer(s_humid_layer), false);
  }

  if (strncmp(settings.BatteryChoice, "bar", 3) == 0) {
    layer_set_hidden(s_battery_layer, false);
    layer_set_hidden(text_layer_get_layer(s_batt_percent_layer), true);
  } else if (strncmp(settings.BatteryChoice, "percent", 6) == 0) {
    layer_set_hidden(s_battery_layer, true);
    layer_set_hidden(text_layer_get_layer(s_batt_percent_layer), false);
    text_layer_set_text_color(s_batt_percent_layer, settings.TextColor);
  } else if (strncmp(settings.BatteryChoice, "both", 4) == 0) {
    layer_set_hidden(s_battery_layer, false);
    layer_set_hidden(text_layer_get_layer(s_batt_percent_layer), false);
    text_layer_set_text_color(s_batt_percent_layer, settings.BackgroundColor);
  } else if (strncmp(settings.BatteryChoice, "hide", 4) == 0) {
    layer_set_hidden(s_battery_layer, true);
    layer_set_hidden(text_layer_get_layer(s_batt_percent_layer), true);
  }
  layer_mark_dirty(s_battery_layer);
}


// Positioning
static void set_positions(void) {
  bounds = layer_get_bounds(window_layer);
  int time_height = 42;
  int medium_height = 20;
  int small_height = 12;
  time_y = (bounds.size.h / 2) - 15;
  hl_y = 24;
  weather_y = 10;
  condition_y = -5;
  rise_set_y = bounds.size.h - small_height - 2;
  date_y = time_y - medium_height - 5;
  city_y = time_y + time_height + 8;
  health_y = city_y + small_height + 5;
  humid_y = condition_y + small_height / 2 + 5;
}


// City layer
static void create_city_layer(void) {
  s_city_layer = text_layer_create(
      GRect(0, city_y, bounds.size.w, 30));
  text_layer_set_font(s_city_layer, s_font_small);
  text_layer_set_text_color(s_city_layer, settings.TextColor);
  text_layer_set_background_color(s_city_layer, GColorClear);
  text_layer_set_text_alignment(s_city_layer, GTextAlignmentLeft);
  text_layer_set_text(s_city_layer, "-----------");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_city_layer));
};


// Weather handling
static void create_weather_layer(void) {
  s_weather_layer = text_layer_create(
      GRect(0, weather_y, bounds.size.w, 30));
  text_layer_set_font(s_weather_layer, s_font_medium);
  text_layer_set_text_color(s_weather_layer, settings.TextColor);
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentLeft);
  text_layer_set_text(s_weather_layer, "----");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_weather_layer));
}
static void create_conditons_layer(void){
  s_conditions_layer = text_layer_create(
    GRect(-45, condition_y, bounds.size.w, 30));
    text_layer_set_font(s_conditions_layer, s_font_weather);
    text_layer_set_text_color(s_conditions_layer, settings.TextColor);
    text_layer_set_background_color(s_conditions_layer, GColorClear);
    text_layer_set_text_alignment(s_conditions_layer, GTextAlignmentRight);
    text_layer_set_text(s_conditions_layer, "0");
    layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_conditions_layer));
}
static void create_hl_layer(void) {
  s_hl_layer = text_layer_create(
      GRect(0, hl_y, bounds.size.w, 30));
  text_layer_set_font(s_hl_layer, s_font_small);
  text_layer_set_text_color(s_hl_layer, settings.TextColor);
  text_layer_set_background_color(s_hl_layer, GColorClear);
  text_layer_set_text_alignment(s_hl_layer, GTextAlignmentRight);
  text_layer_set_text(s_hl_layer, "-------");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_hl_layer));
}
static void create_humid_layer(void) {
  s_humid_layer = text_layer_create(
      GRect(0, humid_y, bounds.size.w, 30));
  text_layer_set_font(s_humid_layer, s_font_small);
  text_layer_set_text_color(s_humid_layer, settings.TextColor);
  text_layer_set_background_color(s_humid_layer, GColorClear);
  text_layer_set_text_alignment(s_humid_layer, GTextAlignmentRight);
  text_layer_set_text(s_humid_layer, "----");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_humid_layer));
}


// Health handling
static void health_handler(HealthEventType event, void *context) {
  if (health_available && (event == HealthEventHeartRateUpdate || event == HealthEventSignificantUpdate)) {
    HealthMetric metric = HealthMetricStepCount;
    HealthServiceAccessibilityMask mask;
    bool any_data_available;
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    mask = health_service_metric_accessible(metric, start, end);
    any_data_available = mask & HealthServiceAccessibilityMaskAvailable;
    if (any_data_available) {
      APP_LOG(APP_LOG_LEVEL_INFO, "data available");
      HealthValue steps = health_service_sum_today(HealthMetricStepCount);
      HealthValue hr = health_service_peek_current_value(HealthMetricHeartRateBPM);
      APP_LOG(APP_LOG_LEVEL_INFO, "Steps: %d HR: %d", (int)steps, (int)hr);
      snprintf(steps_buffer, sizeof(steps_buffer), "%d", (int)steps);
      snprintf(hr_buffer, sizeof(hr_buffer), "%d", (int)hr);
      text_layer_set_text(s_steps_layer, steps_buffer);
      text_layer_set_text(s_hr_layer, hr_buffer);
    }
  }
}
static void create_steps_layer(void) {
  s_steps_layer = text_layer_create(
      GRect(0, health_y, bounds.size.w, 30));
  text_layer_set_font(s_steps_layer, s_font_small);
  text_layer_set_text_color(s_steps_layer, settings.TextColor);
  text_layer_set_background_color(s_steps_layer, GColorClear);
  text_layer_set_text_alignment(s_steps_layer, GTextAlignmentLeft);
  text_layer_set_text(s_steps_layer, "------");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_steps_layer));
}
static void create_hr_layer(void) {
  s_hr_layer = text_layer_create(
      GRect(0, health_y, bounds.size.w, 30));
  text_layer_set_font(s_hr_layer, s_font_small);
  text_layer_set_text_color(s_hr_layer, settings.TextColor);
  text_layer_set_background_color(s_hr_layer, GColorClear);
  text_layer_set_text_alignment(s_hr_layer, GTextAlignmentRight);
  text_layer_set_text(s_hr_layer, "---");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_hr_layer));
}


// Battery handling
static void battery_callback(BatteryChargeState state) {
  battery_level = state.charge_percent;
  layer_mark_dirty(s_battery_layer);
}
static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect l_bounds = layer_get_bounds(layer);
  // Find the width of the bar (inside the border)
  int bar_width = ((battery_level * (l_bounds.size.w - 4)) / 100);
  // Draw the border
  graphics_context_set_stroke_color(ctx, settings.TextColor);
  graphics_draw_round_rect(ctx, l_bounds, 2);
  // Choose color based on battery level
  GColor bar_color;
  if (battery_level <= 20) {
    bar_color = PBL_IF_COLOR_ELSE(GColorRed, settings.TextColor);
  } else if (battery_level <= 40) {
    bar_color = PBL_IF_COLOR_ELSE(GColorChromeYellow, settings.TextColor);
  } else {
    bar_color = PBL_IF_COLOR_ELSE(GColorGreen, settings.TextColor);
  }
  // Draw background
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, GRect(2, 2, l_bounds.size.w - 4, l_bounds.size.h - 4), 1, GCornerNone);
  // Draw the filled bar inside the border
  graphics_context_set_fill_color(ctx, bar_color);
  graphics_fill_rect(ctx, GRect(2, 2, bar_width, l_bounds.size.h - 4), 1, GCornerNone);

  // Update the battery percentage text
  snprintf(batt, sizeof(batt), "%d%%", battery_level);
  APP_LOG(APP_LOG_LEVEL_INFO, "Battery: %s", batt);
  text_layer_set_text(s_batt_percent_layer, batt);
}
static void create_battery_layer(void) {
  int bar_width = bounds.size.w / 3;
  int bar_height = 12;
  int bar_x = (bounds.size.w - bar_width) / 2;
  int bar_y = bounds.size.h - bar_height - 2;
  s_battery_layer = layer_create(
    GRect(bar_x, bar_y, bar_width, bar_height));
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  layer_add_child(window_get_root_layer(s_window), s_battery_layer);
}
static void create_batt_percent_layer(void) {
  int l_width = 48;
  int l_x = ((bounds.size.w - l_width) / 2) - 4;
  s_batt_percent_layer = text_layer_create(
      GRect(l_x, rise_set_y, l_width, 12));
  text_layer_set_font(s_batt_percent_layer, s_font_small);
  text_layer_set_text_color(s_batt_percent_layer, settings.BackgroundColor);
  text_layer_set_background_color(s_batt_percent_layer, GColorClear);
  text_layer_set_text_alignment(s_batt_percent_layer, GTextAlignmentRight);
  text_layer_set_text(s_batt_percent_layer, "----");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_batt_percent_layer));
}


// Time handling
static void create_date_layer(void) {
  s_date_layer = text_layer_create(
      GRect(0, date_y, bounds.size.w, 30));
  text_layer_set_font(s_date_layer, s_font_medium);
  text_layer_set_text_color(s_date_layer, settings.TextColor);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date_layer, "--- -- --");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_date_layer));
}
static void create_time_layer(void) {
  s_time_layer = text_layer_create(
      GRect(0, time_y, bounds.size.w, 50));
  text_layer_set_font(s_time_layer, s_font_time);
  text_layer_set_text_color(s_time_layer, settings.TextColor);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text(s_time_layer, "--:--");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_time_layer));
}
static void create_seconds_layer(void) {
  s_seconds_layer = text_layer_create(
      GRect(0, city_y, bounds.size.w, 30));
  text_layer_set_font(s_seconds_layer, s_font_small);
  text_layer_set_text_color(s_seconds_layer, settings.TextColor);
  text_layer_set_background_color(s_seconds_layer, GColorClear);
  text_layer_set_text_alignment(s_seconds_layer, GTextAlignmentRight);
  text_layer_set_text(s_seconds_layer, "--");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_seconds_layer));
}
static void create_rise_layer(void) {
  s_rise_layer = text_layer_create(
      GRect(0, rise_set_y, bounds.size.w, 30));
  text_layer_set_font(s_rise_layer, s_font_small);
  text_layer_set_text_color(s_rise_layer, settings.TextColor);
  text_layer_set_background_color(s_rise_layer, GColorClear);
  text_layer_set_text_alignment(s_rise_layer, GTextAlignmentLeft);
  text_layer_set_text(s_rise_layer, "---");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_rise_layer));
}
static void create_set_layer(void) {
  s_set_layer = text_layer_create(
      GRect(0, rise_set_y, bounds.size.w, 30));
  text_layer_set_font(s_set_layer, s_font_small);
  text_layer_set_text_color(s_set_layer, settings.TextColor);
  text_layer_set_background_color(s_set_layer, GColorClear);
  text_layer_set_text_alignment(s_set_layer, GTextAlignmentRight);
  text_layer_set_text(s_set_layer, "---");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_set_layer));
}
static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_time_buffer[16];
  static char s_date_buffer[16];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ?
                                                    "%H:%M" : "%I:%M", tick_time);
  strftime(s_date_buffer, sizeof(s_date_buffer), "%a %b %d", tick_time);
  text_layer_set_text(s_time_layer, s_time_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (show_seconds_now) {
    static char s_seconds_buffer[4];
    strftime(s_seconds_buffer, sizeof(s_seconds_buffer), "%S", tick_time);
    text_layer_set_text(s_seconds_layer, s_seconds_buffer);
  }
  if (tick_time->tm_min % 30 == 0 && tick_time->tm_sec == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, MESSAGE_KEY_REQUEST_WEATHER, 1);
    app_message_outbox_send();
  } else if (tick_time->tm_sec == 0) {
    update_time();
  }
}
static void hide_seconds_callback(void *data) {
  show_seconds_now = false;
  text_layer_set_text(s_seconds_layer, "--");
}
static void tap_handler(AccelAxisType axis, int32_t direction) {
  if (strncmp(settings.SecondsChoice, "tap", 3) != 0) return;
  show_seconds_now = true;
  layer_set_hidden(text_layer_get_layer(s_seconds_layer), false);
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  static char buf[4];
  strftime(buf, sizeof(buf), "%S", t);
  text_layer_set_text(s_seconds_layer, buf);

  if (s_seconds_timer) {
    app_timer_cancel(s_seconds_timer);
  }
  s_seconds_timer = app_timer_register(settings.SecondsLimit * 1000, hide_seconds_callback, NULL);
}


// Window handling
static void main_window_load(Window* window) {
  // load custom fonts once and keep handles globally
  s_font_medium = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DSEGFT_20));
  s_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DSEGFT_12));
  s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DSEG_42));
  s_font_weather = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DSEG_WEATHER_24));

  window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);
  set_positions();
  create_time_layer();
  create_seconds_layer();
  create_date_layer();
  create_hl_layer();
  create_rise_layer();
  create_set_layer();
  create_weather_layer();
  create_conditons_layer();
  create_humid_layer();
  create_city_layer();
  create_battery_layer();
  create_batt_percent_layer();
  create_steps_layer();
  create_hr_layer();
  update_time();
  prv_update_display();
}
static void main_window_unload(Window* window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_seconds_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_conditions_layer);
  text_layer_destroy(s_humid_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_hl_layer);
  text_layer_destroy(s_rise_layer);
  text_layer_destroy(s_set_layer);
  text_layer_destroy(s_city_layer);
  text_layer_destroy(s_steps_layer);
  text_layer_destroy(s_hr_layer);
  text_layer_destroy(s_batt_percent_layer);
  layer_destroy(s_battery_layer);
  fonts_unload_custom_font(s_font_medium);
  fonts_unload_custom_font(s_font_small);
  fonts_unload_custom_font(s_font_time);
  fonts_unload_custom_font(s_font_weather);
}
static void create_main_window(void) {
  s_window = window_create();
  window_set_background_color(s_window, settings.BackgroundColor);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_window, true);
}


// Message Handling
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Weather and related data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
  Tuple *humid_tuple = dict_find(iterator, MESSAGE_KEY_HUMIDITY);
  Tuple *precip_tuple = dict_find(iterator, MESSAGE_KEY_PRECIP);
  Tuple *high_tuple = dict_find(iterator, MESSAGE_KEY_HIGH);
  Tuple *low_tuple = dict_find(iterator, MESSAGE_KEY_LOW);
  Tuple *sunrise_tuple = dict_find(iterator, MESSAGE_KEY_SUNRISE);
  Tuple *sunset_tuple = dict_find(iterator, MESSAGE_KEY_SUNSET);
  Tuple *city_tuple = dict_find(iterator, MESSAGE_KEY_CITY);
  static char weather_layer_buffer[16];
  static char condition_layer_buffer[8];
  static char humid_layer_buffer[8];
  static char hl_layer_buffer[16];
  static char rise_layer_buffer[12];
  static char set_layer_buffer[12];
  static char city_layer_buffer[12];
 
  if (temp_tuple && conditions_tuple) {
    static char temperature_buffer[16];
    static char conditions_buffer[8];
    static char humid_buffer[8];
    static char hl_buffer[16];
    static char rise_buffer[12];
    static char set_buffer[12];
    static char city_buffer[12];
    int temp_value = temp_tuple->value->int32;
    int high_value = high_tuple ? high_tuple->value->int32 : 0;
    int low_value = low_tuple ? low_tuple->value->int32 : 0;
    const char *sunrise_str = sunrise_tuple ? sunrise_tuple->value->cstring : "";
    const char *sunset_str = sunset_tuple ? sunset_tuple->value->cstring : "";

    // Convert to Fahrenheit if setting is enabled
    if (strncmp(settings.TemperatureUnit, "true", 4) == 0) {
      temp_value = (temp_value * 9 / 5) + 32;
      high_value = (high_value * 9 / 5) + 32;
      low_value = (low_value * 9 / 5) + 32;
    }
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", temp_value);
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
    
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    snprintf(condition_layer_buffer, sizeof(condition_layer_buffer), "%s", conditions_buffer);

    if (strncmp(settings.PercentChoice, "precip", 4) == 0)
    {
      snprintf(humid_buffer, sizeof(humid_buffer), "%d%%", (int)precip_tuple->value->int32);
    } else if (strncmp(settings.PercentChoice, "humid", 5) == 0){
      snprintf(humid_buffer, sizeof(humid_buffer), "%d%%", (int)humid_tuple->value->int32);
    }
    snprintf(humid_layer_buffer, sizeof(humid_layer_buffer), "%s", humid_buffer);

    snprintf(hl_buffer, sizeof(hl_buffer), "%d°/%d°", high_value, low_value);
    snprintf(hl_layer_buffer, sizeof(hl_layer_buffer), "%s", hl_buffer);

    snprintf(rise_buffer, sizeof(rise_buffer), "%s", sunrise_str);
    snprintf(rise_layer_buffer, sizeof(rise_layer_buffer), "%s", rise_buffer);
    snprintf(set_buffer, sizeof(set_buffer), "%s", sunset_str);
    snprintf(set_layer_buffer, sizeof(set_layer_buffer), "%s", set_buffer);
    if (city_tuple) {
      snprintf(city_buffer, sizeof(city_buffer), "%s", city_tuple->value->cstring);
      snprintf(city_layer_buffer, sizeof(city_layer_buffer), "%s", city_buffer);
      text_layer_set_text(s_city_layer, city_layer_buffer);
    }

    text_layer_set_text(s_weather_layer, weather_layer_buffer);
    text_layer_set_text(s_conditions_layer, condition_layer_buffer);
    text_layer_set_text(s_humid_layer, humid_layer_buffer);
    text_layer_set_text(s_hl_layer, hl_layer_buffer);
    text_layer_set_text(s_rise_layer, rise_layer_buffer);
    text_layer_set_text(s_set_layer, set_layer_buffer);
  }
  // Check for Clay settings
  Tuple *bg_color_t = dict_find(iterator, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }
  Tuple *text_color_t = dict_find(iterator, MESSAGE_KEY_TextColor);
  if (text_color_t) {
    settings.TextColor = GColorFromHEX(text_color_t->value->int32);
  }
  Tuple *temp_unit_t = dict_find(iterator, MESSAGE_KEY_TemperatureUnit);
  if (temp_unit_t) {
    strcpy(settings.TemperatureUnit, temp_unit_t->value->cstring);
  }
  Tuple *percent_choice_t = dict_find(iterator, MESSAGE_KEY_PercentChoice);
  if (percent_choice_t){
    strcpy(settings.PercentChoice, percent_choice_t->value->cstring);
  }
  Tuple *show_date_t = dict_find(iterator, MESSAGE_KEY_ShowDate);
  if (show_date_t) {
    settings.ShowDate = show_date_t->value->int32 == 1;
  }
  Tuple *show_city_t = dict_find(iterator, MESSAGE_KEY_ShowCity);
  if (show_city_t) {
    settings.ShowCity = show_city_t->value->int32 == 1;
  }
  Tuple *show_health_t = dict_find(iterator, MESSAGE_KEY_ShowHealth);
  if (show_health_t){
    settings.ShowHealth = show_health_t->value->int32 == 1;
  }
  Tuple *show_conditions_t = dict_find(iterator, MESSAGE_KEY_ShowConditions);
  if (show_conditions_t){
    settings.ShowConditions = show_conditions_t->value->int32 == 1;
  }
  Tuple *show_high_low_t = dict_find(iterator, MESSAGE_KEY_ShowHighLow);
  if (show_high_low_t){
    settings.ShowHighLow = show_high_low_t->value->int32 == 1;
  }
  Tuple *show_temp_t = dict_find(iterator, MESSAGE_KEY_ShowTemp);
  if (show_temp_t){
    settings.ShowTemp = show_temp_t->value->int32 == 1;
  }
  Tuple *show_rise_set_t = dict_find(iterator, MESSAGE_KEY_ShowRiseSet);
  if (show_rise_set_t){
    settings.ShowRiseSet = show_rise_set_t->value->int32 == 1;
  }
  Tuple *seconds_choice_t = dict_find(iterator, MESSAGE_KEY_SecondsChoice);
  if (seconds_choice_t) {
    strcpy(settings.SecondsChoice, seconds_choice_t->value->cstring);
  }
  Tuple *seconds_limit_t = dict_find(iterator, MESSAGE_KEY_SecondsLimit);
  if (seconds_limit_t) {
    settings.SecondsLimit = (int)seconds_limit_t->value->int32;
  }
  Tuple *battery_choice_t = dict_find(iterator, MESSAGE_KEY_BatteryChoice);
  if (battery_choice_t){
    strcpy(settings.BatteryChoice, battery_choice_t->value->cstring);
  }
  // Save and apply if any settings were changed
  if (bg_color_t || text_color_t || temp_unit_t || show_date_t || show_city_t || seconds_choice_t || seconds_limit_t) {
    prv_save_settings();
    prv_update_display();
    // Refetch weather if the temperature unit changed so the display updates
    if (temp_unit_t) {
      DictionaryIterator *iter;
      app_message_outbox_begin(&iter);
      dict_write_uint8(iter, MESSAGE_KEY_REQUEST_WEATHER, 1);
      app_message_outbox_send();
    }
  }
}
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


// Default funcs
static void init(void) {
  prv_load_settings();
  create_main_window();
  tick_timer_service_subscribe(strncmp(settings.SecondsChoice, "hide", 4) == 0 ? MINUTE_UNIT: SECOND_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
  if (strncmp(settings.BatteryChoice, "hide", 4) != 0)
  {
    battery_state_service_subscribe(battery_callback);
    battery_callback(battery_state_service_peek());
  }
  if(!health_service_events_subscribe(health_handler, NULL)) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    health_available = false;
  } else{
    APP_LOG(APP_LOG_LEVEL_INFO, "Health available!");
    health_available = true;
  }
  // Register AppMessage callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  const int inbox_size = 256;
  const int outbox_size = 256;
  app_message_open(inbox_size, outbox_size);
}
static void deinit(void) {
    accel_tap_service_unsubscribe();
    if (s_seconds_timer) app_timer_cancel(s_seconds_timer);
  window_destroy(s_window);
}
int main(void) {
  init();
  app_event_loop();
  deinit();
}
