/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_ASSET_ASSET_MAN_KEY_H_
#define CYBEL_ASSET_ASSET_MAN_KEY_H_

#include "cybel/common.h"

namespace cybel {

class AssetMan;
class CybelEngine;

/// Prevents loading certain assets outside of AssetMan/AssetLoader.
class AssetManKey final {
public:
  friend class AssetMan;
  friend class CybelEngine; // For AssetMan/Renderer.on_gpu_context_loss/restore().

private:
  explicit constexpr AssetManKey() noexcept = default;
};

} // namespace cybel
#endif
