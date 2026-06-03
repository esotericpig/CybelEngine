/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_ASSET_ASSET_LOADER_KEY_H_
#define CYBEL_ASSET_ASSET_LOADER_KEY_H_

#include "cybel/common.h"

namespace cybel {

class CpuGfxLoader;
class GpuGfxLoader;
class AudioLoader;

/// Only the Asset Loaders can call the load functions in AssetMan.
class AssetLoaderKey final {
public:
  friend class CpuGfxLoader;
  friend class GpuGfxLoader;
  friend class AudioLoader;

private:
  explicit constexpr AssetLoaderKey() noexcept = default;
};

} // namespace cybel
#endif
