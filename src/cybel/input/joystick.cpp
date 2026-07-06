/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "joystick.h"

namespace cybel {

Joystick::Joystick(Joystick&& other) noexcept {
  move_from(std::move(other));
}

void Joystick::move_from(Joystick&& other) noexcept {
  close();

  handle_ = other.handle_;
  other.handle_ = NULL;

  id_ = std::exchange(other.id_,-1);
}

Joystick::~Joystick() noexcept {
  close();
}

Joystick& Joystick::operator=(Joystick&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

Joystick::operator bool() const { return handle_ != NULL; }

void Joystick::open(int id) noexcept {
  close();

  handle_ = SDL_JoystickOpen(id);

  if(handle_ == NULL) {
    std::cerr << "[WARN] Failed to open joystick [" << id << "]: " << SDL_GetError() << '.' << std::endl;
    return;
  }

  id_ = id;
}

void Joystick::close() noexcept {
  if(handle_ != NULL) {
    SDL_JoystickClose(handle_);
    handle_ = NULL;

    id_ = -1;
  }
}

bool Joystick::matches(int id) const {
  return handle_ != NULL && id_ == id;
}

int Joystick::id() const { return id_; }

SDL_Joystick* Joystick::handle() const { return handle_; }

} // namespace cybel
