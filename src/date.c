#include <pebble.h>

static void draw_date_later(TextLayer *layer){
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(layer, GColorLiberty);
  text_layer_set_text_color(layer, GColorBlack);
  text_layer_set_text(layer, "date");
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
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

static TextLayer* build_date_layer(GRect bounds){
  TextLayer * layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(108, 102), bounds.size.w, 50));

  draw_date_later(layer);
  return layer;
}
