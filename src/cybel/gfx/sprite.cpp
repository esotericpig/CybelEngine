/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "sprite.h"

#include <algorithm>

namespace cybel {

Pos4f Sprite::build_src(const Size2i& texture_size,const Pos2i& offset,const Size2i& size,int padding) {
  // Must be at least 1 to prevent divides by 0.
  const auto tex_w = static_cast<float>(std::max(texture_size.w,1));
  const auto tex_h = static_cast<float>(std::max(texture_size.h,1));

  const auto src_x = static_cast<float>(offset.x + padding);
  const auto src_y = static_cast<float>(offset.y + padding);

  // calc_size() subtracts padding & ensures at least 1.
  const auto src_size = calc_size(texture_size,offset,size,padding);
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

Size2i Sprite::calc_size(const Size2i& texture_size,const Pos2i& offset,const Size2i& size,int padding) {
  const int w = (size.w > 0) ? size.w : (texture_size.w - offset.x);
  const int h = (size.h > 0) ? size.h : (texture_size.h - offset.y);
  const int p2 = padding * 2;

  return Size2i{std::max(w - p2,1),std::max(h - p2,1)};
}

Sprite::Sprite(AssetManKey key,const Texture& texture)
  : Sprite{key,texture,Config{}} {}

Sprite::Sprite(AssetManKey,const Texture& texture,const Config& config)
  : handle_{texture.handle()},
    size_{calc_size(texture.size(),config.offset,config.size,config.padding)},
    src_{build_src(texture.size(),config.offset,config.size,config.padding)} {}

const Size2i& Sprite::size() const { return size_; }

const Pos4f& Sprite::src() const { return src_; }

GLuint Sprite::handle() const { return handle_; }

} // namespace cybel
