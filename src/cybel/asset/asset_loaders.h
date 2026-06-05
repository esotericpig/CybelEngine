/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_ASSET_ASSET_LOADERS_H_
#define CYBEL_ASSET_ASSET_LOADERS_H_

#include "cybel/common.h"

#include "cybel/asset/asset_loader_key.h"
#include "cybel/asset/asset_man.h"
#include "cybel/asset/asset_man_key.h"
#include "cybel/asset/asset_types.h"
#include "cybel/audio/music.h"
#include "cybel/audio/sound.h"
#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"

namespace cybel {

// NOTE: Decided not to use forwarding template funcs because
//       IDEs can't show the actual (non-template) params when using the funcs.

class CpuGfxLoader final {
public:
  explicit CpuGfxLoader(AssetManKey,AssetMan& asset_man) noexcept;

  template <AssetIdLike Id>
  Image& load_image(Id id,const std::filesystem::path& file);
  template <AssetIdLike Id>
  Image& load_image(Id id,const Size2i& size,const Color4f& color);
  template <AssetIdLike Id>
  Image& load_image(Id id,const Image& other);

private:
  AssetMan& asset_man_;
};

class GpuGfxLoader final {
public:
  explicit GpuGfxLoader(AssetManKey,AssetMan& asset_man) noexcept;

  Image load_image(const std::filesystem::path& file);
  Image load_image(const Size2i& size,const Color4f& color);
  Image load_image(const Image& other);

  template <AssetIdLike Id>
  Texture& load_texture(Id id,const Image& image);
  template <AssetIdLike Id>
  Texture& load_texture(Id id,const Color4f& color);
  Texture& load_texture(const Image& image);
  Texture& load_texture(const Color4f& color);

  template <AssetIdLike Id>
  Sprite& load_sprite(Id id,Texture& texture);
  template <AssetIdLike Id>
  Sprite& load_sprite(Id id,Texture& texture,const Sprite::Config& config);
  template <AssetIdLike Id>
  SpriteAtlas& load_sprite_atlas(Id id,Texture& texture,const SpriteAtlas::Config& config);
  template <AssetIdLike Id>
  FontAtlas& load_font_atlas(Id id,Texture& texture,const FontAtlas::Config& config);

private:
  AssetMan& asset_man_;
};

class AudioLoader final {
public:
  explicit AudioLoader(AssetManKey,AssetMan& asset_man) noexcept;

  template <AssetIdLike Id>
  Music* load_music(Id id,const std::filesystem::path& file);
  template <AssetIdLike Id>
  Sound* load_sound(Id id,const std::filesystem::path& file);

private:
  AssetMan& asset_man_;
};

template <AssetIdLike Id>
Image& CpuGfxLoader::load_image(Id id,const std::filesystem::path& file) {
  return asset_man_.load_gfx_asset<Image,Id>(AssetLoaderKey{},id,file);
}

template <AssetIdLike Id>
Image& CpuGfxLoader::load_image(Id id,const Size2i& size,const Color4f& color) {
  return asset_man_.load_gfx_asset<Image,Id>(AssetLoaderKey{},id,size,color);
}

template <AssetIdLike Id>
Image& CpuGfxLoader::load_image(Id id,const Image& other) {
  return asset_man_.load_gfx_asset<Image,Id>(AssetLoaderKey{},id,other.dup());
}

template <AssetIdLike Id>
Texture& GpuGfxLoader::load_texture(Id id,const Image& image) {
  return asset_man_.load_gfx_asset<Texture,Id>(AssetLoaderKey{},id,image);
}

template <AssetIdLike Id>
Texture& GpuGfxLoader::load_texture(Id id,const Color4f& color) {
  return asset_man_.load_gfx_asset<Texture,Id>(AssetLoaderKey{},id,color);
}

template <AssetIdLike Id>
Sprite& GpuGfxLoader::load_sprite(Id id,Texture& texture) {
  return asset_man_.load_gfx_asset<Sprite,Id>(AssetLoaderKey{},id,texture);
}

template <AssetIdLike Id>
Sprite& GpuGfxLoader::load_sprite(Id id,Texture& texture,const Sprite::Config& config) {
  return asset_man_.load_gfx_asset<Sprite,Id>(AssetLoaderKey{},id,texture,config);
}

template <AssetIdLike Id>
SpriteAtlas& GpuGfxLoader::load_sprite_atlas(Id id,Texture& texture,const SpriteAtlas::Config& config) {
  return asset_man_.load_gfx_asset<SpriteAtlas,Id>(AssetLoaderKey{},id,texture,config);
}

template <AssetIdLike Id>
FontAtlas& GpuGfxLoader::load_font_atlas(Id id,Texture& texture,const FontAtlas::Config& config) {
  return asset_man_.load_gfx_asset<FontAtlas,Id>(AssetLoaderKey{},id,texture,config);
}

template <AssetIdLike Id>
Music* AudioLoader::load_music(Id id,const std::filesystem::path& file) {
  return asset_man_.load_audio_asset<Music,Id>(AssetLoaderKey{},id,file);
}

template <AssetIdLike Id>
Sound* AudioLoader::load_sound(Id id,const std::filesystem::path& file) {
  return asset_man_.load_audio_asset<Sound,Id>(AssetLoaderKey{},id,file);
}

} // namespace cybel
#endif
