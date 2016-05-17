#include <pebble.h>

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

static void draw_temp_later(TextLayer *layer){
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(layer, GColorRoseVale);
  text_layer_set_text_color(layer, GColorBlack);
  text_layer_set_text(layer, "...");
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
}

static void send_message_to_get_temp(){
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

static void update_temp(DictionaryIterator *iterator, void *context, TextLayer *layer){
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
  text_layer_set_text(layer, weather_layer_buffer);
}

static TextLayer* build_temp_layer(GRect bounds){
  TextLayer *layer = text_layer_create(
      GRect(0, 0, bounds.size.w, PBL_IF_ROUND_ELSE(58,52)));
  draw_temp_later(layer);
  return layer;
};
  

