/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_AUDIO_SOUND_H_
#define CYBEL_AUDIO_SOUND_H_

#include "cybel/common.h"

#include "cybel/asset/asset_man_key.h"

#include <filesystem>

namespace cybel {

class Sound final {
public:
  explicit Sound(AssetManKey,const std::filesystem::path& file);
  ~Sound() noexcept;

  Sound(const Sound& other) = delete;
  Sound& operator=(const Sound& other) = delete;

  Sound(Sound&& other) noexcept;
  Sound& operator=(Sound&& other) noexcept;

  Mix_Chunk* handle() const;

private:
  Mix_Chunk* handle_ = nullptr;

  void destroy() noexcept;
  void move_from(Sound&& other) noexcept;
};

} // namespace cybel
#endif
