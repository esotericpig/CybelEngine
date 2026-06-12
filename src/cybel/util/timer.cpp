/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "timer.h"

#include <thread>

namespace cybel {

Timer::time_stamp_t Timer::now() {
  return clock_t::now();
}

Timer::Timer(bool start) {
  if(start) { this->start(); }
}

void Timer::start() {
  is_ticking_ = true;
  duration_.set_to_zero();
  start_time_ = now();
}

Duration Timer::stop() {
  pause();

  Duration duration = duration_;
  duration_.set_to_zero();

  return duration;
}

void Timer::resume() {
  if(!is_ticking_) {
    is_ticking_ = true;
    start_time_ = now();
  }
}

const Duration& Timer::pause() {
  if(is_ticking_) {
    duration_.value_ += (now() - start_time_);
    is_ticking_ = false;
  }

  return duration_;
}

void Timer::sleep_for_fps(const Duration& target_fps) {
  if(!is_ticking_) { return; }

  std::this_thread::sleep_until(start_time_ + target_fps.value_);
}

Duration Timer::peek() const {
  if(!is_ticking_) { return duration_; }

  return duration_ + Duration{now() - start_time_};
}

const Duration& Timer::duration() const { return duration_; }

} // namespace cybel
