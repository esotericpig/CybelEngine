/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_ASSET_ASSET_LOADER_H_
#define CYBEL_ASSET_ASSET_LOADER_H_

#include "cybel/common.h"

#include "cybel/asset/asset_loaders.h"
#include "cybel/asset/asset_man.h"

namespace cybel {

class AssetLoader {
public:
  explicit AssetLoader() = default;
  virtual ~AssetLoader() noexcept = default;

  virtual void load_cpu_gfx([[maybe_unused]] AssetMan& assets,[[maybe_unused]] CpuGfxLoader& gfx) {}
  virtual void load_gpu_gfx([[maybe_unused]] AssetMan& assets,[[maybe_unused]] GpuGfxLoader& gfx) {}
  virtual void load_audio([[maybe_unused]] AssetMan& assets,[[maybe_unused]] AudioLoader& audio) {}

protected:
  AssetLoader(const AssetLoader& other) = default;
  AssetLoader(AssetLoader&& other) noexcept = default;

  AssetLoader& operator=(const AssetLoader& other) = default;
  AssetLoader& operator=(AssetLoader&& other) noexcept = default;
};

} // namespace cybel
#endif
