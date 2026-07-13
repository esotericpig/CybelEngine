/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_INPUT_JOYSTICK_H_
#define CYBEL_INPUT_JOYSTICK_H_

#include "cybel/common.h"

namespace cybel {

class Joystick final {
public:
  explicit Joystick() noexcept = default;
  ~Joystick() noexcept;

  Joystick(const Joystick& other) = delete;
  Joystick& operator=(const Joystick& other) = delete;

  Joystick(Joystick&& other) noexcept;
  Joystick& operator=(Joystick&& other) noexcept;

  void open(int id) noexcept;
  void close() noexcept;

  explicit operator bool() const;
  bool matches(int id) const;
  int id() const;

  SDL_Joystick* handle() const;

private:
  int id_ = -1;
  SDL_Joystick* handle_ = nullptr;

  void move_from(Joystick&& other) noexcept;
};

} // namespace cybel
#endif
