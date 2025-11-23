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

#include "board/board.h"

#define ESP32C3_COMPATIBLE
#include <mcu.h>

#include <stdint.h>
#include <stdbool.h>

// ESP32-C3 board configuration header
// Minimal implementation for bootloader support

#define BT_VENDOR_ID 0x0EEA
#define BT_VENDOR_NAME "Core Devices LLC"

// Minimal type definitions for ESP32-C3 (matching board_nrf5.h structure)
typedef struct {
  uint32_t gpio_pin;
} GpioteConfig;
typedef GpioteConfig ExtiConfig;

typedef struct {
  void *gpio;
  const uint32_t gpio_pin;
} InputConfig;

typedef struct {
  void *gpio;
  const uint32_t gpio_pin;
  bool active_high;
} OutputConfig;

typedef struct {
  const bool has_mic;
  // MicConfig mic_config; // TODO: Add when mic support is needed
} MicConfig;

// Minimal board config for ESP32-C3 (matching BoardConfig from board_nrf5.h)
static const BoardConfig BOARD_CONFIG = {
  .ambient_light_dark_threshold = 100,
  .ambient_k_delta_threshold = 30,
  .als_always_on = false,
  
  .backlight_on_percent = 25,
  .backlight_max_duty_cycle_percent = 67,
  
  .has_mic = false,
};

static const BoardConfigButton BOARD_CONFIG_BUTTON = {
  .active_high = false,
};

static const BoardConfigPower BOARD_CONFIG_POWER = {
  .low_power_threshold = 20,
  .battery_capacity_hours = 168, // ~7 days
};

