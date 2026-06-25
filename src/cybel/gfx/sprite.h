/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GFX_SPRITE_H_
#define CYBEL_GFX_SPRITE_H_

#include "cybel/common.h"

#include "cybel/asset/asset_man_key.h"
#include "cybel/gfx/texture.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

namespace cybel {

class Sprite final {
public:
  struct Config final {
    Pos2i offset{};
    Size2i size{};
    int padding = 0;
  };

  static Pos4f build_src(const Size2i& texture_size,const Pos2i& offset,const Size2i& size,int padding);
  static Size2i calc_size(const Size2i& texture_size,const Pos2i& offset,const Size2i& size,int padding);

  explicit Sprite(AssetManKey,const Texture& texture);
  explicit Sprite(AssetManKey,const Texture& texture,const Config& config);

  const Size2i& size() const;

  const Pos4f& src() const;
  GLuint handle() const;

private:
  GLuint handle_ = 0;
  Size2i size_{};
  Pos4f src_{};
};

} // namespace cybel
#endif
