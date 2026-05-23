/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "asset_man.h"

#include "cybel/asset/asset_loader.h"
#include "cybel/asset/asset_loaders.h"

namespace cybel {

AssetMan::AssetMan(bool is_audio_alive)
  : is_audio_alive_{is_audio_alive} {}

void AssetMan::load_assets(std::shared_ptr<AssetLoader> asset_loader) {
  if(asset_loader_) { throw CybelError{"Multiple Asset Loaders are not currently supported."}; }
  if(!asset_loader) { return; }

  asset_loader_ = std::move(asset_loader);
  reload_assets();
}

void AssetMan::reload_assets() {
  reload_gfx();
  reload_audio();
}

void AssetMan::reload_gfx() {
  reload_cpu_gfx();
  reload_gpu_gfx();
}

void AssetMan::reload_cpu_gfx() {
  if(!asset_loader_) { return; }

  reset_assets<Image>(gfx_asset_bags_);

  CpuGfxLoader loader{AssetManKey{},*this};
  asset_loader_->load_cpu_gfx(*this,loader);

  shrink_assets<Image>(gfx_asset_bags_);
  check_assets<Image>(gfx_asset_bags_);
}

void AssetMan::reload_gpu_gfx() {
  if(!asset_loader_) { return; }

  reset_assets<Texture,Sprite,SpriteAtlas,FontAtlas>(gfx_asset_bags_);
  reset_ghost_assets<Texture>(ghost_gfx_asset_bags_);

  GpuGfxLoader loader{AssetManKey{},*this};
  asset_loader_->load_gpu_gfx(*this,loader);

  shrink_assets<Texture,Sprite,SpriteAtlas,FontAtlas>(gfx_asset_bags_);
  shrink_ghost_assets<Texture>(ghost_gfx_asset_bags_);

  check_assets<Texture,Sprite,SpriteAtlas,FontAtlas>(gfx_asset_bags_);
  check_assets<Texture>(ghost_gfx_asset_bags_);
}

void AssetMan::reload_audio() {
  if(!asset_loader_) { return; }

  reset_assets<Music,Sound>(audio_asset_bags_);

  AudioLoader loader{AssetManKey{},*this};
  asset_loader_->load_audio(*this,loader);

  shrink_assets<Music,Sound>(audio_asset_bags_);
  check_assets<Music,Sound>(audio_asset_bags_);
}

void AssetMan::on_gpu_context_loss(AssetManKey key) {
  for(auto& asset_ptr : std::get<AssetBag<Texture>>(gfx_asset_bags_).assets) {
    if(asset_ptr) { asset_ptr->zombify(key); }
  }
  for(auto& asset_ptr : std::get<AssetBag<Texture>>(ghost_gfx_asset_bags_).assets) {
    if(asset_ptr) { asset_ptr->zombify(key); }
  }

  missing_texture_.zombify(key);
}

void AssetMan::on_gpu_context_restore(AssetManKey) {
  missing_texture_ = Texture{AssetManKey{},missing_image_};

  reload_gpu_gfx();
}

} // namespace cybel
