/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "sound.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

Sound::Sound(const std::filesystem::path& file) {
  const auto file_str = file.u8string();
  const auto* file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  handle_ = Mix_LoadWAV(file_cstr);

  if(!handle_) {
    throw CybelError{"Failed to load Sound `",file_cstr,"`: ",Util::get_sdl_mix_error(),'.'};
  }
}

Sound::Sound(Sound&& other) noexcept {
  move_from(std::move(other));
}

void Sound::move_from(Sound&& other) noexcept {
  destroy();

  handle_ = std::exchange(other.handle_,nullptr);
}

Sound::~Sound() noexcept {
  destroy();
}

void Sound::destroy() noexcept {
  if(handle_) {
    Mix_FreeChunk(handle_);
    handle_ = nullptr;
  }
}

Sound& Sound::operator=(Sound&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

} // namespace cybel
