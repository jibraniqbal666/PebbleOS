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

inline static bool mcu_state_is_isr(void);

//! Return the priority level of the currently executing exception handler.
//! Returns ~0 if not in an exception handler. Lower numbers mean higher
//! priority. Anything below 0xB should not execute any FreeRTOS calls.
inline static uint32_t mcu_state_get_isr_priority(void);

//! Disable interrupts globally (equivalent to ARM's __disable_irq())
inline static void mcu_state_disable_interrupts(void);

//! Enable interrupts globally (equivalent to ARM's __enable_irq())
inline static void mcu_state_enable_interrupts(void);

bool mcu_state_are_interrupts_enabled(void);

#ifdef __arm__
#include "mcu/interrupts_arm.inl.h"
#else
#include "mcu/interrupts_stubs.inl.h"
#endif
