#include <pebble.h>
#include <time.c>
#include <date.c>
#include <temp.c>

static Window *s_main_window;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
  send_message_to_get_temp();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  update_temp(iterator, context);

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

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  build_text_layer(bounds);
  build_date_layer(bounds);
  build_temp_layer(bounds);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(get_time_layer()));
  layer_add_child(window_layer, text_layer_get_layer(get_date_layer()));
  layer_add_child(window_layer, text_layer_get_layer(get_temp_layer()));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(get_time_layer());
  APP_LOG(APP_LOG_LEVEL_DEBUG, "time layer destroyed!");
  text_layer_destroy(get_date_layer());
  APP_LOG(APP_LOG_LEVEL_DEBUG, "date layer destroyed!");
  text_layer_destroy(get_temp_layer());
  APP_LOG(APP_LOG_LEVEL_DEBUG, "temp layer destroyed!");
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
