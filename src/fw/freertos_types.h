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

// ESP-IDF's FreeRTOS already defines QueueHandle_t, SemaphoreHandle_t, and TaskHandle_t
// But we still need to define TaskParameters_t and MemoryRegion_t for ESP32-C3
#if !defined(MICRO_FAMILY_ESP32C3)
typedef void * QueueHandle_t;

typedef QueueHandle_t SemaphoreHandle_t;

typedef void * TaskHandle_t;

typedef void (*TaskFunction_t)( void * );

typedef struct xTASK_PARAMETERS TaskParameters_t;

typedef struct xMEMORY_REGION MemoryRegion_t;
#else
// ESP32-C3: ESP-IDF defines QueueHandle_t, SemaphoreHandle_t, TaskHandle_t in FreeRTOS headers
// Include ESP-IDF's FreeRTOS headers directly to get the types
// Note: We can't include our FreeRTOS.h wrapper here as it would create circular dependencies
// Instead, include ESP-IDF's headers directly
#include "asm_compat.h"  // Needed for ESP-IDF headers
#include "freertos/FreeRTOSConfig.h"  // Include config first
#include "freertos/FreeRTOS.h"  // This provides QueueHandle_t, TaskHandle_t, etc.

// Forward declarations for PebbleOS-specific types that aren't in ESP-IDF
typedef struct xTASK_PARAMETERS TaskParameters_t;
typedef struct xMEMORY_REGION MemoryRegion_t;
// TaskFunction_t is defined by ESP-IDF FreeRTOS, so we don't need to redefine it
#endif
