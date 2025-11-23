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

#include "system/bootbits.h"

// Minimal implementation using ESP-IDF NVS or RTC memory
// For now, use a simple static variable (will be lost on reset)
static uint32_t boot_bits = 0;

void boot_bit_init() {
    // TODO: Initialize from NVS or RTC memory
    boot_bits = 0;
}

void boot_bit_set(BootBitValue bit) {
    boot_bits |= bit;
}

void boot_bit_clear(BootBitValue bit) {
    boot_bits &= ~bit;
}

bool boot_bit_test(BootBitValue bit) {
    return (boot_bits & bit) != 0;
}

