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

// Minimal glue layer for ESP-IDF integration
// This header provides compatibility shims between PebbleOS and ESP-IDF

#include <stdint.h>
#include <stdbool.h>

// ESP-IDF compatibility defines
#ifndef CONFIG_IDF_TARGET_ESP32C3
#define CONFIG_IDF_TARGET_ESP32C3
#endif

// ESP32-C3 uses legacy INTC, not CLIC
// Provide compatibility define for RVHAL_EXCM_LEVEL_CLIC
// This is used by spinlock.h even though ESP32-C3 doesn't support CLIC
// Note: RVHAL_EXCM_LEVEL is defined in riscv/rv_utils.h which is included
// before spinlock.h uses RVHAL_EXCM_LEVEL_CLIC, so we can reference it here
#ifndef RVHAL_EXCM_LEVEL_CLIC
// This will be defined after riscv/rv_utils.h is included
// We'll define it as a macro that expands to RVHAL_EXCM_LEVEL
// The actual value (4) is defined in riscv/rv_utils.h
#define RVHAL_EXCM_LEVEL_CLIC 4  // Same as RVHAL_EXCM_LEVEL for ESP32-C3
#endif

