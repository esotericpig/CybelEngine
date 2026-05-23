/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GFX_FONT_ATLAS_H_
#define CYBEL_GFX_FONT_ATLAS_H_

#include "cybel/common.h"

#include "cybel/asset/asset_man_key.h"
#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

#include <unordered_map>

namespace cybel {

class FontAtlas final : public SpriteAtlas {
public:
  class Config final {
  public:
    Config& offset(int x,int y);
    Config& cell_size(int width,int height);
    Config& cell_padding(int padding);
    Config& grid_size(int cols,int rows);

    Config& spacing(int rune_spacing,int line_spacing);
    Config& default_index(std::size_t index);
    Config& default_cell(int col,int row);
    Config& default_rune(char32_t rune);
    Config& index_to_rune(std::string_view str);
    Config& index_to_rune(std::initializer_list<std::string_view> lines);

    friend class FontAtlas;

  private:
    SpriteAtlas::Config sprite_atlas_{};
    Size2i spacing_{};

    // Since we don't know the final grid width & height until build,
    // we have to store both `default_index_` & `default_cell_`.
    std::size_t default_index_ = 0;
    Pos2i default_cell_{};
    char32_t default_rune_ = 0;

    std::unordered_map<char32_t,std::size_t> rune_to_index_{};
  };

  explicit FontAtlas(AssetManKey,Texture& texture,const Config& config);

  const Size2i& spacing() const;
  std::size_t rune_index(char32_t rune) const;

protected:
  Size2i spacing_{};
  std::size_t default_index_ = 0;
  std::unordered_map<char32_t,std::size_t> rune_to_index_{};
};

} // namespace cybel
#endif
