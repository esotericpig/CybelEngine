/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "file_sys.h"

#include "cybel/util/util.h"

#include <cstdlib>
#include <exception>
#include <system_error>

namespace cybel {

std::filesystem::path FileSys::fetch_base_dir() {
  std::filesystem::path dir{"."}; // Fallback value.

  // On macOS, this is `Contents/Resources`.
  char* base_path = SDL_GetBasePath();

  if(base_path) {
    std::filesystem::path base_dir{base_path};

    SDL_free(base_path);
    base_path = nullptr;

    std::error_code ec{};

    if(is_directory(base_dir,ec)) {
      dir = base_dir;
    } else {
      std::cerr << "[WARN] The game's Base Path `" << base_dir << "` isn't a folder." << std::endl;
    }
  } else {
    std::cerr << "[WARN] Failed to get the game's Base Path: " + Util::get_sdl_error() + '.' << std::endl;
  }

  return dir;
}

std::filesystem::path FileSys::fetch_app_image_dir() {
  std::filesystem::path dir{"."}; // Fallback value.

  const char* app_image_path = std::getenv("APPIMAGE");

  if(app_image_path) {
    // parent_path() can throw exceptions.
    try {
      std::filesystem::path app_image_dir{app_image_path};
      app_image_dir = app_image_dir.parent_path();

      std::error_code ec{};

      if(is_directory(app_image_dir,ec)) {
        dir = app_image_dir;
      } else {
        std::cerr << "[WARN] AppImage Path `" << app_image_path << "` doesn't have a parent folder."
                  << std::endl;
      }
    } catch(const std::exception& e) {
      std::cerr << "[WARN] Failed to get parent path of AppImage Path `" << app_image_path << "`: "
                << e.what() << '.' << std::endl;
    }
  }

  return dir;
}

const std::filesystem::path& FileSys::base_dir() const { return base_dir_; }

const std::filesystem::path& FileSys::app_image_dir() const { return app_image_dir_; }

} // namespace cybel
