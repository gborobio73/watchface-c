#include <pebble.h>

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_temp_layer;


static void update_time(TextLayer *s_time_layer) {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);

  switch( tick_time->tm_min % 10){
    case 0: 
      text_layer_set_background_color(s_time_layer, GColorGreen);
      break;
    case 1: 
      text_layer_set_background_color(s_time_layer, GColorOrange);
      break;
    case 2: 
      text_layer_set_background_color(s_time_layer, GColorRed);
      break;
    case 3: 
      text_layer_set_background_color(s_time_layer, GColorCobaltBlue);
      break;
    case 4: 
      text_layer_set_background_color(s_time_layer, GColorYellow);
      break;
    case 5: 
      text_layer_set_background_color(s_time_layer, GColorGreen);
      break;
    case 6: 
      text_layer_set_background_color(s_time_layer, GColorMalachite);
      break;
    case 7: 
      text_layer_set_background_color(s_time_layer, GColorCyan);
      break;
    case 8: 
      text_layer_set_background_color(s_time_layer, GColorIndigo);
      break;
    case 9: 
      text_layer_set_background_color(s_time_layer, GColorJazzberryJam);
      break;
  }
}

static void update_date(TextLayer *s_date_layer){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  // Copy date into buffer from tm structure
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);
}

static void update_temp(TextLayer *s_temp_layer){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(s_time_layer);
  update_date(s_date_layer);
  update_temp(s_temp_layer);
}

static void draw_time_later(TextLayer *layer){
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(layer, GColorBlue);
  text_layer_set_text_color(layer, GColorBlack);
  text_layer_set_text(layer, "00:00");
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
}

static void build_text_layer(GRect bounds){
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  draw_time_later(s_time_layer);
  
}

static void draw_date_later(TextLayer *layer){
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(layer, GColorLiberty);
  text_layer_set_text_color(layer, GColorBlack);
  text_layer_set_text(layer, "date");
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
}

static void build_date_layer(GRect bounds){
  // Create the TextLayer with specific bounds
  s_date_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(108, 102), bounds.size.w, 50));

  draw_date_later(s_date_layer);
}

static void draw_temp_later(TextLayer *layer){
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(layer, GColorRoseVale);
  text_layer_set_text_color(layer, GColorBlack);
  text_layer_set_text(layer, "...");
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
}

static void build_temp_layer(GRect bounds){
  // Create the TextLayer with specific bounds
  s_temp_layer = text_layer_create(
      GRect(0, 0, bounds.size.w, PBL_IF_ROUND_ELSE(58,52)));
  draw_temp_later(s_temp_layer);
};
  

//temp stuff
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);

  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
  }

  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_temp_layer, weather_layer_buffer);

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

//-----------
static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  build_text_layer(bounds);
  build_date_layer(bounds);
  build_temp_layer(bounds);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_temp_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_temp_layer);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
