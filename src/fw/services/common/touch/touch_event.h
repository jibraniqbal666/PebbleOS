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

#include "applib/graphics/gtypes.h"

// For ESP32-C3, use PebbleOS's custom time.h instead of system time.h
#if defined(MICRO_FAMILY_ESP32C3)
#include "util/time/time.h"
#else
#include <time.h>
#endif
#include <stdint.h>

typedef uint8_t TouchIdx;
typedef int16_t TouchPressure;

//! Touch event type
typedef enum TouchEventType {
  TouchEvent_Touchdown,
  TouchEvent_Liftoff,
  TouchEvent_PositionUpdate,
  TouchEvent_PressureUpdate,
} TouchEventType;

//! Touch event
typedef struct TouchEvent {
  TouchEventType type;
  TouchIdx index;
  GPoint start_pos;
  GPoint diff_pos;
  int64_t start_time_ms;
  int64_t diff_time_ms;
  TouchPressure start_pressure;
  TouchPressure diff_pressure;
} TouchEvent;
