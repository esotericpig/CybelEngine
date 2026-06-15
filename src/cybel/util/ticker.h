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

    kOneShot = 1 << 0, /// Default.
    kLoop = 1 << 1,

    kStart = 1 << 2,
    kFireAsap = 1 << 3,
  };

  bool is_ticking = false;
  bool is_looping = false;
  T value{};
  T interval{};

  explicit Ticker() noexcept = default;
  explicit Ticker(T interval,int flags = kNone) noexcept;
  explicit Ticker(T value,T interval,int flags = kNone) noexcept;

  void start(int flags = kNone);
  void stop();
  void toggle(int flags = kNone);

  void resume();
  void pause();

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
Ticker<T>::Ticker(T interval,int flags) noexcept
  : Ticker{T{},interval,flags} {}

template <typename T>
Ticker<T>::Ticker(T value,T interval,int flags) noexcept
  : is_looping{(flags & kLoop) != 0},
    value{(flags & kFireAsap) ? interval : value},
    interval{interval} {
  if(flags & kStart) { resume(); }
}

template <typename T>
void Ticker<T>::start(int flags) {
  // Only if explicitly requested, change looping.
  if(flags & kOneShot) {
    is_looping = false;
  } else if(flags & kLoop) {
    is_looping = true;
  }

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
void Ticker<T>::toggle(int flags) {
  if(is_ticking) {
    // Only if explicitly requested, change looping.
    if(flags & kOneShot) {
      is_looping = false;
    } else if(flags & kLoop) {
      is_looping = true;
    }

    stop();
  } else {
    start(flags);
  }
}

template <typename T>
void Ticker<T>::resume() {
  is_ticking = true;
}

template <typename T>
void Ticker<T>::pause() {
  is_ticking = false;
}

template <typename T>
bool Ticker<T>::tick(const FrameStep& step) {
  if(!update(step)) { return false; }
  if(value < interval) { return false; }

  if(is_looping) {
    reset_value();
  } else { // One-Shot
    pause();
    value = interval; // Stay at clamped end.
  }

  return true;
}

template <typename T>
int Ticker<T>::spoon_ticks(const FrameStep& step) {
  if(!update(step)) { return 0; }

  int ticks = 0;

  if(value >= interval) {
    do {
      ++ticks;
      value -= interval;
    } while(value >= interval);

    if(!is_looping) {
      pause();
      value += interval; // Stay at end with overflow.
    }
  }

  return ticks;
}

template <typename T>
bool Ticker<T>::spooned_ticks(const FrameStep& step) {
  return spoon_ticks(step) > 0;
}

template <typename T>
bool Ticker<T>::update(const FrameStep& step) {
  if(!is_ticking) { return false; }

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
