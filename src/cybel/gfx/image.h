/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GFX_IMAGE_H_
#define CYBEL_GFX_IMAGE_H_

#include "cybel/common.h"

#include "cybel/asset/asset_man_key.h"
#include "cybel/types/color.h"
#include "cybel/types/size.h"

#include <filesystem>
#include <functional>

namespace cybel {

class CybelEngine;
class Texture;

class Image final {
public:
  using EditPixel = std::function<void(Color4f&)>;

  explicit Image(AssetManKey,const std::filesystem::path& file);
  explicit Image(const Size2i& size,const Color4f& color);

  Image(const Image& other) = delete;
  Image(Image&& other) noexcept;
  ~Image() noexcept;

  Image& operator=(const Image& other) = delete;
  Image& operator=(Image&& other) noexcept;

  Image dup() const;

  Image& make_weird();
  Image& colorize(const Color4f& to_color);
  Image& edit_pixels(const EditPixel& edit_pixel);

  void lock() const;
  void unlock() const noexcept;

  const std::string& id() const;
  const Size2i& size() const;
  std::uint8_t bytes_per_pixel() const;
  bool is_red_first() const;

  friend class CybelEngine; // For `handle_` in set_icon().
  friend class Texture; // For pixels() & gl_type() in load().

private:
  std::string id_{};
  SDL_Surface* handle_ = nullptr;
  Size2i size_{};
  mutable bool is_locked_ = false;

  explicit Image() noexcept = default;

  void move_from(Image&& other) noexcept;
  void destroy() noexcept;

  const void* pixels() const;
  GLenum gl_type() const;
};

} // namespace cybel
#endif
