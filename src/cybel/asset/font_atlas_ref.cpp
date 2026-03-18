/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "font_atlas_ref.h"

#include "cybel/asset/asset_man.h"

namespace cybel {

FontAtlasRef::FontAtlasRef(AssetMan& asset_man,asset_id_t id) noexcept
  : asset_man_(&asset_man),id_(id) {}

FontAtlas* FontAtlasRef::get() { return asset_man_->font_atlas(id_); }

} // namespace cybel
