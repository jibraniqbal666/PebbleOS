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

#include "date_time_selection_window_private.h"

#include "applib/ui/selection_layer.h"
#include "applib/ui/status_bar_layer.h"
#include "applib/ui/text_layer.h"
#include "applib/ui/window.h"
#include "services/common/clock.h"

#if defined(MICRO_FAMILY_ESP32C3)
#include "util/time/time.h"
#else
#include <time.h>
#endif

#define TIME_SELECTION_WINDOW_MAX_RANGE_LENGTH 64

struct TimeSelectionWindowData;

typedef void (*TimeSelectionCompleteCallback)(struct TimeSelectionWindowData *window, void *ctx);

typedef struct TimeSelectionWindowData {
  Window window;
  SelectionLayer selection_layer;
  TextLayer label_text_layer;
  TextLayer range_subtitle_text_layer;
  TextLayer range_text_layer;
  StatusBarLayer status_layer;
  TimeData time_data;

  TimeSelectionCompleteCallback complete_callback;
  void *callback_context;

  const char *range_text;
  int range_duration_m;

  //! Range buffer. Large enough for just the two time strings
  char range_buf[2 * TIME_STRING_TIME_LENGTH];
  //! Range subtitle buffer. Large enough for the range label
  char range_subtitle_buf[TIME_SELECTION_WINDOW_MAX_RANGE_LENGTH];
  char cell_buf[3];
} TimeSelectionWindowData;

typedef struct TimeSelectionWindowConfig {
  const char *label;
  GColor color;
  struct {
    bool update;
    const char *text;
    int duration_m;
    bool enabled;
  } range;
  struct {
    bool update;
    TimeSelectionCompleteCallback complete;
    void *context;
  } callback;
} TimeSelectionWindowConfig;

void time_selection_window_set_to_current_time(TimeSelectionWindowData *date_time_window);

void time_selection_window_configure(TimeSelectionWindowData *time_selection_window,
                                     const TimeSelectionWindowConfig *config);

void time_selection_window_init(TimeSelectionWindowData *time_selection_window,
                                const TimeSelectionWindowConfig *config);

void time_selection_window_deinit(TimeSelectionWindowData *time_selection_window);
