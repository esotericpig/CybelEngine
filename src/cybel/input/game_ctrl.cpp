/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "game_ctrl.h"

namespace cybel {

GameCtrl::~GameCtrl() noexcept {
  close();
}

GameCtrl::GameCtrl(GameCtrl&& other) noexcept {
  move_from(std::move(other));
}

GameCtrl& GameCtrl::operator=(GameCtrl&& other) noexcept {
  if(this != &other) {
    close();
    move_from(std::move(other));
  }

  return *this;
}

void GameCtrl::move_from(GameCtrl&& other) noexcept {
  id_ = std::exchange(other.id_,-1);
  handle_ = std::exchange(other.handle_,nullptr);
}

void GameCtrl::open(int id) noexcept {
  close();

  handle_ = SDL_GameControllerOpen(id);

  if(!handle_) {
    std::cerr << "[WARN] Failed to open Game Controller `" << id << "`: " << SDL_GetError() << '.'
              << std::endl;
    return;
  }

  id_ = id;
}

void GameCtrl::close() noexcept {
  if(handle_) {
    SDL_GameControllerClose(handle_);
    handle_ = nullptr;

    id_ = -1;
  }
}

GameCtrl::operator bool() const { return handle_ != nullptr; }

bool GameCtrl::matches(int id) const {
  return handle_ && id_ == id;
}

int GameCtrl::id() const { return id_; }

SDL_GameController* GameCtrl::handle() const { return handle_; }

} // namespace cybel
