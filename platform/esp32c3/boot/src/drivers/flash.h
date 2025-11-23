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

#include <stdbool.h>
#include <stdint.h>

//! Configure the micro's peripherals to communicate with the flash chip
void flash_init(void);

//! Read 1 or more bytes starting at the specified 24bit address into
//! the provided buffer. This function does no range checking, so it is
//! currently possible to run off the end of the flash.
//!
//! @param buffer A byte-buffer that will be used to store the data
//! read from flash.
//! @param start_addr The address of the first byte to be read from flash.
//! @param buffer_size The total number of bytes to be read from flash.
void flash_read_bytes(uint8_t* buffer, uint32_t start_addr, uint32_t buffer_size);

//! Check if we can talk to the flash.
//! @return true if the CFI table can be queried.
bool flash_sanity_check(void);

//! Get the checksum of a region of flash
uint32_t flash_calculate_checksum(uint32_t flash_addr, uint32_t length);

