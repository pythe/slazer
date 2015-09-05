#include <pebble.h>
#include "slazer.h"

static Window *window;
static TextLayer *slazer_text_layer;
static TextLayer *on_text_layer;
static TextLayer *off_text_layer;
static TextLayer *back_text_layer;

static void activate_laser() {
  text_layer_set_text_color(slazer_text_layer, GColorRed);
  smartstrap_subscribe((SmartstrapHandlers) {
    .availability_did_change = availability_handler,
    .did_read = NULL,
    .did_write = NULL,
    .notified = NULL
  });
}

static void deactivate_laser() {
  text_layer_set_text_color(slazer_text_layer, GColorWhite);
  smartstrap_unsubscribe();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  activate_laser();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  deactivate_laser();
}

static void availability_handler(SmartstrapServiceId s, bool is_available) {

}

static void momentary_down_handler(ClickRecognizerRef recognizer, void *context) {
  activate_laser();
}

static void momentary_up_handler(ClickRecognizerRef recognizer, void *context) {
  deactivate_laser();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_raw_click_subscribe(BUTTON_ID_DOWN, momentary_down_handler,
      momentary_up_handler, NULL);
}

static TextLayer* create_text_layer(char* content, Layer* parent, int x, int y, int xSize, int ySize, uint8_t alignment) {
  TextLayer * textLayer = text_layer_create((GRect) { .origin = { x, y }, .size = { xSize, ySize } });
  text_layer_set_text(textLayer, content);
  text_layer_set_background_color(textLayer, GColorBlack);
  text_layer_set_text_color(textLayer, GColorWhite);
  text_layer_set_font(textLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(textLayer, alignment);
  layer_add_child(parent, text_layer_get_layer(textLayer));

  return textLayer;
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  slazer_text_layer = create_text_layer("SLAZER", window_layer, 0, 128, bounds.size.w - 5, 28, GTextAlignmentRight);
  on_text_layer     = create_text_layer("ON",     window_layer, 0,  67, bounds.size.w - 5, 28, GTextAlignmentRight);
  off_text_layer    = create_text_layer("OFF",    window_layer, 0,  10, bounds.size.w - 5, 28, GTextAlignmentRight);
  back_text_layer   = create_text_layer("BACK",   window_layer, 0,  10, 70,                28, GTextAlignmentLeft);
}

static void window_unload(Window *window) {
  text_layer_destroy(slazer_text_layer);
}

static void init(void) {
  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  deactivate_laser();
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
