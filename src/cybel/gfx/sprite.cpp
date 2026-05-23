/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "sprite.h"

namespace cybel {

Pos4f Sprite::build_src(const Texture& texture,const Pos2i& offset,const Size2i& size,int padding) {
  // Must be at least 1 to prevent divides by 0.
  const auto tex_w = static_cast<float>(std::max(texture.size().w,1));
  const auto tex_h = static_cast<float>(std::max(texture.size().h,1));

  const auto src_x = static_cast<float>(offset.x + padding);
  const auto src_y = static_cast<float>(offset.y + padding);

  // calc_size() subtracts padding & ensures at least 1.
  const auto src_size = calc_size(texture,offset,size,padding);
  const auto src_w = static_cast<float>(src_size.w);
  const auto src_h = static_cast<float>(src_size.h);

  Pos4f src{};

  // Convert to be between [0.0,1.0].
  src.x1 = std::clamp(src_x / tex_w,0.0f,1.0f);
  src.x2 = std::clamp((src_x + src_w) / tex_w,0.0f,1.0f);
  src.y1 = std::clamp(src_y / tex_h,0.0f,1.0f);
  src.y2 = std::clamp((src_y + src_h) / tex_h,0.0f,1.0f);

  return src;
}

Size2i Sprite::calc_size(const Texture& texture,const Pos2i& offset,const Size2i& size,int padding) {
  const int w = (size.w > 0) ? size.w : (texture.size().w - offset.x);
  const int h = (size.h > 0) ? size.h : (texture.size().h - offset.y);
  const int p2 = padding * 2;

  return Size2i{std::max(w - p2,1),std::max(h - p2,1)};
}

Sprite::Sprite(AssetManKey key,Texture& texture)
  : Sprite{key,texture,Config{}} {}

Sprite::Sprite(AssetManKey,Texture& texture,const Config& config)
  : texture_{texture},
    size_{calc_size(texture,config.offset,config.size,config.padding)},
    src_{build_src(texture,config.offset,config.size,config.padding)} {}

const Texture& Sprite::texture() const { return texture_; }

const Pos4f& Sprite::src() const { return src_; }

const Size2i& Sprite::size() const { return size_; }

} // namespace cybel
