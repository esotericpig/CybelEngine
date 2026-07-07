/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_UTIL_TIMER_H_
#define CYBEL_UTIL_TIMER_H_

#include "cybel/common.h"

#include "cybel/types/duration.h"

#include <chrono>

namespace cybel {

class Timer final {
public:
  explicit Timer(bool start = false);

  void start();
  Duration stop();

  const Duration& pause();
  void resume();

  void sleep_for_fps(const Duration& target_fps);

  Duration peek() const;
  const Duration& duration() const;

private:
  using clock_t = std::chrono::steady_clock;
  using time_stamp_t = clock_t::time_point;

  bool is_ticking_ = false;
  time_stamp_t start_time_{};
  Duration duration_{};

  static time_stamp_t now();
};

} // namespace cybel
#endif
