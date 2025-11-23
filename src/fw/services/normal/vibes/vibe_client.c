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

#include "vibe_client.h"

#include "applib/ui/vibes.h"
#include "services/normal/notifications/alerts_preferences_private.h"
#include "services/normal/vibes/vibe_score.h"
#include "services/normal/vibes/vibe_score_info.h"
#include "system/logging.h"

static VibeScoreId prv_get_resource_for_client(VibeClient client) {
  if (client == VibeClient_AlarmsLPM) {
    return VibeScoreId_AlarmsLPM;
  }
#if CAPABILITY_HAS_VIBE_SCORES
  return alerts_preferences_get_vibe_score_for_client(client);
#else
  // ESP32-C3 doesn't support vibe scores, return disabled
  return VibeScoreId_Disabled;
#endif
}

VibeScore *vibe_client_get_score(VibeClient client) {
  VibeScoreId id = prv_get_resource_for_client(client);
  if (id == VibeScoreId_Disabled) {
    return NULL;
  }
  VibeScore *score = vibe_score_create_with_resource(vibe_score_info_get_resource_id(id));
  if (!score) {
    PBL_LOG(LOG_LEVEL_ERROR, "Got a null VibeScore resource!");
  }
  return score;
}
