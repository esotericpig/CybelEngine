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

class Image final {
public:
  class PixelsGuard final {
  public:
    PixelsGuard(const PixelsGuard& other) = default;
    PixelsGuard(PixelsGuard&& other) noexcept = default;
    ~PixelsGuard() noexcept;

    PixelsGuard& operator=(const PixelsGuard& other) = default;
    PixelsGuard& operator=(PixelsGuard&& other) noexcept = default;

    const void* pixels() const;

    friend class Image;

  private:
    std::reference_wrapper<const Image> image_;

    explicit PixelsGuard(const Image& image);
  };

  using EditPixel = std::function<void(Color4f& color)>;

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

  const std::string& id() const;
  const Size2i& size() const;
  std::uint8_t bytes_per_pixel() const;
  bool is_red_first() const;

  [[nodiscard]] PixelsGuard lock_pixels() const;
  SDL_Surface* handle() const;

private:
  std::string id_{};
  SDL_Surface* handle_ = nullptr;
  Size2i size_{};
  mutable bool is_locked_ = false;

  explicit Image() noexcept = default;

  void move_from(Image&& other) noexcept;
  void destroy() noexcept;

  void lock() const;
  void unlock() const noexcept;

  const void* pixels() const;
};

} // namespace cybel
#endif
