#include <pebble.h>

static TextLayer *s_date_layer;

static void draw_date_later(){
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_date_layer, GColorLiberty);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "date");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
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

static void build_date_layer(GRect bounds){
  s_date_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(108, 102), bounds.size.w, 50));

  draw_date_later();
}

static TextLayer* get_date_layer(){
	return s_date_layer;
}
