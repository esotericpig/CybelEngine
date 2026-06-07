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

void AssetMan::add_asset_dir(const std::filesystem::path& dir) {
  std::error_code ec{};
  // NOTE: Don't use canonical() in case the dir is created later during refresh.
  //       Also, using `/ ""` to add a trailing slash so that dirs equal.
  const auto abs_dir = absolute(dir / "",ec).lexically_normal();

  if(ec != std::error_code{}) {
    std::cerr << "[WARN] Invalid Asset folder `" << dir << "` => `" << abs_dir << '`'
              << "; error [" << ec << "]: " << ec.message() << '.' << std::endl;
    return;
  }

  // Already added? (Slow, but fine, as we want insertion order.)
  if(std::ranges::find(asset_dirs_,abs_dir) != asset_dirs_.cend()) { return; }

  asset_dirs_.push_back(abs_dir);
}

void AssetMan::add_asset_dirs(std::initializer_list<std::filesystem::path> dirs,
                              const std::filesystem::path& sub_dir) {
  for(const auto& dir: dirs) {
    add_asset_dir(dir / sub_dir);
  }
}

void AssetMan::load_assets(std::shared_ptr<AssetLoader> asset_loader) {
  if(asset_loader_) { throw CybelError{"Multiple Asset Loaders are not currently supported."}; }

  if(!asset_loader) {
    // This Game doesn't use assets.
    shrink_assets();
    return;
  }

  asset_loader_ = std::move(asset_loader);
  reload_assets();
}

void AssetMan::reload_assets() {
  reload_gfx();
  reload_audio();
}

void AssetMan::shrink_assets() {
  std::apply(
    [](auto&... asset_bag) {
      (asset_bag.shrink(),...);
    },
    asset_bags_
  );
  std::apply(
    [](auto&... asset_bag) {
      (asset_bag.shrink(),...);
    },
    ghost_asset_bags_
  );
}

void AssetMan::reload_gfx() {
  reload_cpu_gfx();
  reload_gpu_gfx();
}

void AssetMan::reload_cpu_gfx() {
  if(!asset_loader_) {
    shrink_assets<Image>(asset_bags_);
    return;
  }

  reset_assets<Image>(asset_bags_);

  CpuGfxLoader loader{AssetManKey{},*this};
  asset_loader_->load_cpu_gfx(*this,loader);

  shrink_assets<Image>(asset_bags_);
  check_assets<Image>(asset_bags_);
}

void AssetMan::reload_gpu_gfx() {
  if(!asset_loader_) {
    shrink_assets<Texture,Sprite,SpriteAtlas,FontAtlas>(asset_bags_);
    shrink_assets<Texture>(ghost_asset_bags_);
    return;
  }

  reset_assets<Texture,Sprite,SpriteAtlas,FontAtlas>(asset_bags_);
  reset_assets<Texture>(ghost_asset_bags_);

  GpuGfxLoader loader{AssetManKey{},*this};
  asset_loader_->load_gpu_gfx(*this,loader);

  shrink_assets<Texture,Sprite,SpriteAtlas,FontAtlas>(asset_bags_);
  shrink_assets<Texture>(ghost_asset_bags_);

  check_assets<Texture,Sprite,SpriteAtlas,FontAtlas>(asset_bags_);
  check_assets<Texture>(ghost_asset_bags_);
}

void AssetMan::reload_audio() {
  if(!asset_loader_ || !is_audio_alive_) {
    shrink_assets<Music,Sound>(asset_bags_);
    return;
  }

  reset_assets<Music,Sound>(asset_bags_);

  AudioLoader loader{AssetManKey{},*this};
  asset_loader_->load_audio(*this,loader);

  shrink_assets<Music,Sound>(asset_bags_);
  check_assets<Music,Sound>(asset_bags_);
}

void AssetMan::on_gpu_context_loss(GpuContextKey) {
  zombify_assets<Texture>(ghost_asset_bags_);
  zombify_assets<Texture>(asset_bags_);

  missing_texture_.zombify(AssetManKey{});
}

void AssetMan::on_gpu_context_restore(GpuContextKey) {
  missing_texture_ = Texture{AssetManKey{},missing_image_};

  reload_gpu_gfx();
}

void AssetMan::set_fail_on_audio_error(bool fail) { fail_on_audio_error_ = fail; }

const std::vector<std::filesystem::path>& AssetMan::asset_dirs() const { return asset_dirs_; }

} // namespace cybel
