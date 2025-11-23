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

// Compatibility header to ensure 'asm' keyword works
// This should be included before any ESP-IDF headers that use 'asm'
// GCC should recognize 'asm' as a keyword in C mode, but if it doesn't,
// we can define it here

#ifndef __cplusplus
// In C mode, 'asm' should be a keyword, but if it's not recognized,
// we can ensure it's available
// Note: This is a workaround - normally GCC should handle this automatically
#ifndef asm
// If 'asm' is somehow not defined, map it to __asm__
#define asm __asm__
#endif
#endif

