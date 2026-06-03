/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GFX_GPU_CONTEXT_KEY_H_
#define CYBEL_GFX_GPU_CONTEXT_KEY_H_

#include "cybel/common.h"

namespace cybel {

class CybelEngine;

/// Only CybelEngine can call AssetMan/Renderer.on_gpu_context_loss/restore().
class GpuContextKey final {
public:
  friend class CybelEngine;

private:
  explicit constexpr GpuContextKey() noexcept = default;
};

} // namespace cybel
#endif
