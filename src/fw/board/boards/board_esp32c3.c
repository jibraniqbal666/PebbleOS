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

#include "board/board.h"
#include "board/boards/board_esp32c3.h"
#include "drivers/uart.h"

// ESP32-C3 board implementation
// Minimal implementation for bootloader/firmware support
// Most functionality will be provided by ESP-IDF

// Note: This is a minimal stub implementation
// Full board support will be added as needed

// UART DEVICES
// ESP32-C3 uses UART0 for debug serial (matching bootloader)
#if defined(MICRO_FAMILY_ESP32C3)
// For ESP32-C3, we use stub UART driver
// Since UARTDevice is an opaque type (forward declared in uart.h),
// we create a minimal dummy structure that can be cast to UARTDevice*
// The stub UART driver doesn't actually access the structure contents
typedef struct {
  void *dummy;
} DummyUARTDevice;

static const DummyUARTDevice s_dbg_uart_device = {NULL};

UARTDevice * const DBG_UART = (UARTDevice *)&s_dbg_uart_device;
#endif
