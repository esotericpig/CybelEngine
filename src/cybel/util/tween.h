/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_UTIL_TWEEN_H_
#define CYBEL_UTIL_TWEEN_H_

#include "cybel/common.h"

#include "cybel/types/cybel_error.h"
#include "cybel/types/frame_step.h"

#include <cmath>

namespace cybel {

template <typename T>
class Tween final {
public:
  bool is_playing = false;
  T value{};
  T duration{1.0};
  T progress{};

  explicit Tween() noexcept = default;
  explicit Tween(T duration,bool play = false);
  explicit Tween(T progress,T duration,bool play = false);

  void play();
  void stop();

  void pause();
  void resume();

  void fade(const FrameStep& step);
  void flash(const FrameStep& step);

  bool was_activated();

private:
  bool update(const FrameStep& step);

  void reset();
};

using Tween0f = Tween<float>;
using Tween0d = Tween<double>;

template <typename T>
Tween<T>::Tween(T duration,bool play)
  : Tween{T{},duration,play} {}

template <typename T>
Tween<T>::Tween(T progress,T duration,bool play)
  : duration{duration},progress{progress} {
  if(duration == T{0.0}) { throw CybelError{"`duration` cannot be zero."}; }

  if(play) { resume(); }
}

template <typename T>
void Tween<T>::play() {
  reset();
  resume();
}

template <typename T>
void Tween<T>::stop() {
  pause();
  reset();
}

template <typename T>
void Tween<T>::pause() {
  is_playing = false;
}

template <typename T>
void Tween<T>::resume() {
  is_playing = true;
}

template <typename T>
void Tween<T>::fade(const FrameStep& step) {
  if(!update(step)) { return; }

  value = progress;
}

template <typename T>
void Tween<T>::flash(const FrameStep& step) {
  if(!update(step)) { return; }

  value = T{1.0} - std::abs(progress * T{2.0} - T{1.0});
}

template <typename T>
bool Tween<T>::update(const FrameStep& step) {
  if(!is_playing) { return false; }

  if(progress >= T{1.0}) {
    pause();
    return false;
  }

  // TODO: If add float/double delta_time(_high), do `if constexpr` here.
  progress += (static_cast<T>(step.delta_time) / duration);

  if(progress > T{1.0}) {
    // Don't pause (set `is_playing` to false) and/or return false,
    // so that 100% value/progress shows at least once.
    progress = T{1.0};
  }

  return true;
}

template <typename T>
void Tween<T>::reset() {
  value = T{};
  progress = T{};
}

template <typename T>
bool Tween<T>::was_activated() {
  return is_playing || progress > T{0.0};
}

} // namespace cybel
#endif
