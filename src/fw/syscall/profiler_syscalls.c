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

#include <inttypes.h>

#include "syscall/syscall_internal.h"
#include "system/profiler.h"

#define CMSIS_COMPATIBLE
#define ESP32C3_COMPATIBLE
#include <mcu.h>

#if defined(MICRO_FAMILY_ESP32C3)
// Include asm_compat.h FIRST before any ESP-IDF headers that use 'asm' keyword
#include "asm_compat.h"
// RISC-V cycle counter (mcycle CSR = 0xB00)
// Create our own CSR read function to avoid ESP-IDF's inline assembly macros
// which have issues with assembler flag passing
static inline uint32_t riscv_read_cycle_counter(void) {
  uint32_t result;
  // Use direct inline assembly with mcycle CSR (0xB00)
  // This avoids ESP-IDF's rv_utils.h which has assembler flag issues
  __asm__ volatile ("csrr %0, mcycle" : "=r" (result));
  return result;
}
#endif


// ------------------------------------------------------------------------------------
// Find node by ptr
static bool prv_ptr_list_filter(ListNode* list_node, void* data) {
  ProfilerNode* node = (ProfilerNode*)list_node;
  return (node == data);
}


ProfilerNode *prv_find_node(ProfilerNode *find_node) {
  ListNode* node = list_find(g_profiler.nodes, prv_ptr_list_filter, (void*)find_node);

  return (ProfilerNode *)node;
}

DEFINE_SYSCALL(void, sys_profiler_init, void) {
  profiler_init();
}

DEFINE_SYSCALL(void, sys_profiler_start, void) {
  profiler_start();
}

DEFINE_SYSCALL(void, sys_profiler_stop, void) {
  profiler_stop();
}

DEFINE_SYSCALL(void, sys_profiler_print_stats, void) {
  profiler_print_stats();
}

DEFINE_SYSCALL(void, sys_profiler_node_start, ProfilerNode *node) {
  if (PRIVILEGE_WAS_ELEVATED) {
    if (!list_contains(g_profiler.nodes, (ListNode *)node)) {
      // Instead of calling syscall_failed(), simply return. If PROFILE_INIT has not been
      // executed yet, there won't be any nodes in the list.
      return;
    }
  }

#if defined(MICRO_FAMILY_ESP32C3)
  node->start = riscv_read_cycle_counter();
#else
  node->start = DWT->CYCCNT;
#endif
}

DEFINE_SYSCALL(void, sys_profiler_node_stop, ProfilerNode *node) {

  // Capture the cycle count as soon as possible, before we validate the node argument
#if defined(MICRO_FAMILY_ESP32C3)
  uint32_t dwt_cyc_cnt = riscv_read_cycle_counter();
#else
  uint32_t dwt_cyc_cnt = DWT->CYCCNT;
#endif

  if (PRIVILEGE_WAS_ELEVATED) {
    if (!list_contains(g_profiler.nodes, (ListNode *)node)) {
      // Instead of calling syscall_failed(), simply return. If PROFILE_INIT has not been
      // executed yet, there won't be any nodes in the list.
      return;
    }
  }

  profiler_node_stop(node, dwt_cyc_cnt);
}
