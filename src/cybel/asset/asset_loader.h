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

  virtual void load_cpu_gfx(AssetMan& assets,CpuGfxLoader& gfx);
  virtual void load_gpu_gfx(AssetMan& assets,GpuGfxLoader& gfx);
  virtual void load_audio(AssetMan& assets,AudioLoader& audio);

protected:
  AssetLoader(const AssetLoader& other) = default;
  AssetLoader& operator=(const AssetLoader& other) = default;

  AssetLoader(AssetLoader&& other) noexcept = default;
  AssetLoader& operator=(AssetLoader&& other) noexcept = default;
};

inline void AssetLoader::load_cpu_gfx(AssetMan&,CpuGfxLoader&) {}

inline void AssetLoader::load_gpu_gfx(AssetMan&,GpuGfxLoader&) {}

inline void AssetLoader::load_audio(AssetMan&,AudioLoader&) {}

} // namespace cybel
#endif
