/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "sprite_ref.h"

#include "cybel/asset/asset_man.h"

namespace cybel {

SpriteRef::SpriteRef(AssetMan& asset_man,asset_id_t id) noexcept
  : asset_man_(&asset_man),id_(id) {}

Sprite* SpriteRef::get() { return asset_man_->sprite(id_); }

} // namespace cybel
