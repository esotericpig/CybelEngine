/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "asset_loaders.h"

#include "cybel/asset/asset_man.h"

namespace cybel {

CpuGfxLoader::CpuGfxLoader(AssetManKey,AssetMan& asset_man) noexcept
  : asset_man_{asset_man} {}

GpuGfxLoader::GpuGfxLoader(AssetManKey,AssetMan& asset_man) noexcept
  : asset_man_{asset_man} {}

Texture& GpuGfxLoader::load_texture(const Image& image) {
  return asset_man_.load_ghost_gfx_asset<Texture>(image);
}

Texture& GpuGfxLoader::load_texture(const Color4f& color) {
  return asset_man_.load_ghost_gfx_asset<Texture>(color);
}

AudioLoader::AudioLoader(AssetManKey,AssetMan& asset_man) noexcept
  : asset_man_{asset_man} {}

} // namespace cybel
