#include <pebble.h>
#include "common.h"

static Window *s_window;
static TextLayer *s_text_layer;

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Construct a message to send
  AppWorkerMessage message = {
    .data0 = 3,
    .data1 = 4
  };

  // Send the data to the background app
  app_worker_send_message(SOURCE_FOREGROUND, &message);

  text_layer_set_text(s_text_layer, "Press SELECT");
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Check to see if the worker is currently active
  bool running = app_worker_is_running();

  // Toggle running state
  AppWorkerResult result;
  if(running) {
    result = app_worker_kill();

    if(result == APP_WORKER_RESULT_SUCCESS) {
      text_layer_set_text(s_text_layer, "Worker stopped!");
    } else {
      text_layer_set_text(s_text_layer, "Error killing worker!");
    }
  } else {
    result = app_worker_launch();

    if(result == APP_WORKER_RESULT_SUCCESS) {
      text_layer_set_text(s_text_layer, "Worker launched!\nPress DOWN");
    } else {
      text_layer_set_text(s_text_layer, "Error launching worker!");
    }
  }
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_worker_message_handler(uint16_t type, 
                                    AppWorkerMessage *message) {
  if(type == SOURCE_BACKGROUND) {
    // Get the data, only if it was sent from the background
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Foreground received: %d, %d", message->data0, 
              message->data1);
  }
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Sample text layer
  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 60));
  text_layer_set_text(s_text_layer, "Press SELECT");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  // Subscribe to get AppWorkerMessages
  app_worker_message_subscribe(prv_worker_message_handler);
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, true);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
