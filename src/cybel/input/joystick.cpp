/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "joystick.h"

namespace cybel {

Joystick::~Joystick() noexcept {
  close();
}

Joystick::Joystick(Joystick&& other) noexcept {
  move_from(std::move(other));
}

Joystick& Joystick::operator=(Joystick&& other) noexcept {
  if(this != &other) {
    close();
    move_from(std::move(other));
  }

  return *this;
}

void Joystick::move_from(Joystick&& other) noexcept {
  id_ = std::exchange(other.id_,-1);
  handle_ = std::exchange(other.handle_,nullptr);
}

void Joystick::open(int id) noexcept {
  close();

  handle_ = SDL_JoystickOpen(id);

  if(!handle_) {
    std::cerr << "[WARN] Failed to open Joystick `" << id << "`: " << SDL_GetError() << '.' << std::endl;
    return;
  }

  id_ = id;
}

void Joystick::close() noexcept {
  if(handle_) {
    SDL_JoystickClose(handle_);
    handle_ = nullptr;

    id_ = -1;
  }
}

Joystick::operator bool() const { return handle_ != nullptr; }

bool Joystick::matches(int id) const {
  return handle_ && id_ == id;
}

int Joystick::id() const { return id_; }

SDL_Joystick* Joystick::handle() const { return handle_; }

} // namespace cybel
