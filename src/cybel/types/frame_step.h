/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_FRAME_STEP_H_
#define CYBEL_TYPES_FRAME_STEP_H_

#include "cybel/common.h"

#include "cybel/types/duration.h"

namespace cybel {

struct FrameStep {
  Duration dpf{};
  double delta_time{};
};

} // namespace cybel
#endif
