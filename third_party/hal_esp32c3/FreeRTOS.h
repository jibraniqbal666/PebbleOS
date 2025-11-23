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

// Wrapper header to redirect FreeRTOS.h includes to ESP-IDF's FreeRTOS.h
// This prevents conflicts between PebbleOS's FreeRTOS config and ESP-IDF's FreeRTOS config
// Include asm compatibility first
#include "asm_compat.h"

// Undefine config that might be defined by projdefs.h before FreeRTOSConfig.h redefines it
// This prevents redefinition warnings/errors
#ifdef configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES
#undef configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES
#endif

// Include ESP-IDF's FreeRTOS.h
// ESP-IDF FreeRTOS headers are in freertos/ subdirectory
// FreeRTOS.h will include FreeRTOSConfig.h before projdefs.h
#include "freertos/FreeRTOS.h"

