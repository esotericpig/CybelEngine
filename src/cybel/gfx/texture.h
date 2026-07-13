/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GFX_TEXTURE_H_
#define CYBEL_GFX_TEXTURE_H_

#include "cybel/common.h"

#include "cybel/asset/asset_man_key.h"
#include "cybel/gfx/image.h"
#include "cybel/types/color.h"
#include "cybel/types/size.h"

namespace cybel {

class Texture final {
public:
  /// Only supports images with pixel formats: RGB, RGBA, BGR, or BGRA.
  explicit Texture(AssetManKey,const Image& image);
  explicit Texture(AssetManKey,const Color4f& color);
  explicit Texture(AssetManKey,const Size2i& size,const Color4f& color);
  ~Texture() noexcept;

  Texture(const Texture& other) = delete;
  Texture& operator=(const Texture& other) = delete;

  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  /// For WebGL context lost/restored so that it won't try to delete the now invalid OpenGL handle.
  void zombify(AssetManKey) noexcept;

  const Size2i& size() const;

  GLuint handle() const;

private:
  GLuint handle_ = 0;
  Size2i size_{};

  void destroy() noexcept;
  void move_from(Texture&& other) noexcept;
};

} // namespace cybel
#endif
