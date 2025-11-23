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

#include <stdint.h>
#include <string.h>

#include "board/board_esp32c3.h"
#include "drivers/dbgserial.h"
#include "drivers/flash.h"
#include "firmware.h"
#include "fw_copy.h"
#include "system/bootbits.h"

// ESP-IDF includes
#include "esp_system.h"
#include "esp_chip_info.h"

// Function pointer type for firmware entry
typedef void (*firmware_entry_t)(void);

static void prv_get_fw_reset_vector(void **reset_handler,
                                     void **initial_stack_pointer) {
  // ESP32-C3 uses standard vector table format
  // First word is stack pointer, second is reset handler
  uintptr_t** fw_vector_table = (uintptr_t**) FIRMWARE_BASE;
  *initial_stack_pointer = (void *)fw_vector_table[0];
  *reset_handler = (void *)fw_vector_table[1];
}

static void __attribute__((noreturn)) jump_to_fw(void) {
  void *initial_stack_pointer, *reset_handler;
  prv_get_fw_reset_vector(&reset_handler, &initial_stack_pointer);

  dbgserial_print("Booting firmware @ ");
  dbgserial_print_hex((uintptr_t)reset_handler);
  dbgserial_newline();

  // Disable interrupts
  __asm__ volatile ("csrci mstatus, 8");  // Clear MIE bit

  // Set stack pointer
  __asm__ volatile (
    "mv sp, %0"
    :
    : "r" (initial_stack_pointer)
  );

  // Jump to firmware
  firmware_entry_t entry = (firmware_entry_t)reset_handler;
  entry();

  // Should never reach here
  while (1) {
    __asm__ volatile ("wfi");  // Wait for interrupt
  }
}

void boot_main(void) {
  // Initialize debug serial (critical for debugging)
  dbgserial_init();

  dbgserial_putstr("");
  dbgserial_putstr("ESP32-C3 Bootloader");
  dbgserial_putstr("");

  // Initialize flash
  flash_init();
  dbgserial_putstr("Flash initialized");

  // Initialize boot bits
  boot_bit_init();
  dbgserial_putstr("Boot bits initialized");

  // Check for firmware updates (stub for now)
  check_update_fw();

  // Verify firmware exists at FIRMWARE_BASE
  // Read first word to check if it's valid (non-zero stack pointer)
  uint32_t fw_stack_ptr = *(volatile uint32_t*)FIRMWARE_BASE;
  if (fw_stack_ptr == 0 || fw_stack_ptr == 0xFFFFFFFF) {
    dbgserial_putstr("ERROR: No valid firmware found!");
    dbgserial_print("Firmware base: ");
    dbgserial_print_hex(FIRMWARE_BASE);
    dbgserial_newline();
    // Stay in bootloader
    while (1) {
      __asm__ volatile ("wfi");
    }
  }

  dbgserial_print("Firmware found at ");
  dbgserial_print_hex(FIRMWARE_BASE);
  dbgserial_newline();

  // Clear boot bits if firmware was stable
  if (boot_bit_test(BOOT_BIT_FW_STABLE)) {
    dbgserial_putstr("Last firmware boot was stable; clearing strikes");
    boot_bit_clear(BOOT_BIT_FW_STABLE);
  }

  // Jump to firmware
  dbgserial_putstr("Jumping to firmware...");
  dbgserial_putstr("");
  jump_to_fw();
}

