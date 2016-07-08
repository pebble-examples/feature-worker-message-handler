#include <pebble_worker.h>
#include "../src/common.h"

static void prv_worker_message_handler(uint16_t type, 
                                    AppWorkerMessage *message) {
  if(type == SOURCE_FOREGROUND) {
    // Get the data, if it was sent from the foreground
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Background received: %d, %d", message->data0, 
          message->data1);
  }
}

static void prv_worker_init() {
  // Subscribe to get AppWorkerMessages
  app_worker_message_subscribe(prv_worker_message_handler);

  // Construct a message to send
  AppWorkerMessage message = {
    .data0 = 1,
    .data1 = 2
  };
  
  // Send the data to the foreground app
  app_worker_send_message(SOURCE_BACKGROUND, &message);
}

int main(void) {
  prv_worker_init();
	worker_event_loop();
}
