#include <pebble.h>
#define SETTINGS_KEY 1

// App Settings for watchface
typedef struct ClaySettings {
  GColor BackgroundColor;
  GColor TextColor;
  bool TemperatureUnit; // false = Celsius, true = Fahrenheit
  bool ShowDate;
} ClaySettings;
static ClaySettings settings;


// Global vars
static Window* s_window;
Layer* window_layer;
static TextLayer* s_time_layer;
static TextLayer* s_weather_layer;
static TextLayer* s_conditions_layer;
static TextLayer* s_date_layer;
static TextLayer* s_hl_layer;
static TextLayer* s_rise_set_layer;
static TextLayer* s_city_layer;

// fonts used throughout the app
static GFont s_font_time;
static GFont s_font_medium;
static GFont s_font_small;
static GFont s_font_weather;

static GRect bounds;
int time_y;
int date_y;
int hl_y;
int weather_y;
int condition_y;
int rise_set_y;
int city_y;


// Settings handling
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.TextColor = GColorWhite;
  settings.TemperatureUnit = false;
  settings.ShowDate = true;
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

  layer_set_hidden(text_layer_get_layer(s_date_layer), !settings.ShowDate);

  //layer_mark_dirty(s_battery_layer);
}


// Positioning
static void set_positions(void) {
  bounds = layer_get_bounds(window_layer);
  int time_height = 42;
  int date_height = 20;
  time_y = (bounds.size.h / 2) - 35;
  hl_y = 24;
  weather_y = 15;
  condition_y = -5;
  rise_set_y = bounds.size.h - 14;
  date_y = rise_set_y - date_height - 5;
  city_y = date_y - date_height - 5;
}


// City layer
static void create_city_layer(void) {
  s_city_layer = text_layer_create(
      GRect(0, city_y, bounds.size.w, 30));
  text_layer_set_font(s_city_layer, s_font_medium);
  text_layer_set_text_color(s_city_layer, settings.TextColor);
  text_layer_set_background_color(s_city_layer, GColorClear);
  text_layer_set_text_alignment(s_city_layer, GTextAlignmentLeft);
  text_layer_set_text(s_city_layer, "City");
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
    GRect(-24, condition_y, bounds.size.w, 30));
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
  text_layer_set_text(s_hl_layer, "---");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_hl_layer));
}


// Time handling
static void create_date_layer(void) {
  s_date_layer = text_layer_create(
      GRect(0, date_y, bounds.size.w, 30));
  text_layer_set_font(s_date_layer, s_font_medium);
  text_layer_set_text_color(s_date_layer, settings.TextColor);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date_layer, "--- --");
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
static void create_rise_set_layer(void) {
  s_rise_set_layer = text_layer_create(
      GRect(0, rise_set_y, bounds.size.w, 30));
  text_layer_set_font(s_rise_set_layer, s_font_small);
  text_layer_set_text_color(s_rise_set_layer, settings.TextColor);
  text_layer_set_background_color(s_rise_set_layer, GColorClear);
  text_layer_set_text_alignment(s_rise_set_layer, GTextAlignmentCenter);
  text_layer_set_text(s_rise_set_layer, "---");
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_rise_set_layer));
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
  update_time();
  if (tick_time->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, MESSAGE_KEY_REQUEST_WEATHER, 1);
    app_message_outbox_send();
  }
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
  create_date_layer();
  create_hl_layer();
  create_rise_set_layer();
  create_weather_layer();
  create_conditons_layer();
  create_city_layer();
  update_time();
  
}
static void main_window_unload(Window* window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_conditions_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_hl_layer);
  text_layer_destroy(s_rise_set_layer);
  text_layer_destroy(s_city_layer);

  // unload fonts
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
  Tuple *high_tuple = dict_find(iterator, MESSAGE_KEY_HIGH);
  Tuple *low_tuple = dict_find(iterator, MESSAGE_KEY_LOW);
  Tuple *sunrise_tuple = dict_find(iterator, MESSAGE_KEY_SUNRISE);
  Tuple *sunset_tuple = dict_find(iterator, MESSAGE_KEY_SUNSET);
  Tuple *city_tuple = dict_find(iterator, MESSAGE_KEY_CITY);
  static char weather_layer_buffer[16];
  static char condition_layer_buffer[8];
  static char hl_layer_buffer[16];
  static char rise_set_layer_buffer[48];
  static char city_layer_buffer[32];
 
  if (temp_tuple && conditions_tuple) {
    static char temperature_buffer[16];
    static char conditions_buffer[8];
    static char hl_buffer[16];
    static char rise_set_buffer[48];
    static char city_buffer[32];
    int temp_value = temp_tuple->value->int32;
    int high_value = high_tuple ? high_tuple->value->int32 : 0;
    int low_value = low_tuple ? low_tuple->value->int32 : 0;
    const char *sunrise_str = sunrise_tuple ? sunrise_tuple->value->cstring : "";
    const char *sunset_str = sunset_tuple ? sunset_tuple->value->cstring : "";

    // Convert to Fahrenheit if setting is enabled
    if (settings.TemperatureUnit) {
      temp_value = (temp_value * 9 / 5) + 32;
      high_value = (high_value * 9 / 5) + 32;
      low_value = (low_value * 9 / 5) + 32;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°F", temp_value);
    } else {
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", temp_value);
    }
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);

    snprintf(hl_buffer, sizeof(hl_buffer), "%d°/%d°", high_value, low_value);
    snprintf(hl_layer_buffer, sizeof(hl_layer_buffer), "%s", hl_buffer);
    
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    snprintf(condition_layer_buffer, sizeof(condition_layer_buffer), "%s", conditions_buffer);

    snprintf(rise_set_buffer, sizeof(rise_set_buffer), "%s                        %s", sunrise_str, sunset_str);
    snprintf(rise_set_layer_buffer, sizeof(rise_set_layer_buffer), "%s", rise_set_buffer);
    if (city_tuple) {
      snprintf(city_buffer, sizeof(city_buffer), "%s", city_tuple->value->cstring);
      snprintf(city_layer_buffer, sizeof(city_layer_buffer), "%s", city_buffer);
      text_layer_set_text(s_city_layer, city_layer_buffer);
    }

    text_layer_set_text(s_weather_layer, weather_layer_buffer);
    text_layer_set_text(s_hl_layer, hl_layer_buffer);
    text_layer_set_text(s_conditions_layer, condition_layer_buffer);
    text_layer_set_text(s_rise_set_layer, rise_set_layer_buffer);
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
    settings.TemperatureUnit = temp_unit_t->value->int32 == 1;
  }
  Tuple *show_date_t = dict_find(iterator, MESSAGE_KEY_ShowDate);
  if (show_date_t) {
    settings.ShowDate = show_date_t->value->int32 == 1;
  }
  // Save and apply if any settings were changed
  if (bg_color_t || text_color_t || temp_unit_t || show_date_t) {
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
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  // Register AppMessage callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  const int inbox_size = 256;
  const int outbox_size = 256;
  app_message_open(inbox_size, outbox_size);
  //update_time();
}
static void deinit(void) {
  window_destroy(s_window);
}
int main(void) {
  init();
  app_event_loop();
  deinit();
}
