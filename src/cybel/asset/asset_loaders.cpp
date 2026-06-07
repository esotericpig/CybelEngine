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

Image GpuGfxLoader::load_image(const std::filesystem::path& file) {
  return asset_man_.load_temp_gfx_asset<Image>(AssetLoaderKey{},file);
}

Image GpuGfxLoader::load_image(const Size2i& size,const Color4f& color) {
  return Image{size,color};
}

Image GpuGfxLoader::load_image(const Image& other) {
  return other.dup();
}

Texture& GpuGfxLoader::load_texture(const Image& image) {
  return asset_man_.load_ghost_gfx_asset<Texture>(AssetLoaderKey{},image);
}

Texture& GpuGfxLoader::load_texture(const Color4f& color) {
  return asset_man_.load_ghost_gfx_asset<Texture>(AssetLoaderKey{},color);
}

Texture& GpuGfxLoader::load_texture(const Size2i& size,const Color4f& color) {
  return asset_man_.load_ghost_gfx_asset<Texture>(AssetLoaderKey{},size,color);
}

AudioLoader::AudioLoader(AssetManKey,AssetMan& asset_man) noexcept
  : asset_man_{asset_man} {}

} // namespace cybel
