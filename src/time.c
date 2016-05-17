#include <pebble.h>

static TextLayer *s_time_layer;


static void draw_time_later(TextLayer *layer){
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(layer, GColorBlue);
  text_layer_set_text_color(layer, GColorBlack);
  text_layer_set_text(layer, "00:00");
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
}

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

static void build_text_layer(GRect bounds){
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  draw_time_later(s_time_layer);
}

static TextLayer* get_time_layer(){
	return s_time_layer;
}

