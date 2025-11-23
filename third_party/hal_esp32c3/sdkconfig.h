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

// Minimal sdkconfig.h stub for ESP32-C3
// ESP-IDF's FreeRTOS headers require these CONFIG_* defines
// These are minimal defaults for bootloader support

// Stack checking (disabled for minimal bootloader)
#define CONFIG_STACK_CHECK_ALL 0
#define CONFIG_STACK_CHECK_STRONG 0

// Compiler optimization
#define CONFIG_COMPILER_OPTIMIZATION_NONE 0

// App trace (disabled for minimal bootloader)
#define CONFIG_APPTRACE_ENABLE 0
#define CONFIG_APPTRACE_SV_ENABLE 0

// FreeRTOS configuration
#define CONFIG_FREERTOS_USE_LIST_DATA_INTEGRITY_CHECK_BYTES 0
#define CONFIG_FREERTOS_UNICORE 1  // ESP32-C3 is single-core
#define CONFIG_FREERTOS_SMP 0  // Not using SMP (single-core)
#define CONFIG_FREERTOS_NUMBER_OF_CORES 1
#define CONFIG_FREERTOS_MAX_TASK_NAME_LEN 16
#define CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER 0
#define CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE 0
#define CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP 0
#define CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK 0
#define CONFIG_FREERTOS_USE_TICKLESS_IDLE 0
#define CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP 0
#define CONFIG_FREERTOS_HZ 100
#define CONFIG_FREERTOS_IDLE_TASK_STACKSIZE 1536
#define CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE 0
#define CONFIG_FREERTOS_TASK_NOTIFICATION_ARRAY_ENTRIES 1
#define CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS 1
#define CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS 0
#define CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY 0
#define CONFIG_FREERTOS_USE_IDLE_HOOK 0
#define CONFIG_FREERTOS_USE_TICK_HOOK 0
#define CONFIG_FREERTOS_CHECK_STACKOVERFLOW_NONE 0
#define CONFIG_FREERTOS_CHECK_STACKOVERFLOW_PTRVAL 0
#define CONFIG_FREERTOS_CHECK_STACKOVERFLOW_CANARY 0
#define CONFIG_FREERTOS_OPTIMIZED_SCHEDULER 1
#define CONFIG_FREERTOS_USE_PASSIVE_IDLE_HOOK 0
#define CONFIG_FREERTOS_USE_TRACE_FACILITY 0
#define CONFIG_FREERTOS_ISR_STACKSIZE 1536

// libc configuration
#define CONFIG_LIBC_NEWLIB 1  // Use newlib

// ESP32-C3 CPU frequency (default 160MHz)
#define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ 160

// CLIC (Core-Local Interrupt Controller) configuration
// ESP32-C3 does NOT use CLIC - it uses standard RISC-V interrupt handling
// So we don't define CONFIG_RISCV_CLIC or SOC_INT_CLIC_SUPPORTED
#define SOC_INT_CLIC_SUPPORTED 0
#define SOC_INT_PLIC_SUPPORTED 0

// FreeRTOS additional config defines
// Note: portBYTE_ALIGNMENT is defined by ESP-IDF's portmacro.h (as 16)
// Note: INCLUDE_vTaskDelayUntil conflicts with ESP-IDF's INCLUDE_xTaskDelayUntil
// We should not define these here - ESP-IDF handles them

