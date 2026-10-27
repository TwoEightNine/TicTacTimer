// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "neo.h"

struct Animation {
  const char* name;
  const uint8_t (*frames)[ANIMATION_FRAME_SIZE];
  uint16_t frame_count;
  uint16_t frame_duration_ms;
};

#define ANIMATIONS_COUNT 1

const Animation animations[ANIMATIONS_COUNT] = {
  { "neo", neo_frames, NEO_FRAME_COUNT, 60 }
};
