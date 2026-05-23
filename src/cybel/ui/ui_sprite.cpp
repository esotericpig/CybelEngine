/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "ui_sprite.h"

namespace cybel {

UiSprite::UiSprite(asset_id_t id) noexcept
  : id{id} {}

void UiSprite::draw(Renderer& ren) {
  // const auto* sprite = sprite_ref.get();
  // if(sprite == nullptr) { return; }
  //
  // ren.wrap_sprite(*sprite,[&](auto& t) { t.draw_quad(pos_,size_); });

  // TODO: Implement UiSprite.
  ren.begin_color(Color4f::kMagenta);
  ren.draw_quad(pos_,size_);
  ren.end_color();
}

} // namespace cybel
