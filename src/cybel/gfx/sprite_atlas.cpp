/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "sprite_atlas.h"

#include "cybel/gfx/sprite.h"

namespace cybel {

SpriteAtlas::SpriteAtlas(AssetManKey,const Texture& texture,const Config& config)
  : handle_{texture.handle()},
    cell_size_{Sprite::calc_size(texture.size(),config.offset,config.cell_size,config.cell_padding)},
    grid_size_{std::max(config.grid_size.w,1),std::max(config.grid_size.h,1)},
    index_to_src_(static_cast<std::size_t>(grid_size_.area()),Pos4f{}) {
  // Include padding for cell offset math with col & row.
  Size2i full_cell_size{std::max(config.cell_size.w,1),std::max(config.cell_size.h,1)};

  for(std::size_t i = 0; i < index_to_src_.size(); ++i) {
    const int col = static_cast<int>(i) % grid_size_.w;
    const int row = static_cast<int>(i) / grid_size_.w;
    const Pos2i cell_offset{
      config.offset.x + (full_cell_size.w * col),
      config.offset.y + (full_cell_size.h * row)
    };

    index_to_src_[i] = Sprite::build_src(texture.size(),cell_offset,full_cell_size,config.cell_padding);
  }
}

const Size2i& SpriteAtlas::cell_size() const { return cell_size_; }

const Size2i& SpriteAtlas::grid_size() const { return grid_size_; }

std::size_t SpriteAtlas::cell_count() const { return index_to_src_.size(); }

const Pos4f* SpriteAtlas::src(std::size_t index) const {
  if(index >= index_to_src_.size()) [[unlikely]] { return nullptr; }

  return &index_to_src_[index];
}

const Pos4f* SpriteAtlas::src(const Pos2i& cell) const {
  return src(static_cast<std::size_t>(cell.x + (cell.y * grid_size_.w)));
}

GLuint SpriteAtlas::handle() const { return handle_; }

} // namespace cybel
