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

#include "tick_timer_service.h"
#include "tick_timer_service_private.h"

#include "event_service_client.h"
#include "process_management/app_manager.h"

#include "services/common/event_service.h"
#include "services/common/tick_timer.h"
#include "kernel/events.h"
#include "kernel/kernel_applib_state.h"

#include "process_state/app_state/app_state.h"
#include "process_state/worker_state/worker_state.h"

#include "syscall/syscall.h"
#include "system/passert.h"

// ----------------------------------------------------------------------------------------------------
static TickTimerServiceState* prv_get_state(PebbleTask task) {
  if (task == PebbleTask_Unknown) {
    task = pebble_task_get_current();
  }

  if (task == PebbleTask_App) {
    return app_state_get_tick_timer_service_state();
  } else if (task == PebbleTask_Worker) {
    return worker_state_get_tick_timer_service_state();
  } else if (task == PebbleTask_KernelMain) {
    return kernel_applib_get_tick_timer_service_state();
  } else {
    WTF;
  }
}


static void do_handle(PebbleEvent *e, void *context) {
  TickTimerServiceState *state = prv_get_state(PebbleTask_Unknown);
  PBL_ASSERTN(state->handler != NULL);

  TimeUnits units_changed = 0;
  struct tm currtime;
  // Copy packed struct member to local variable to avoid taking address warning
  time_t tick_time = e->clock_tick.tick_time;
  sys_localtime_r(&tick_time, &currtime);

  if (!state->first_tick) {
    if (state->last_time.tm_sec != currtime.tm_sec) {
      units_changed |= SECOND_UNIT;
    }
    if (state->last_time.tm_min != currtime.tm_min) {
      units_changed |= MINUTE_UNIT;
    }
    if (state->last_time.tm_hour != currtime.tm_hour) {
      units_changed |= HOUR_UNIT;
    }
    if (state->last_time.tm_mday != currtime.tm_mday) {
      units_changed |= DAY_UNIT;
    }
    if (state->last_time.tm_mon != currtime.tm_mon) {
      units_changed |= MONTH_UNIT;
    }
    if (state->last_time.tm_year != currtime.tm_year) {
      units_changed |= YEAR_UNIT;
    }
  }
  state->last_time = currtime;
  state->first_tick = false;

  if ((state->tick_units & units_changed) || (units_changed == 0)) {
    state->handler(&currtime, units_changed);
  }
}

void tick_timer_service_init(void) {
  TickTimerServiceState *state = prv_get_state(PebbleTask_Unknown);
  state->handler = NULL;
  event_service_init(PEBBLE_TICK_EVENT, &tick_timer_add_subscriber, &tick_timer_remove_subscriber);
}

void tick_timer_service_subscribe(TimeUnits tick_units, TickHandler handler) {
  TickTimerServiceState *state = prv_get_state(PebbleTask_Unknown);
  state->handler = handler;
  state->tick_units = tick_units;
  state->first_tick = true;
  event_service_client_subscribe(&state->tick_service_info);
  // TODO: make an effort to get this closer to the "actual" second tick
}

void tick_timer_service_unsubscribe(void) {
  TickTimerServiceState *state = prv_get_state(PebbleTask_Unknown);
  event_service_client_unsubscribe(&state->tick_service_info);
  state->handler = NULL;
}


void tick_timer_service_state_init(TickTimerServiceState *state) {
  *state = (TickTimerServiceState) {
    .tick_service_info = {
      .type = PEBBLE_TICK_EVENT,
      .handler = &do_handle,
    },
  };
}
