/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "font_atlas.h"

#include "cybel/str/utf8/rune_range.h"

namespace cybel {

FontAtlas::FontAtlas(AssetManKey key,Texture& texture,const Config& config)
  : SpriteAtlas{key,texture,config.sprite_atlas_},
    spacing_{config.spacing_},
    rune_to_index_{config.rune_to_index_} {
  if(config.default_index_ > 0) {
    default_index_ = config.default_index_;
  } else if(config.default_cell_.x > 0 || config.default_cell_.y > 0) {
    default_index_ = static_cast<std::size_t>(
      config.default_cell_.x + (config.default_cell_.y * grid_size_.w)
    );
  } else if(config.default_rune_ != 0) {
    for(auto [rune,index] : rune_to_index_) {
      if(rune == config.default_rune_) {
        default_index_ = index;
        break;
      }
    }
  }

  if(default_index_ >= rune_to_index_.size()) {
    default_index_ = rune_to_index_.empty() ? 0 : (rune_to_index_.size() - 1);
  }
}

const Size2i& FontAtlas::spacing() const { return spacing_; }

std::size_t FontAtlas::rune_index(char32_t rune) const {
  const auto it = rune_to_index_.find(rune);
  if(it == rune_to_index_.end()) [[unlikely]] { return default_index_; }

  return it->second;
}

FontAtlas::Config& FontAtlas::Config::offset(int x,int y) {
  sprite_atlas_.offset.x = x;
  sprite_atlas_.offset.y = y;

  return *this;
}

FontAtlas::Config& FontAtlas::Config::cell_size(int width,int height) {
  sprite_atlas_.cell_size.w = width;
  sprite_atlas_.cell_size.h = height;

  return *this;
}

FontAtlas::Config& FontAtlas::Config::cell_padding(int padding) {
  sprite_atlas_.cell_padding = padding;

  return *this;
}

FontAtlas::Config& FontAtlas::Config::grid_size(int cols,int rows) {
  sprite_atlas_.grid_size.w = cols;
  sprite_atlas_.grid_size.h = rows;

  return *this;
}

FontAtlas::Config& FontAtlas::Config::spacing(int rune_spacing,int line_spacing) {
  spacing_.w = rune_spacing;
  spacing_.h = line_spacing;

  return *this;
}

FontAtlas::Config& FontAtlas::Config::default_index(std::size_t index) {
  default_index_ = index;
  default_cell_.x = 0;
  default_cell_.y = 0;
  default_rune_ = 0;

  return *this;
}

FontAtlas::Config& FontAtlas::Config::default_cell(int col,int row) {
  default_index_ = 0;
  default_cell_.x = col;
  default_cell_.y = row;
  default_rune_ = 0;

  return *this;
}

FontAtlas::Config& FontAtlas::Config::default_rune(char32_t rune) {
  default_index_ = 0;
  default_cell_.x = 0;
  default_cell_.y = 0;
  default_rune_ = rune;

  return *this;
}

FontAtlas::Config& FontAtlas::Config::index_to_rune(std::string_view str) {
  std::size_t index = 0;

  for(auto rune : utf8::RuneRange{str}) {
    rune_to_index_[rune] = index;
    ++index;
  }

  return *this;
}

FontAtlas::Config& FontAtlas::Config::index_to_rune(std::initializer_list<std::string_view> lines) {
  std::size_t index = 0;
  int col_count = 0;

  for(const auto& line : lines) {
    int len = 0;

    for(auto rune : utf8::RuneRange{line}) {
      rune_to_index_[rune] = index;
      ++index;
      ++len;
    }

    if(len > col_count) { col_count = len; }
  }

  auto& grid_size = sprite_atlas_.grid_size;

  if(grid_size.w <= 0) { grid_size.w = col_count; }
  if(grid_size.h <= 0) { grid_size.h = static_cast<int>(lines.size()); }

  return *this;
}

} // namespace cybel
