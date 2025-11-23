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

/*
 * core_dump_structs.h
 *
 * This file specifies core_dump structures previously defined in fw/kernel/core_dump_private.h
 * This is so the Dialog BLE core_dump code can use the same structures.
 *
 */

#include "util/attributes.h"
#if defined(MICRO_FAMILY_ESP32C3)
#include "kernel/portmacro_esp32c3.h"
#else
#include "portmacro.h"
#endif

// Structure of thread info stored within a CORE_DUMP_CHUNK_KEY_THREAD chunk in the core dump
#define CORE_DUMP_THREAD_NAME_SIZE  16
typedef struct PACKED {
  int8_t    name[CORE_DUMP_THREAD_NAME_SIZE];       // Name, includes null termination
  uint32_t  id;             // thread id
  uint8_t   running;        // true if this thread is running
  uint32_t  registers[portCANONICAL_REG_COUNT];  // registers [r0-r12, sp, lr, pc, xpsr]
} CoreDumpThreadInfo;

// Structure of extra registers stored within a CORE_DUMP_CHUNK_KEY_EXTRA_REG chunk in the
// core dump
typedef struct PACKED {
  uint32_t  msp;
  uint32_t  psp;
  uint8_t   primask;
  uint8_t   basepri;
  uint8_t   faultmask;
  uint8_t   control;
} CoreDumpExtraRegInfo;



// We save all the important registers on entry to core_dump_reset() in a structure of this type
// on the core_dump_reset() stack and save a pointer to it in the s_saved_registers global.
// IMPORTANT!: There is assembly code near the top of core_dump_reset() that makes assumptions
// about the order and packing of this structure.
typedef struct PACKED {
  uint32_t core_reg[portCANONICAL_REG_COUNT];
  CoreDumpExtraRegInfo extra_reg;
} CoreDumpSavedRegisters;
