#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_temp_layer;


static void update_time() {
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

static void update_date(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  // Copy date into buffer from tm structure
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
}

static void build_text_layer(GRect bounds){
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorBlue);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
}

static void build_date_layer(GRect bounds){
  // Create the TextLayer with specific bounds
  s_date_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(108, 102), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_date_layer, GColorLiberty);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "date");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
}

static void build_temp_layer(GRect bounds){
  // Create the TextLayer with specific bounds
  s_temp_layer = text_layer_create(
      GRect(0, 0, bounds.size.w, PBL_IF_ROUND_ELSE(58,52)));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_temp_layer, GColorRoseVale);
  text_layer_set_text_color(s_temp_layer, GColorBlack);
  text_layer_set_text(s_temp_layer, "temp C");
  text_layer_set_font(s_temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
}

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
