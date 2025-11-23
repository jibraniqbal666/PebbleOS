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
#include <stddef.h>  // For NULL

// Include button_id.h to get NUM_BUTTONS definition
#include "drivers/button_id.h"

// ESP32-C3 board configuration header
// Minimal implementation for bootloader support

#define BT_VENDOR_ID 0x0EEA
#define BT_VENDOR_NAME "Core Devices LLC"

#define GPIO_Port_NULL (NULL)
#define GPIO_Pin_NULL ((uint16_t)-1)

// Minimal type definitions for ESP32-C3 (matching board_nrf5.h structure)
typedef struct {
  uint32_t gpio_pin;
} GpioteConfig;
typedef GpioteConfig ExtiConfig;

typedef struct {
  const char* const name;
  GpioteConfig gpiote;
  uint32_t pull; // Placeholder for pull config
} ButtonConfig;

typedef struct {
  const uint32_t gpio_pin;
} ButtonComConfig;

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
  int axes_offsets[3];
  bool axes_inverts[3];
  uint32_t shake_thresholds[2]; // AccelThreshold_Num = 2
  uint32_t double_tap_threshold;
  uint8_t tap_shock;
  uint8_t tap_quiet;
  uint8_t tap_dur;
} AccelConfig;

typedef struct {
  int axes_offsets[3];
  bool axes_inverts[3];
} MagConfig;

typedef struct {
  const bool has_mic;
  // MicConfig mic_config; // TODO: Add when mic support is needed
} MicConfig;

typedef enum {
  ActuatorOptions_Ctl = 1 << 0,
  ActuatorOptions_Pwm = 1 << 1,
  ActuatorOptions_HBridge = 1 << 3,
} ActuatorOptions;

typedef enum {
  BacklightOptions_Ctl = 1 << 0,
  BacklightOptions_Pwm = 1 << 1,
  BacklightOptions_LedController = 1 << 2,
} BacklightOptions;

typedef struct {
  OutputConfig output;
  void *peripheral; // Placeholder
  void *state; // Placeholder
} PwmConfig;

typedef struct {
  void *spi; // Placeholder
  const OutputConfig mosi;
  const OutputConfig clk;
  const OutputConfig cs;
  const OutputConfig on_ctrl;
  void *extcomin; // Placeholder
} BoardConfigSharpDisplay;

// Board Configuration Structure
/////////////////////////////////////////////////////////////////////////////
typedef struct {
  // Audio Configuration
  const bool has_mic;
  const MicConfig mic_config;

  // Ambient Light Configuration
  const uint32_t ambient_light_dark_threshold;
  const uint32_t ambient_k_delta_threshold;
  const OutputConfig photo_en;
  const bool als_always_on;

  // Debug Serial Configuration
  const GpioteConfig dbgserial_int;
  const InputConfig dbgserial_int_gpio;

  // Display Configuration
  const OutputConfig lcd_com;

  const uint8_t backlight_on_percent;
  const uint8_t backlight_max_duty_cycle_percent;
} BoardConfig;

// Button Configuration
/////////////////////////////////////////////////////////////////////////////
typedef struct {
  const ButtonConfig buttons[NUM_BUTTONS];
  const ButtonComConfig button_com;
  const bool active_high;
} BoardConfigButton;

// Power Configuration
/////////////////////////////////////////////////////////////////////////////
typedef struct {
  const GpioteConfig pmic_int;
  const InputConfig pmic_int_gpio;

  //! Percentage for watch only mode
  const uint8_t low_power_threshold;

  //! Approximate hours of battery life
  const uint16_t battery_capacity_hours;
} BoardConfigPower;

// Minimal board config for ESP32-C3
static const BoardConfig BOARD_CONFIG = {
  .has_mic = false,
  .mic_config = { .has_mic = false },
  .ambient_light_dark_threshold = 100,
  .ambient_k_delta_threshold = 30,
  .photo_en = { .gpio = NULL, .gpio_pin = 0, .active_high = false },
  .als_always_on = false,
  .dbgserial_int = { .gpio_pin = 0 },
  .dbgserial_int_gpio = { .gpio = NULL, .gpio_pin = 0 },
  .lcd_com = { .gpio = NULL, .gpio_pin = 0, .active_high = false },
  .backlight_on_percent = 25,
  .backlight_max_duty_cycle_percent = 67,
};

