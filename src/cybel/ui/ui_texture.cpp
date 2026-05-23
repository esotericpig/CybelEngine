/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "ui_texture.h"

namespace cybel {

UiTexture::UiTexture(asset_id_t id) noexcept
  : id{id} {}

void UiTexture::draw(Renderer& ren) {
  // const auto* tex = tex_ref.get();
  // if(tex == nullptr) { return; }
  //
  // ren.wrap_tex(*tex,[&](auto& t) { t.draw_quad(pos_,size_); });

  // TODO: Implement UiTexture.
  ren.begin_color(Color4f::kMagenta);
  ren.draw_quad(pos_,size_);
  ren.end_color();
}

} // namespace cybel
