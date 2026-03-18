/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "ui_quad.h"

namespace cybel {

UiQuad::UiQuad(const Color4f& color) noexcept
  : color(color) {}

void UiQuad::draw(Renderer& ren) {
  ren.wrap_color(color,[&] { ren.draw_quad(pos_,size_); });
}

} // namespace cybel
