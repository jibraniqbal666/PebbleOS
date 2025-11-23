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
// ESP32-C3 (RISC-V) interrupt control
// Include asm_compat.h first to ensure 'asm' keyword is recognized
#include "asm_compat.h"
#include "riscv/csr.h"
#include "riscv/encoding.h"  // For MSTATUS_MIE

static inline void mcu_state_disable_interrupts(void) {
  // Disable interrupts globally using RISC-V CSR
  // Clear mstatus.MIE (Machine Interrupt Enable) bit (bit 3 = 0x8)
  // This is equivalent to ARM's __disable_irq()
  RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
}

static inline void mcu_state_enable_interrupts(void) {
  // Enable interrupts globally using RISC-V CSR
  // Set mstatus.MIE (Machine Interrupt Enable) bit (bit 3 = 0x8)
  // This is equivalent to ARM's __enable_irq()
  RV_SET_CSR(mstatus, MSTATUS_MIE);
}
#else
// Generic non-ARM implementation (stub)
static inline void mcu_state_disable_interrupts(void) {
  // Stub implementation for non-ARM platforms
  // This should be implemented per platform if needed
}

static inline void mcu_state_enable_interrupts(void) {
  // Stub implementation for non-ARM platforms
  // This should be implemented per platform if needed
}
#endif

static inline bool mcu_state_is_isr(void) {
  return false;
}

static inline uint32_t mcu_state_get_isr_priority(void) {
  return ~0;
}
