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

#include "drivers/flash.h"
#include "board/board_esp32c3.h"

#include <string.h>

// ESP-IDF includes
#include "esp_flash.h"
#include "spi_flash_mmap.h"
#include "util/crc32.h"

// Helper function for crc32_update compatibility
static inline uint32_t crc32_update(uint32_t crc, const void *data, size_t length) {
    return crc32(crc, data, length);
}

static bool flash_initialized = false;

void flash_init(void) {
    if (flash_initialized) {
        return;
    }
    // ESP32-C3 flash is memory-mapped, so initialization is minimal
    // ESP-IDF would handle this automatically, but for minimal bootloader
    // we just mark it as initialized
    flash_initialized = true;
}

void flash_read_bytes(uint8_t* buffer, uint32_t start_addr, uint32_t buffer_size) {
    if (!flash_initialized) {
        flash_init();
    }

    // ESP32-C3 uses memory-mapped flash, so we can read directly
    // start_addr is relative to flash base (0x0)
    const void* flash_ptr = (const void*)(FLASH_BASE + start_addr);
    memcpy(buffer, flash_ptr, buffer_size);
}

bool flash_sanity_check(void) {
    if (!flash_initialized) {
        flash_init();
    }
    // Basic sanity check - try to read first few bytes
    uint8_t test_buffer[4];
    flash_read_bytes(test_buffer, 0, 4);
    // If we can read, assume it's working
    return true;
}

uint32_t flash_calculate_checksum(uint32_t flash_addr, uint32_t length) {
    if (!flash_initialized) {
        flash_init();
    }

    // Read data in chunks to calculate CRC
    uint8_t buffer[256];
    uint32_t crc = 0;
    uint32_t remaining = length;
    uint32_t offset = 0;

    while (remaining > 0) {
        uint32_t chunk_size = (remaining > sizeof(buffer)) ? sizeof(buffer) : remaining;
        flash_read_bytes(buffer, flash_addr + offset, chunk_size);
        crc = crc32_update(crc, buffer, chunk_size);
        offset += chunk_size;
        remaining -= chunk_size;
    }

    return crc;
}

