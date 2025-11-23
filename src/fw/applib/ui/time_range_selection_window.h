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

#pragma once

#include "applib/ui/date_time_selection_window_private.h"
#include "applib/ui/selection_layer.h"
#include "applib/ui/text_layer.h"
#include "applib/ui/window.h"

#if defined(MICRO_FAMILY_ESP32C3)
#include "util/time/time.h"  // Use PebbleOS's custom time.h for ESP32-C3
#else
#include <time.h>
#endif

struct TimeRangeSelectionWindowData;

typedef void (*TimeRangeSelectionCompleteCallback)(struct TimeRangeSelectionWindowData *window,
                                                   void *context);

typedef struct TimeRangeSelectionWindowData {
  Window window;
  SelectionLayer from_selection_layer;
  SelectionLayer to_selection_layer;
  TextLayer from_text_layer;
  TextLayer to_text_layer;

  TimeRangeSelectionCompleteCallback complete_callback;
  void *callback_context;

  TimeData from;
  TimeData to;
  char buf[3];
} TimeRangeSelectionWindowData;


void time_range_selection_window_init(TimeRangeSelectionWindowData *time_range_selection_window,
    GColor color, TimeRangeSelectionCompleteCallback complete_callback, void *callback_context);

void time_range_selection_window_deinit(TimeRangeSelectionWindowData *time_range_selection_window);
