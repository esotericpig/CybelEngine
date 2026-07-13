/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_AUDIO_MUSIC_H_
#define CYBEL_AUDIO_MUSIC_H_

#include "cybel/common.h"

#include "cybel/asset/asset_man_key.h"
#include "cybel/types/duration.h"

#include <filesystem>

namespace cybel {

class Music final {
public:
  explicit Music(AssetManKey,const std::filesystem::path& file);
  ~Music() noexcept;

  Music(const Music& other) = delete;
  Music& operator=(const Music& other) = delete;

  Music(Music&& other) noexcept;
  Music& operator=(Music&& other) noexcept;

  const std::string& id() const;
  Duration fetch_duration(const Duration& fallback = Duration::kZero) const;

  Mix_Music* handle() const;

private:
  std::string id_{};
  Mix_Music* handle_ = nullptr;

  void destroy() noexcept;
  void move_from(Music&& other) noexcept;
};

} // namespace cybel
#endif
