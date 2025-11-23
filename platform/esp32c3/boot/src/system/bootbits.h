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

#include <stdint.h>
#include <stdbool.h>

typedef enum BootBitValue {
  BOOT_BIT_INITIALIZED = 0x1 << 0,
  BOOT_BIT_FW_STABLE = 0x1 << 14,
} BootBitValue;

void boot_bit_init();
void boot_bit_set(BootBitValue bit);
void boot_bit_clear(BootBitValue bit);
bool boot_bit_test(BootBitValue bit);

