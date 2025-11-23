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

#if defined(MICRO_FAMILY_ESP32C3)
#include "util/time/time.h"
#else
#include "time.h"
#endif

#include "syscall.h"

#include "syscall_internal.h"

#include "drivers/rtc.h"
#include "kernel/memory_layout.h"
#include "kernel/pebble_tasks.h"
#include "mcu/privilege.h"
#include "os/tick.h"
#include "process_management/app_manager.h"
#include "process_management/worker_manager.h"
#include "services/common/comm_session/session.h"
#include "system/logging.h"
#include "system/passert.h"
#include "util/string.h"

#include "FreeRTOS.h"
#include "task.h"

DEFINE_SYSCALL(int, sys_test, int arg) {
  uint32_t ipsr = 0;
#if defined(MICRO_FAMILY_ESP32C3)
  // RISC-V doesn't have IPSR register, use 0 as placeholder
  // IPSR (Interrupt Program Status Register) is ARM-specific
#else
  __asm volatile("mrs %0, ipsr" : "=r" (ipsr));
#endif

  PBL_LOG(LOG_LEVEL_DEBUG, "Inside test kernel function! Privileged? %s Arg %u IPSR: %"PRIu32,
          bool_to_str(mcu_state_is_privileged()), arg, ipsr);

  return arg * 2;
}

DEFINE_SYSCALL(time_t, sys_get_time, void) {
  return rtc_get_time();
}

DEFINE_SYSCALL(void, sys_get_time_ms, time_t *t, uint16_t *out_ms) {
  if (PRIVILEGE_WAS_ELEVATED) {
    syscall_assert_userspace_buffer(t, sizeof(*t));
    syscall_assert_userspace_buffer(out_ms, sizeof(*out_ms));
  }

  rtc_get_time_ms(t, out_ms);
}

DEFINE_SYSCALL(RtcTicks, sys_get_ticks, void) {
  return rtc_get_ticks();
}

DEFINE_SYSCALL(void, sys_pbl_log, LogBinaryMessage* log_message, bool async) {
  kernel_pbl_log(log_message, async);
}

DEFINE_SYSCALL(void, sys_copy_timezone_abbr, char* timezone_abbr, time_t time) {
  if (PRIVILEGE_WAS_ELEVATED) {
    syscall_assert_userspace_buffer(timezone_abbr, TZ_LEN);
  }
  time_get_timezone_abbr(timezone_abbr, time);
}

DEFINE_SYSCALL(struct tm*, sys_gmtime_r, const time_t *timep, struct tm *result) {
  if (PRIVILEGE_WAS_ELEVATED) {
    syscall_assert_userspace_buffer(timep, sizeof(*timep));
    syscall_assert_userspace_buffer(result, sizeof(*result));
  }
  return gmtime_r(timep, result);
}

DEFINE_SYSCALL(struct tm*, sys_localtime_r, const time_t *timep, struct tm *result) {
  if (PRIVILEGE_WAS_ELEVATED) {
    syscall_assert_userspace_buffer(timep, sizeof(*timep));
    syscall_assert_userspace_buffer(result, sizeof(*result));
  }
  return localtime_r(timep, result);
}

//! System call to exit an application gracefully.
DEFINE_SYSCALL(NORETURN, sys_exit, void) {
  process_manager_task_exit();
}

DEFINE_SYSCALL(void, sys_psleep, int millis) {
  vTaskDelay(milliseconds_to_ticks(millis));
}
