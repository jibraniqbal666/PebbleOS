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

#include "util/attributes.h"

#include "wfi.h"

#if defined(MICRO_FAMILY_ESP32C3)
// RISC-V (ESP32-C3) implementation
void NOINLINE NAKED_FUNC do_wfi(void) {
  // RISC-V uses 'wfi' instruction and 'ret' to return
  __asm volatile (
      "wfi      \n"  // Wait for interrupt
      "ret      \n"  // Return (RISC-V equivalent of bx lr)
      );
}
#else
// ARM implementation
void NOINLINE NAKED_FUNC do_wfi(void) {
  // Work around a very strange bug in the STM32F where, upon waking from
  // STOP or SLEEP mode, the processor begins acting strangely depending on the
  // contents of the bytes following the "bx lr" instruction.
  __asm volatile (
      ".align 4 \n"  // Force 16-byte alignment
      "wfi      \n"  // This instruction cannot be placed at 0xnnnnnnn4
      "nop      \n"
      "bx lr    \n"
      "nop      \n"  // Fill the rest of the cache line with NOPs as the bytes
      "nop      \n"  // following the bx affect the processor for some reason.
      "nop      \n"
      "nop      \n"
      "nop      \n"
      );
}
#endif
