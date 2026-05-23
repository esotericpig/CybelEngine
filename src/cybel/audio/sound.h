/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_AUDIO_SOUND_H_
#define CYBEL_AUDIO_SOUND_H_

#include "cybel/common.h"

#include <filesystem>

namespace cybel {

class AudioPlayer;

class Sound final {
public:
  explicit Sound(const std::filesystem::path& file);

  Sound(const Sound& other) = delete;
  Sound(Sound&& other) noexcept;
  ~Sound() noexcept;

  Sound& operator=(const Sound& other) = delete;
  Sound& operator=(Sound&& other) noexcept;

  friend class AudioPlayer;

private:
  Mix_Chunk* handle_ = nullptr;

  void move_from(Sound&& other) noexcept;
  void destroy() noexcept;
};

} // namespace cybel
#endif
