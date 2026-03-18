/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "ui_box_node.h"

namespace cybel {

void UiBoxNode::resize(const Pos3i& pos,const Size2i& size) {
  pos_ = pos;
  size_ = size;
}

} // namespace cybel
