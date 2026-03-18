/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "ui_texture.h"

namespace cybel {

UiTexture::UiTexture(const TextureRef& tex_ref) noexcept
  : tex_ref(tex_ref) {}

void UiTexture::draw(Renderer& ren) {
  const auto* tex = tex_ref.get();
  if(tex == nullptr) { return; }

  ren.wrap_tex(*tex,[&](auto& t) { t.draw_quad(pos_,size_); });
}

} // namespace cybel
