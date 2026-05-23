/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GFX_SPRITE_ATLAS_H_
#define CYBEL_GFX_SPRITE_ATLAS_H_

#include "cybel/common.h"

#include "cybel/asset/asset_man_key.h"
#include "cybel/gfx/texture.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

#include <functional>
#include <vector>

namespace cybel {

class SpriteAtlas {
public:
  struct Config final {
    Pos2i offset{};
    Size2i cell_size{};
    int cell_padding = 0;
    Size2i grid_size{};
  };

  explicit SpriteAtlas(AssetManKey,Texture& texture,const Config& config);
  virtual ~SpriteAtlas() noexcept = default;

  const Texture& texture() const;
  const Pos4f* src(std::size_t index) const;
  const Pos4f* src(const Pos2i& cell) const;
  const Size2i& cell_size() const;
  const Size2i& grid_size() const;
  std::size_t cell_count() const;

protected:
  std::reference_wrapper<Texture> texture_;
  Size2i cell_size_{};
  Size2i grid_size_{};
  std::vector<Pos4f> index_to_src_{};

  SpriteAtlas(const SpriteAtlas& other) = default;
  SpriteAtlas(SpriteAtlas&& other) noexcept = default;

  SpriteAtlas& operator=(const SpriteAtlas& other) = default;
  SpriteAtlas& operator=(SpriteAtlas&& other) noexcept = default;
};

} // namespace cybel
#endif
