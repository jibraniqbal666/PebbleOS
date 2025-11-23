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

// ESP32-C3 portmacro.h wrapper
// ESP-IDF FreeRTOS's portmacro.h is actually at portable/riscv/include/freertos/portmacro.h
// The include paths from freertos wscript have portable/riscv/include
// So we need to include freertos/portmacro.h relative to that path

// Include asm compatibility header before ESP-IDF headers
// This ensures 'asm' keyword is recognized
// hal_esp32c3 directory is in include path, so include asm_compat.h directly
#include "asm_compat.h"

#include "freertos/portmacro.h"

// ESP-IDF FreeRTOS doesn't have portIN_CRITICAL(), which is a PebbleOS extension
// Provide a stub implementation for ESP32-C3
#if defined(MICRO_FAMILY_ESP32C3)
#include <stdbool.h>
// ESP-IDF doesn't expose a way to check if we're in a critical section
// For now, return false (not in critical section)
// This is a minimal implementation - a proper one would need to track critical section nesting
static inline bool vPortInCritical(void) {
    return false; // Stub - always return false for now
}
#define portIN_CRITICAL() vPortInCritical()

// Define portCANONICAL_REG_COUNT for RISC-V (ESP32-C3)
// RISC-V has 32 general purpose registers (x0-x31)
// For core dumps, we'll use a subset: x1-x15 (saved registers), x16-x31 (temporaries), 
// plus pc (program counter) and mstatus (machine status) = 32 total
// Using 16 to match ARM's count for compatibility
#ifndef portCANONICAL_REG_COUNT
#define portCANONICAL_REG_COUNT 16
#endif
#endif

