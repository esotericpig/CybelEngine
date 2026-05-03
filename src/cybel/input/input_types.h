/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_INPUT_INPUT_TYPES_H_
#define CYBEL_INPUT_INPUT_TYPES_H_

#include "cybel/common.h"

#include <unordered_set>
#include <vector>

namespace cybel {

using input_id_t = std::size_t;
using InputIds = std::unordered_set<input_id_t>;
using InputStates = std::vector<std::uint8_t>;

} // namespace cybel
#endif
