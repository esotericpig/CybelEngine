/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "music.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

Music::Music(AssetManKey,const std::filesystem::path& file)
  : id_{file} {
  const auto file_str = file.u8string();
  const auto* file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  handle_ = Mix_LoadMUS(file_cstr);

  if(!handle_) {
    throw CybelError{"Failed to load Music file `",file_cstr,"`: ",Util::get_sdl_mix_error(),'.'};
  }
}

Music::Music(Music&& other) noexcept {
  move_from(std::move(other));
}

void Music::move_from(Music&& other) noexcept {
  destroy();

  id_ = std::exchange(other.id_,"");
  handle_ = std::exchange(other.handle_,nullptr);
}

Music::~Music() noexcept {
  destroy();
}

void Music::destroy() noexcept {
  if(handle_) {
    Mix_FreeMusic(handle_);
    handle_ = nullptr;
  }
}

Music& Music::operator=(Music&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

const std::string& Music::id() const { return id_; }

} // namespace cybel
