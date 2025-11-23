/*
 * Copyright 2024 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "events.h"

#include "debug/setup.h"

#include "system/logging.h"
#include "system/passert.h"
#include "system/reset.h"

#include "kernel/pbl_malloc.h"
#include "os/tick.h"

#include <stddef.h>

#include "services/normal/app_outbox_service.h"

// Helper macro to get address of packed struct member safely
#if defined(MICRO_FAMILY_ESP32C3)
// For RISC-V, use pointer arithmetic to avoid warning
// Helper function to get address of packed struct member
static inline void **get_packed_member_addr_impl(void *ptr, size_t offset) {
  return (void **)((char *)ptr + offset);
}
// Macros for specific struct types (to avoid typeof)
#define GET_PACKED_MEMBER_ADDR_SYS_NOTIFICATION(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleSysNotificationEvent, member))
#define GET_PACKED_MEMBER_ADDR_BLOB_DB(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleBlobDBEvent, member))
#define GET_PACKED_MEMBER_ADDR_BT_PAIR(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleBluetoothPairEvent, member))
#define GET_PACKED_MEMBER_ADDR_LAUNCH_APP(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleLaunchAppEvent, member))
#define GET_PACKED_MEMBER_ADDR_VOICE_SERVICE(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleVoiceServiceEvent, member))
#define GET_PACKED_MEMBER_ADDR_REMINDER(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleReminderEvent, member))
#define GET_PACKED_MEMBER_ADDR_BLE_GATT_CLIENT_SERVICE(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleBLEGATTClientServiceEvent, member))
#define GET_PACKED_MEMBER_ADDR_HRM(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleHRMEvent, member))
#define GET_PACKED_MEMBER_ADDR_APP_GLANCE(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleAppGlanceEvent, member))
#define GET_PACKED_MEMBER_ADDR_TIMELINE_PEEK(ptr, member) \
  get_packed_member_addr_impl((ptr), offsetof(PebbleTimelinePeekEvent, member))
#else
// For other platforms, direct address is fine
#define GET_PACKED_MEMBER_ADDR(ptr, member) &((ptr)->member)
#endif
#include "syscall/syscall.h"

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static QueueHandle_t s_kernel_event_queue = NULL;
static QueueHandle_t s_from_app_event_queue = NULL;
static QueueHandle_t s_from_worker_event_queue = NULL;

// The following conventions insure that the s_from_kernel_event_queue queue will always have sufficient space and that
// KernelMain will never deadlock trying to send an event to itself:
// 1.) KernelMain must never enqueue more than MAX_FROM_KERNEL_MAIN_EVENTS events to itself while processing another
//     event.
// 2.) The ONLY task that posts events to s_from_kernel_event_queue is the KernelMain task.
// 3.) Whenever KernelMain wants to post an event to itself, it MUST use this queue.
// 4.) The KernelMain task will always service this queue first, before servicing the kernel or from_app queues.
static QueueHandle_t s_from_kernel_event_queue = NULL;

// This queue set contains the s_kernel_event_queue, s_from_app_event_queue, and s_from_worker_event_queue queues
static QueueSetHandle_t s_system_event_queue_set = NULL;

static const int MAX_KERNEL_EVENTS = 32;
static const int MAX_FROM_APP_EVENTS = 10;
static const int MAX_FROM_WORKER_EVENTS = 5;
static const int MAX_FROM_KERNEL_MAIN_EVENTS = 14;

uint32_t s_current_event;

#define EVENT_DEBUG 0

#if EVENT_DEBUG
static void prv_queue_dump(QueueHandle_t queue) {
  PebbleEvent event;
  PBL_LOG(LOG_LEVEL_DEBUG, "Dumping queue:");
  while (xQueueReceive(queue, &event, 0) == pdTRUE) {
    PBL_LOG(LOG_LEVEL_DEBUG, "Event type: %u", event.type);
  }
  for(;;);
}
#endif

void events_init(void) {
  PBL_ASSERTN(s_system_event_queue_set == NULL);

  // This assert is to make sure we don't accidentally bloat our PebbleEvent unecessarily. If you hit this
  // assert and you have a good reason for making the event bigger, feel free to relax the restriction.
  //PBL_LOG(LOG_LEVEL_DEBUG, "PebbleEvent size is %u", sizeof(PebbleEvent));
  // FIXME:
  _Static_assert(sizeof(PebbleEvent) <= 12,
                 "You made the PebbleEvent bigger! It should be no more than 12");


  s_system_event_queue_set = xQueueCreateSet(MAX_KERNEL_EVENTS + MAX_FROM_APP_EVENTS);

  s_kernel_event_queue = xQueueCreate(MAX_KERNEL_EVENTS, sizeof(PebbleEvent));
  PBL_ASSERTN(s_kernel_event_queue != NULL);

  s_from_app_event_queue = xQueueCreate(MAX_FROM_APP_EVENTS , sizeof(PebbleEvent));
  PBL_ASSERTN(s_from_app_event_queue != NULL);

  s_from_worker_event_queue = xQueueCreate(MAX_FROM_WORKER_EVENTS , sizeof(PebbleEvent));
  PBL_ASSERTN(s_from_worker_event_queue != NULL);

  s_from_kernel_event_queue = xQueueCreate(MAX_FROM_KERNEL_MAIN_EVENTS , sizeof(PebbleEvent));
  PBL_ASSERTN(s_from_kernel_event_queue != NULL);

  xQueueAddToSet(s_kernel_event_queue, s_system_event_queue_set);
  xQueueAddToSet(s_from_app_event_queue, s_system_event_queue_set);
  xQueueAddToSet(s_from_worker_event_queue, s_system_event_queue_set);
}

//! Get the from_process queue for a specific task
QueueHandle_t event_get_to_kernel_queue(PebbleTask task) {
  if (task == PebbleTask_App) {
    return s_from_app_event_queue;
  } else if (task == PebbleTask_Worker) {
    return s_from_worker_event_queue;
  } else if (task == PebbleTask_KernelMain) {
    return s_from_kernel_event_queue;
  } else if ((task == PebbleTask_NewTimers) || (task == PebbleTask_KernelBackground)) {
    return s_kernel_event_queue;
  } else {
    WTF;
    return NULL;
  }
}


//! Decode a bit more information out about an event and pack it into a uint32_t
static uint32_t prv_get_fancy_type_from_event(const PebbleEvent *event) {
  if (event->type == PEBBLE_CALLBACK_EVENT) {
    return (uint32_t) event->callback.callback;
  }
  return event->type;
}

static void prv_log_event_put_failure(const char *queue_name, uintptr_t saved_lr, const PebbleEvent *event) {
  PBL_LOG(LOG_LEVEL_ERROR, "Error, %s queue full. Type %u", queue_name, event->type);

  RebootReason reason = {
    .code = RebootReasonCode_EventQueueFull,
    .event_queue = {
      .push_lr = saved_lr,
      .current_event = s_current_event,
      .dropped_event = prv_get_fancy_type_from_event(event)
    }
  };
  reboot_reason_set(&reason);
}

static bool prv_event_put_isr(QueueHandle_t queue, const char* queue_type, uintptr_t saved_lr,
                                  PebbleEvent* event) {
  PBL_ASSERTN(queue);

  portBASE_TYPE should_context_switch = pdFALSE;
  if (!xQueueSendToBackFromISR(queue, event, &should_context_switch)) {
    prv_log_event_put_failure(queue_type, saved_lr, event);

#ifdef NO_WATCHDOG
    enable_mcu_debugging();
    while (1);
#endif

    reset_due_to_software_failure();
  }

  return should_context_switch;
}

static bool prv_try_event_put(QueueHandle_t queue, PebbleEvent *event) {
  PBL_ASSERTN(queue);
  return (xQueueSendToBack(queue, event, milliseconds_to_ticks(3000)) == pdTRUE);
}

static void prv_event_put(QueueHandle_t queue,
                          const char* queue_type,
                          uintptr_t saved_lr,
                          PebbleEvent* event) {
  PBL_ASSERTN(queue);

  if (!xQueueSendToBack(queue, event, milliseconds_to_ticks(3000))) {
    // We waited a reasonable amount of time here before failing. We don't want to wait too long because
    // if the queue really is stuck we'll just get a watchdog reset, which will be harder to debug than
    // just dieing here. However, we want to wait a non-zero amount of time to provide for a little bit
    // of backup to occur before killing ourselves.

    prv_log_event_put_failure(queue_type, saved_lr, event);

#if EVENT_DEBUG
    prv_queue_dump(queue);
#endif

    reset_due_to_software_failure();
  }
}

void event_deinit(PebbleEvent* event) {
  void **buffer = event_get_buffer(event);
  if (buffer && *buffer) {
    kernel_free(*buffer);
    *buffer = NULL;
  }
}

void event_put(PebbleEvent* event) {
  uintptr_t saved_lr;
#if defined(MICRO_FAMILY_ESP32C3)
  // For RISC-V, use __builtin_return_address
  saved_lr = (uintptr_t)__builtin_return_address(0);
#else
  // For ARM, read the link register directly
  register uintptr_t lr __asm("lr");
  saved_lr = lr;
#endif
  // If we are posting from the KernelMain task, use the dedicated s_from_kernel_event_queue queue for that
  // See comments above where s_from_kernel_event_queue is declared.
  if (pebble_task_get_current() == PebbleTask_KernelMain) {
    return prv_event_put(s_from_kernel_event_queue, "from_kernel", saved_lr, event);
  } else {
    return prv_event_put(s_kernel_event_queue, "kernel", saved_lr, event);
  }
}

bool event_put_isr(PebbleEvent* event) {
  uintptr_t saved_lr;
#if defined(MICRO_FAMILY_ESP32C3)
  // For RISC-V, use __builtin_return_address
  saved_lr = (uintptr_t)__builtin_return_address(0);
#else
  // For ARM, read the link register directly
  register uintptr_t lr __asm("lr");
  saved_lr = lr;
#endif

  return prv_event_put_isr(s_kernel_event_queue, "kernel", saved_lr, event);
}

void event_put_from_process(PebbleTask task, PebbleEvent* event) {
  uintptr_t saved_lr;
#if defined(MICRO_FAMILY_ESP32C3)
  // For RISC-V, use __builtin_return_address
  saved_lr = (uintptr_t)__builtin_return_address(0);
#else
  // For ARM, read the link register directly
  register uintptr_t lr __asm("lr");
  saved_lr = lr;
#endif

  QueueHandle_t queue = event_get_to_kernel_queue(task);
  prv_event_put(queue, "from app", saved_lr, event);
}

bool event_try_put_from_process(PebbleTask task, PebbleEvent* event) {
  QueueHandle_t queue = event_get_to_kernel_queue(task);
  return prv_try_event_put(queue, event);
}

bool event_take_timeout(PebbleEvent* event, int timeout_ms) {
  PBL_ASSERTN(s_system_event_queue_set);

  s_current_event = 0;

  // We must prioritize the from_kernel queue and always empty that first in order to avoid deadlocks in
  // KernelMain. See comments at top of file where s_from_kernel_event_queue is declared.

  // Check the from_kernel queue first to see if we posted any events to ourself.
  portBASE_TYPE result = xQueueReceive(s_from_kernel_event_queue, event, 0);
  if (result) {
    s_current_event = prv_get_fancy_type_from_event(event);
    return true;
  }

  // Wait for either the from_app, from_worker, or kernel queue to be ready.
  QueueSetMemberHandle_t activated_queue = xQueueSelectFromSet(s_system_event_queue_set,
                                                              milliseconds_to_ticks(timeout_ms));
  if (!activated_queue) {
    return false;
  }

  // Always service the kernel queue first. This prevents a misbehaving app from starving us.
  // If we're a little lazy servicing the app, the app will just block itself when the queue gets full.
  if (xQueueReceive(s_kernel_event_queue, event, 0) == pdFALSE) {
    // Process the activated queue. This insures that events are handled in FIFO order from the app and worker
    // tasks. Note that sometimes the activated_queue can be the s_kernel_event_queue, even though
    // the above xQueueReceive returned no event
    if (activated_queue == s_from_app_event_queue || activated_queue == s_from_worker_event_queue) {
      result = xQueueReceive(activated_queue, event, 0);
    }
    if (!result) {
      result = xQueueReceive(s_from_app_event_queue, event, 0);
    }
    if (!result) {
      result = xQueueReceive(s_from_worker_event_queue, event, 0);
    }

    // If there was nothing in the queue, return false. We are misusing the queue set by pulling events out
    //  from the s_kernel_event_queue queue before it's activated so likely, the activated queue was
    //  s_kernel_event_queue.
    if (!result) {
      return false;
    }
  }

  s_current_event = prv_get_fancy_type_from_event(event);
  return true;
}

void **event_get_buffer(PebbleEvent *event) {
  switch (event->type) {
    case PEBBLE_SYS_NOTIFICATION_EVENT:
      if (event->sys_notification.type == NotificationActionResult) {
#if defined(MICRO_FAMILY_ESP32C3)
        return GET_PACKED_MEMBER_ADDR_SYS_NOTIFICATION(&event->sys_notification, action_result);
#else
        return (void **)GET_PACKED_MEMBER_ADDR(&event->sys_notification, action_result);
#endif
      } else if ((event->sys_notification.type == NotificationAdded) ||
                 (event->sys_notification.type == NotificationRemoved) ||
                 (event->sys_notification.type == NotificationActedUpon)) {
#if defined(MICRO_FAMILY_ESP32C3)
        return GET_PACKED_MEMBER_ADDR_SYS_NOTIFICATION(&event->sys_notification, notification_id);
#else
        return (void **)GET_PACKED_MEMBER_ADDR(&event->sys_notification, notification_id);
#endif
      }
      break;

    case PEBBLE_BLOBDB_EVENT:
#if defined(MICRO_FAMILY_ESP32C3)
      return GET_PACKED_MEMBER_ADDR_BLOB_DB(&event->blob_db, key);
#else
      return (void **)GET_PACKED_MEMBER_ADDR(&event->blob_db, key);
#endif

    case PEBBLE_BT_PAIRING_EVENT:
      if (event->bluetooth.pair.type ==
          PebbleBluetoothPairEventTypePairingUserConfirmation) {
#if defined(MICRO_FAMILY_ESP32C3)
        return GET_PACKED_MEMBER_ADDR_BT_PAIR(&event->bluetooth.pair, confirmation_info);
#else
        return (void **)GET_PACKED_MEMBER_ADDR(&event->bluetooth.pair, confirmation_info);
#endif
      }
      break;

    case PEBBLE_APP_LAUNCH_EVENT:
#if defined(MICRO_FAMILY_ESP32C3)
      return GET_PACKED_MEMBER_ADDR_LAUNCH_APP(&event->launch_app, data);
#else
      return (void **)GET_PACKED_MEMBER_ADDR(&event->launch_app, data);
#endif

    case PEBBLE_VOICE_SERVICE_EVENT:
#if defined(MICRO_FAMILY_ESP32C3)
      return GET_PACKED_MEMBER_ADDR_VOICE_SERVICE(&event->voice_service, data);
#else
      return (void **)GET_PACKED_MEMBER_ADDR(&event->voice_service, data);
#endif

    case PEBBLE_REMINDER_EVENT:
#if defined(MICRO_FAMILY_ESP32C3)
      return GET_PACKED_MEMBER_ADDR_REMINDER(&event->reminder, reminder_id);
#else
      return (void **)GET_PACKED_MEMBER_ADDR(&event->reminder, reminder_id);
#endif

    case PEBBLE_BLE_GATT_CLIENT_EVENT:
      if (event->bluetooth.le.gatt_client.subtype == PebbleBLEGATTClientEventTypeServiceChange) {
#if defined(MICRO_FAMILY_ESP32C3)
        return GET_PACKED_MEMBER_ADDR_BLE_GATT_CLIENT_SERVICE(&event->bluetooth.le.gatt_client_service, info);
#else
        return (void **)GET_PACKED_MEMBER_ADDR(&event->bluetooth.le.gatt_client_service, info);
#endif
      }
      break;

    case PEBBLE_HRM_EVENT:
      if (event->hrm.event_type == HRMEvent_Diagnostics) {
#if defined(MICRO_FAMILY_ESP32C3)
        return GET_PACKED_MEMBER_ADDR_HRM(&event->hrm, debug);
#else
        return (void **)GET_PACKED_MEMBER_ADDR(&event->hrm, debug);
#endif
      }
      break;

    case PEBBLE_APP_GLANCE_EVENT:
#if defined(MICRO_FAMILY_ESP32C3)
      return GET_PACKED_MEMBER_ADDR_APP_GLANCE(&event->app_glance, app_uuid);
#else
      return (void **)GET_PACKED_MEMBER_ADDR(&event->app_glance, app_uuid);
#endif

    case PEBBLE_TIMELINE_PEEK_EVENT:
#if defined(MICRO_FAMILY_ESP32C3)
      return GET_PACKED_MEMBER_ADDR_TIMELINE_PEEK(&event->timeline_peek, item_id);
#else
      return (void **)GET_PACKED_MEMBER_ADDR(&event->timeline_peek, item_id);
#endif

    default:
      break; // Nothing to do!
  }

  return NULL;
}

void event_cleanup(PebbleEvent* event) {
  event_deinit(event);

#ifndef RELEASE
  // Hopefully this will catch some use after free evil
  *event = (PebbleEvent){};
#endif
}

void event_reset_from_process_queue(PebbleTask task) {
  // Unfortunately, current versions of FreeRTOS don't really handle resetting a queue that's part
  // of a queue set all that well. See PBL-1817. We'll clean up the queue set manually.

  // Notice that we don't disable the scheduler or enter a critical section here. This is because
  // it is usually unsafe to do so when making other FreeRTOS calls that might cause  context switch
  // (see http://www.freertos.org/a00134.html). I think this is OK though - the worse that can
  // happen is that we end up with extra items in the s_system_event_queue_set that don't belong
  // there and event_take_timeout() is tolerant of that. Also see the discussion at
  // https://github.com/pebble/tintin/pull/2416#discussion_r16641981.

  // We want to remove all references to the queue we just reset, while keeping references to other
  // queues in check. This would be really annoying, but luckily we only have two other queues in
  // the set. Count the number of times the other queues exist in the queue set, clear the queue,
  // and then restore the original count.
  QueueHandle_t reset_queue, preserve_queue;
  if (task == PebbleTask_App) {
    reset_queue = s_from_app_event_queue;
    preserve_queue = s_from_worker_event_queue;
  } else if (task == PebbleTask_Worker) {
    reset_queue = s_from_worker_event_queue;
    preserve_queue = s_from_app_event_queue;
  } else {
    preserve_queue = reset_queue = NULL;
    WTF;
  }

  xQueueReset(s_system_event_queue_set);
  event_queue_cleanup_and_reset(reset_queue);

  int num_kernel_events_enqueued = uxQueueMessagesWaiting(s_kernel_event_queue);
  for (int i = 0; i < num_kernel_events_enqueued; ++i) {
    xQueueSend(s_system_event_queue_set, &s_kernel_event_queue, 0);
  }

  int num_client_task_events_enqueued = uxQueueMessagesWaiting(preserve_queue);
  for (int i = 0; i < num_client_task_events_enqueued; ++i) {
    xQueueSend(s_system_event_queue_set, &preserve_queue, 0);
  }
}


QueueHandle_t event_kernel_to_kernel_event_queue(void) {
  return s_from_kernel_event_queue;
}

BaseType_t event_queue_cleanup_and_reset(QueueHandle_t queue) {
  int num_events_in_queue = uxQueueMessagesWaiting(queue);
  PebbleEvent event;
  for (int i = 0; i < num_events_in_queue; ++i) {
    PBL_ASSERTN(xQueueReceive(queue, &event, 0) != pdFAIL);
    // event service does some book-keeping about events, notify it that we're dropping these.
    sys_event_service_cleanup(&event);
#if !RECOVERY_FW
    // app outbox service messages need to be cleaned up:
    app_outbox_service_cleanup_event(&event);
#endif
    // cleanup the event, free associated memory if applicable
    event_cleanup(&event);
  }

  return xQueueReset(queue);
}
