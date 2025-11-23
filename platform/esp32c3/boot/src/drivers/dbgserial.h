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

void dbgserial_init(void);

void dbgserial_putstr(const char* str);

void dbgserial_newline(void);

//! Like dbgserial_putstr, but without a terminating newline
void dbgserial_print(const char* str);

void dbgserial_print_hex(uint32_t value);

void dbgserial_putstr_fmt(char* buffer, unsigned int buffer_size, const char* fmt, ...)
    __attribute__((format(printf, 3, 4)));

