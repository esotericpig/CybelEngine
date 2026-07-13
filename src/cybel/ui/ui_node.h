/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_UI_UI_NODE_H_
#define CYBEL_UI_UI_NODE_H_

#include "cybel/common.h"

#include "cybel/gfx/renderer.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

namespace cybel {

class UiNode {
public:
  explicit UiNode() = default;
  virtual ~UiNode() noexcept = default;

  virtual void resize(const Pos3i& pos,const Size2i& size);
  void resize(const Size2i& size) { resize(Pos3i{},size); }
  virtual void draw(Renderer& ren);

protected:
  UiNode(const UiNode& other) = default;
  UiNode& operator=(const UiNode& other) = default;

  UiNode(UiNode&& other) noexcept = default;
  UiNode& operator=(UiNode&& other) noexcept = default;
};

inline auto kUiSpacer = std::make_shared<UiNode>();

inline void UiNode::resize(const Pos3i&,const Size2i&) {}

inline void UiNode::draw(Renderer&) {}

} // namespace cybel
#endif
