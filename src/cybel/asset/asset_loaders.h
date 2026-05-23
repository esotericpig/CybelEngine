/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_ASSET_ASSET_LOADERS_H_
#define CYBEL_ASSET_ASSET_LOADERS_H_

#include "cybel/common.h"

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

  template <AssetIdLike T>
  Image& load_image(T id,const std::filesystem::path& file);
  template <AssetIdLike T>
  Image& load_image(T id,const Size2i& size,const Color4f& color);
  template <AssetIdLike T>
  Image& load_image(T id,const Image& other);

private:
  AssetMan& asset_man_;
};

class GpuGfxLoader final {
public:
  explicit GpuGfxLoader(AssetManKey,AssetMan& asset_man) noexcept;

  template <AssetIdLike T>
  Texture& load_texture(T id,const Image& image);
  template <AssetIdLike T>
  Texture& load_texture(T id,const Color4f& color);
  Texture& load_texture(const Image& image);
  Texture& load_texture(const Color4f& color);

  template <AssetIdLike T>
  Sprite& load_sprite(T id,Texture& texture);
  template <AssetIdLike T>
  Sprite& load_sprite(T id,Texture& texture,const Sprite::Config& config);
  template <AssetIdLike T>
  SpriteAtlas& load_sprite_atlas(T id,Texture& texture,const SpriteAtlas::Config& config);
  template <AssetIdLike T>
  FontAtlas& load_font_atlas(T id,Texture& texture,const FontAtlas::Config& config);

private:
  AssetMan& asset_man_;
};

class AudioLoader final {
public:
  explicit AudioLoader(AssetManKey,AssetMan& asset_man) noexcept;

  template <AssetIdLike T>
  Music* load_music(T id,const std::filesystem::path& file);
  template <AssetIdLike T>
  Sound* load_sound(T id,const std::filesystem::path& file);

private:
  AssetMan& asset_man_;
};

template <AssetIdLike T>
Image& CpuGfxLoader::load_image(T id,const std::filesystem::path& file) {
  return asset_man_.load_gfx_asset<Image>(static_cast<asset_id_t>(id),file);
}

template <AssetIdLike T>
Image& CpuGfxLoader::load_image(T id,const Size2i& size,const Color4f& color) {
  return asset_man_.load_gfx_asset<Image>(static_cast<asset_id_t>(id),size,color);
}

template <AssetIdLike T>
Image& CpuGfxLoader::load_image(T id,const Image& other) {
  return asset_man_.load_gfx_asset<Image>(static_cast<asset_id_t>(id),other.dup());
}

template <AssetIdLike T>
Texture& GpuGfxLoader::load_texture(T id,const Image& image) {
  return asset_man_.load_gfx_asset<Texture>(static_cast<asset_id_t>(id),image);
}

template <AssetIdLike T>
Texture& GpuGfxLoader::load_texture(T id,const Color4f& color) {
  return asset_man_.load_gfx_asset<Texture>(static_cast<asset_id_t>(id),color);
}

template <AssetIdLike T>
Sprite& GpuGfxLoader::load_sprite(T id,Texture& texture) {
  return asset_man_.load_gfx_asset<Sprite>(static_cast<asset_id_t>(id),texture);
}

template <AssetIdLike T>
Sprite& GpuGfxLoader::load_sprite(T id,Texture& texture,const Sprite::Config& config) {
  return asset_man_.load_gfx_asset<Sprite>(static_cast<asset_id_t>(id),texture,config);
}

template <AssetIdLike T>
SpriteAtlas& GpuGfxLoader::load_sprite_atlas(T id,Texture& texture,const SpriteAtlas::Config& config) {
  return asset_man_.load_gfx_asset<SpriteAtlas>(static_cast<asset_id_t>(id),texture,config);
}

template <AssetIdLike T>
FontAtlas& GpuGfxLoader::load_font_atlas(T id,Texture& texture,const FontAtlas::Config& config) {
  return asset_man_.load_gfx_asset<FontAtlas>(static_cast<asset_id_t>(id),texture,config);
}

template <AssetIdLike T>
Music* AudioLoader::load_music(T id,const std::filesystem::path& file) {
  return asset_man_.load_audio_asset<Music>(static_cast<asset_id_t>(id),file);
}

template <AssetIdLike T>
Sound* AudioLoader::load_sound(T id,const std::filesystem::path& file) {
  return asset_man_.load_audio_asset<Sound>(static_cast<asset_id_t>(id),file);
}

} // namespace cybel
#endif
