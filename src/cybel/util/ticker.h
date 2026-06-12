/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_UTIL_TICKER_H_
#define CYBEL_UTIL_TICKER_H_

#include "cybel/common.h"

#include "cybel/types/duration.h"
#include "cybel/types/frame_step.h"

#include <concepts>
#include <cstdint>

namespace cybel {

/// Spoon!
template <typename T>
class Ticker final {
public:
  enum Flags : std::uint8_t {
    kNone = 0,
    kStart = 1 << 0,
    kFireAsap = 1 << 1,
  };

  bool is_enabled = false;
  T value{};
  T interval{};

  explicit Ticker() noexcept = default;
  explicit Ticker(T interval,Flags flags = kNone) noexcept;
  explicit Ticker(T value,T interval,Flags flags = kNone) noexcept;

  void start(Flags flags = kNone);
  void stop();
  void toggle(Flags flags = kNone);

  void resume();
  void pause();
  void toggle_pause();

  /// If fires, resets the value to 0.
  ///
  /// @return whether fired or not.
  bool tick(const FrameStep& step);

  /// If fires, adjusts the value while preserving overflow.
  ///
  /// @return the number of ticks fired.
  int spoon_ticks(const FrameStep& step);

  /// Calls spoon_ticks() and returns true if the result is > 0.
  ///
  /// @return whether fired or not.
  bool spooned_ticks(const FrameStep& step);

  T time_left() const;

private:
  bool update(const FrameStep& step);

  void reset_value();
};

using ChronoTicker = Ticker<Duration>; /// Chrono Trigger?
using Ticker0f = Ticker<float>;
using Ticker0d = Ticker<double>;

template <typename T>
Ticker<T>::Ticker(T interval,Flags flags) noexcept
  : Ticker{T{},interval,flags} {}

template <typename T>
Ticker<T>::Ticker(T value,T interval,Flags flags) noexcept
  : value{(flags & kFireAsap) ? interval : value},interval{interval} {
  if(flags & kStart) { resume(); }
}

template <typename T>
void Ticker<T>::start(Flags flags) {
  stop();
  resume();

  if(flags & kFireAsap) { value = interval; }
}

template <typename T>
void Ticker<T>::stop() {
  pause();
  reset_value();
}

template <typename T>
void Ticker<T>::toggle(Flags flags) {
  if(is_enabled) {
    stop();
  } else {
    start(flags);
  }
}

template <typename T>
void Ticker<T>::resume() {
  is_enabled = true;
}

template <typename T>
void Ticker<T>::pause() {
  is_enabled = false;
}

template <typename T>
void Ticker<T>::toggle_pause() {
  if(is_enabled) {
    pause();
  } else {
    resume();
  }
}

template <typename T>
bool Ticker<T>::tick(const FrameStep& step) {
  if(!update(step)) { return false; }
  if(value < interval) { return false; }

  reset_value();

  return true;
}

template <typename T>
int Ticker<T>::spoon_ticks(const FrameStep& step) {
  if(!update(step)) { return 0; }

  int ticks = 0;

  for(; value >= interval; value -= interval) {
    ++ticks;
  }

  return ticks;
}

template <typename T>
bool Ticker<T>::spooned_ticks(const FrameStep& step) {
  return spoon_ticks(step) > 0;
}

template <typename T>
bool Ticker<T>::update(const FrameStep& step) {
  if(!is_enabled) { return false; }

  if constexpr(std::same_as<T,Duration>) {
    value += step.dpf;
  } else {
    // TODO: If add float/double delta_time(_high), add `else if constexpr` here.
    value += static_cast<T>(step.delta_time);
  }

  return true;
}

template <typename T>
void Ticker<T>::reset_value() {
  if constexpr(std::same_as<T,Duration>) {
    value.set_to_zero();
  } else {
    value = T{};
  }
}

template <typename T>
T Ticker<T>::time_left() const {
  return interval - value;
}

} // namespace cybel
#endif
