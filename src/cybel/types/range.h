/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_RANGE_H_
#define CYBEL_TYPES_RANGE_H_

#include "cybel/common.h"

#include <type_traits>

namespace cybel {

class Range2i final {
public:
  int min{};
  int max{};

  constexpr bool in_range(int value) const;

  constexpr Range2i& set(int min,int max);
};

static_assert(std::is_aggregate_v<Range2i>);

constexpr bool Range2i::in_range(int value) const {
  return value >= min && value <= max;
}

constexpr Range2i& Range2i::set(int min,int max) {
  this->min = min;
  this->max = max;

  return *this;
}

} // namespace cybel
#endif