static const BoardConfigButton BOARD_CONFIG_BUTTON = {
  .buttons = {}, // Empty array since NUM_BUTTONS is 0
  .button_com = { .gpio_pin = 0 },
  .active_high = false,
};

static const BoardConfigPower BOARD_CONFIG_POWER = {
  .pmic_int = { .gpio_pin = 0 },
  .pmic_int_gpio = { .gpio = NULL, .gpio_pin = 0 },
  .low_power_threshold = 20,
  .battery_capacity_hours = 168, // ~7 days
};

// Additional board config types
typedef struct {
  const AccelConfig accel_config;
  const InputConfig accel_int_gpios[2];
  const GpioteConfig accel_ints[2];
} BoardConfigAccel;

typedef struct {
  const MagConfig mag_config;
  const InputConfig mag_int_gpio;
  const GpioteConfig mag_int;
} BoardConfigMag;

typedef struct {
  const ActuatorOptions options;
  const OutputConfig ctl;
  const PwmConfig pwm;
  const uint16_t vsys_scale;
} BoardConfigActuator;

typedef struct {
  const BacklightOptions options;
  const OutputConfig ctl;
  const PwmConfig pwm;
} BoardConfigBacklight;

// Dummy configs for compatibility
static const BoardConfigActuator BOARD_CONFIG_VIBE = {
  .options = 0,
  .ctl = { .gpio = NULL, .gpio_pin = 0, .active_high = false },
  .pwm = { .output = { .gpio = NULL, .gpio_pin = 0, .active_high = false }, .peripheral = NULL, .state = NULL },
  .vsys_scale = 0,
};
static const BoardConfigAccel BOARD_CONFIG_ACCEL = {
  .accel_config = { .axes_offsets = {0, 0, 0}, .axes_inverts = {false, false, false}, .shake_thresholds = {0, 0}, .double_tap_threshold = 0, .tap_shock = 0, .tap_quiet = 0, .tap_dur = 0 },
  .accel_int_gpios = {{ .gpio = NULL, .gpio_pin = 0 }, { .gpio = NULL, .gpio_pin = 0 }},
  .accel_ints = {{ .gpio_pin = 0 }, { .gpio_pin = 0 }},
};
static const BoardConfigMag BOARD_CONFIG_MAG = {
  .mag_config = { .axes_offsets = {0, 0, 0}, .axes_inverts = {false, false, false} },
  .mag_int_gpio = { .gpio = NULL, .gpio_pin = 0 },
  .mag_int = { .gpio_pin = 0 },
};
static const BoardConfigBacklight BOARD_CONFIG_BACKLIGHT = {
  .options = 0,
  .ctl = { .gpio = NULL, .gpio_pin = 0, .active_high = false },
  .pwm = { .output = { .gpio = NULL, .gpio_pin = 0, .active_high = false }, .peripheral = NULL, .state = NULL },
};
static const BoardConfigSharpDisplay BOARD_CONFIG_DISPLAY = {
  .spi = NULL,
  .mosi = { .gpio = NULL, .gpio_pin = 0, .active_high = false },
  .clk = { .gpio = NULL, .gpio_pin = 0, .active_high = false },
  .cs = { .gpio = NULL, .gpio_pin = 0, .active_high = false },
  .on_ctrl = { .gpio = NULL, .gpio_pin = 0, .active_high = false },
  .extcomin = NULL,
};

void board_early_init(void);
void board_init(void);

// UART device declarations
#if defined(MICRO_FAMILY_ESP32C3)
// Forward declare UARTDevice to avoid circular dependency
// (uart.h includes board.h, so we can't include uart.h here)
typedef const struct UARTDevice UARTDevice;
extern UARTDevice * const DBG_UART;
#endif
