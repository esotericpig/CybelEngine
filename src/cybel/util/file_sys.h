/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_UTIL_FILE_SYS_H_
#define CYBEL_UTIL_FILE_SYS_H_

#include "cybel/common.h"

#include <filesystem>

namespace cybel {

class CybelEngine;

class FileSys final {
public:
  const std::filesystem::path& base_dir() const;
  const std::filesystem::path& app_image_dir() const;

  friend class CybelEngine;

private:
  std::filesystem::path base_dir_ = fetch_base_dir();
  std::filesystem::path app_image_dir_ = fetch_app_image_dir();

  explicit FileSys() = default;

  /// NOTE: This should only ever be called once, since it uses SDL_GetBasePath()
  ///       which is an expensive operation, and it must be called after SDL_Init()
  ///       on some platforms.
  std::filesystem::path fetch_base_dir();
  std::filesystem::path fetch_app_image_dir();
};

} // namespace cybel
#endif
