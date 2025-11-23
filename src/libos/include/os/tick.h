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

#if defined(MICRO_FAMILY_ESP32C3)
// For ESP32-C3, include asm compatibility first, then ESP-IDF's portmacro.h
// hal_esp32c3 directory is in include path, so include asm_compat.h directly
#include "asm_compat.h"
#include "freertos/portmacro.h"
#else
#include "portmacro.h"
#endif

#include <stdint.h>

TickType_t milliseconds_to_ticks(uint32_t milliseconds);

uint32_t ticks_to_milliseconds(TickType_t ticks);
