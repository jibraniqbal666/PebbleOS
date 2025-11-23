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
#if defined(MICRO_FAMILY_ESP32C3)
#include "util/time/time.h"
#else
#include <time.h>
#endif

void vibes_init();

int32_t vibes_get_vibe_strength(void);
int32_t vibes_get_default_vibe_strength(void);
void vibes_set_default_vibe_strength(int32_t vibe_strength_default);

void vibe_service_set_enabled(bool enable);
