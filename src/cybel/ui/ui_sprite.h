/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_UI_UI_SPRITE_H_
#define CYBEL_UI_UI_SPRITE_H_

#include "cybel/common.h"

#include "cybel/asset/asset_types.h"
#include "cybel/ui/ui_box_node.h"

namespace cybel {

class UiSprite : public UiBoxNode {
public:
  asset_id_t id{};

  explicit UiSprite(asset_id_t id) noexcept;

  void draw(Renderer& ren) override;
};

} // namespace cybel
#endif
