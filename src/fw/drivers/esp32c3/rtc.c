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

#include "drivers/rtc.h"

#include "util/time/time.h"

#if defined(MICRO_FAMILY_ESP32C3)
#include "FreeRTOS.h"
#include "task.h"
#else
#include "FreeRTOS.h"
#include "task.h"
#endif

#include <stdint.h>
#include <stdbool.h>

// ESP32-C3 RTC implementation using FreeRTOS tick count
// Minimal implementation for bootloader support

static time_t s_time_base = 0;
static RtcTicks s_time_tick_base = 0;

static RtcTicks prv_get_ticks(void) {
  // Use FreeRTOS tick count as RTC ticks
  // This is a minimal implementation - proper RTC would use ESP32-C3's RTC peripheral
  return (RtcTicks)xTaskGetTickCount();
}

void rtc_set_time(time_t time) {
  s_time_base = time;
  s_time_tick_base = prv_get_ticks();
}

time_t rtc_get_time(void) {
  RtcTicks ticks = prv_get_ticks();
  RtcTicks ticks_since_time_base = (ticks - s_time_tick_base);
  // RTC_TICKS_HZ is defined in rtc.h - use it directly
  return s_time_base + (ticks_since_time_base / RTC_TICKS_HZ);
}

void rtc_get_time_ms(time_t* out_seconds, uint16_t* out_ms) {
  RtcTicks ticks = prv_get_ticks();
  RtcTicks ticks_since_time_base = (ticks - s_time_tick_base);
  
  *out_seconds = s_time_base + (ticks_since_time_base / RTC_TICKS_HZ);
  
  RtcTicks ticks_this_second = ticks_since_time_base % RTC_TICKS_HZ;
  *out_ms = (uint16_t)((ticks_this_second * 1000) / RTC_TICKS_HZ);
}

RtcTicks rtc_get_ticks(void) {
  return prv_get_ticks();
}

// Stub implementations for RTC functions not needed for minimal bootloader
void rtc_init_timers(void) {}
void rtc_alarm_init(void) {}
void rtc_alarm_set(RtcTicks num_ticks) { (void)num_ticks; }
RtcTicks rtc_alarm_get_elapsed_ticks(void) { return 0; }
bool rtc_alarm_is_initialized(void) { return false; }
void rtc_set_timezone(TimezoneInfo *tzinfo) { (void)tzinfo; }
void rtc_get_timezone(TimezoneInfo *tzinfo) { (void)tzinfo; }
uint16_t rtc_get_timezone_id(void) { return 0; }
bool rtc_is_timezone_set(void) { return false; }
void rtc_timezone_clear(void) {}
void rtc_set_time_tm(struct tm* time_tm) { (void)time_tm; }
void rtc_get_time_tm(struct tm* time_tm) {
  time_t t = rtc_get_time();
  gmtime_r(&t, time_tm);
}
const char* rtc_get_time_string(char* buffer) { (void)buffer; return ""; }
bool rtc_sanitize_struct_tm(struct tm* t) { (void)t; return false; }
bool rtc_sanitize_time_t(time_t* t) { (void)t; return false; }

