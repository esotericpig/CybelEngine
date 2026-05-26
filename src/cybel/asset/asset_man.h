/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_ASSET_ASSET_MAN_H_
#define CYBEL_ASSET_ASSET_MAN_H_

#include "cybel/common.h"

#include "cybel/asset/asset_man_key.h"
#include "cybel/asset/asset_types.h"
#include "cybel/audio/music.h"
#include "cybel/audio/sound.h"
#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"
#include "cybel/types/color.h"
#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

#include <algorithm>
#include <concepts>
#include <tuple>
#include <vector>

namespace cybel {

class AssetLoader;
class CpuGfxLoader;
class GpuGfxLoader;
class AudioLoader;

class AssetMan final {
public:
  explicit AssetMan(bool is_audio_alive);

  void load_assets(std::shared_ptr<AssetLoader> asset_loader);
  void reload_assets();

  void reload_gfx();
  void reload_cpu_gfx();
  void reload_gpu_gfx();

  void reload_audio();

  void on_gpu_context_loss(AssetManKey);
  void on_gpu_context_restore(AssetManKey);

  template <AssetIdLike T>
  void swap_image(T id1,T id2);
  template <AssetIdLike T>
  void swap_texture(T id1,T id2);
  template <AssetIdLike T>
  void swap_sprite(T id1,T id2);
  template <AssetIdLike T>
  void swap_sprite_atlas(T id1,T id2);
  template <AssetIdLike T>
  void swap_font_atlas(T id1,T id2);

  template <AssetIdLike T>
  void swap_music(T id1,T id2);
  template <AssetIdLike T>
  void swap_sound(T id1,T id2);

  template <AssetIdLike T>
  Image& image(T id);
  template <AssetIdLike T>
  Texture& texture(T id);
  template <AssetIdLike T>
  Sprite& sprite(T id);
  template <AssetIdLike T>
  SpriteAtlas& sprite_atlas(T id);
  template <AssetIdLike T>
  FontAtlas& font_atlas(T id);

  template <AssetIdLike T>
  Music* music(T id);
  template <AssetIdLike T>
  Sound* sound(T id);

  friend class CpuGfxLoader;
  friend class GpuGfxLoader;
  friend class AudioLoader;

private:
  template <typename T>
  using AssetPtr = std::unique_ptr<T>;
  template <typename T>
  using Assets = std::vector<AssetPtr<T>>;

  template <typename T>
  class AssetBag {
  public:
    const char* name = "";
    Assets<T> assets{};
    bool has_assets = false; // Because can't use -1 in `max_id` (unsigned) to indicate this.
    asset_id_t max_id = 0;

    explicit AssetBag(const char* name,std::size_t init_size,bool is_ghost = false);
  };

  bool is_audio_alive_ = false;
  std::shared_ptr<AssetLoader> asset_loader_{};

  std::tuple<
    AssetBag<Image>,
    AssetBag<Texture>,
    AssetBag<Sprite>,
    AssetBag<SpriteAtlas>,
    AssetBag<FontAtlas>
  > gfx_asset_bags_{
    AssetBag<Image>{"Image",8},
    AssetBag<Texture>{"Texture",64},
    AssetBag<Sprite>{"Sprite",64},
    AssetBag<SpriteAtlas>{"SpriteAtlas",16},
    AssetBag<FontAtlas>{"FontAtlas",8},
  };
  std::tuple<
    AssetBag<Music>,
    AssetBag<Sound>
  > audio_asset_bags_{
    AssetBag<Music>{"Music",8},
    AssetBag<Sound>{"Sound",16},
  };

  // Anonymous assets.
  std::tuple<
    AssetBag<Texture>
  > ghost_gfx_asset_bags_{
    AssetBag<Texture>{"Ghost Texture",64,true},
  };

  Image missing_image_{Size2i{64,64},Color4f::kMagenta};
  Texture missing_texture_{AssetManKey{},missing_image_};
  std::tuple<
    AssetPtr<Sprite>,
    AssetPtr<SpriteAtlas>,
    AssetPtr<FontAtlas>
  > missing_gfx_assets_{
    std::make_unique<Sprite>(AssetManKey{},missing_texture_,Sprite::Config{}),
    std::make_unique<SpriteAtlas>(AssetManKey{},missing_texture_,SpriteAtlas::Config{}),
    std::make_unique<FontAtlas>(AssetManKey{},missing_texture_,FontAtlas::Config{}),
  };

  template <typename... Ts,typename AssetBags>
  void reset_assets(AssetBags& asset_bags);
  template <typename... Ts,typename AssetBags>
  void reset_ghost_assets(AssetBags& asset_bags);
  template <typename... Ts,typename AssetBags>
  void shrink_assets(AssetBags& asset_bags);
  template <typename... Ts,typename AssetBags>
  void shrink_ghost_assets(AssetBags& asset_bags);
  template <typename... Ts,typename AssetBags>
  void check_assets(const AssetBags& asset_bags) const;

  template <typename T,typename... Args>
  T& load_gfx_asset(asset_id_t id,Args&&... args);
  template <typename T,typename... Args>
  T* load_audio_asset(asset_id_t id,Args&&... args);
  template <typename T,typename... Args>
  T* load_asset(AssetBag<T>& asset_bag,asset_id_t id,Args&&... args);
  template <typename T,typename... Args>
  T& load_ghost_gfx_asset(Args&&... args);

  template <typename T>
  void swap_gfx_asset(asset_id_t id1,asset_id_t id2);
  template <typename T>
  void swap_audio_asset(asset_id_t id1,asset_id_t id2);

  template <typename T>
  T& gfx_asset(asset_id_t id);
  template <typename T>
  T* audio_asset(asset_id_t id);
};

// Better than KeyedAsset, which would require passing in the AssetManKey.
template <typename T,typename... Args>
concept UnkeyedAsset = std::constructible_from<T,Args...>;

template <typename... Ts,typename AssetBags>
void AssetMan::reset_assets(AssetBags& asset_bags) {
  ([&asset_bags]() {
    auto& asset_bag = std::get<AssetBag<Ts>>(asset_bags);
    auto& assets = asset_bag.assets;

    // Nullify assets.
    std::ranges::fill(assets,nullptr);

    asset_bag.has_assets = false;
    asset_bag.max_id = 0;
  }(),...);
}

template <typename... Ts,typename AssetBags>
void AssetMan::reset_ghost_assets(AssetBags& asset_bags) {
  ([&asset_bags]() {
    auto& asset_bag = std::get<AssetBag<Ts>>(asset_bags);
    auto& assets = asset_bag.assets;

    assets.clear();

    asset_bag.has_assets = false;
    asset_bag.max_id = 0;
  }(),...);
}

template <typename... Ts,typename AssetBags>
void AssetMan::shrink_assets(AssetBags& asset_bags) {
  ([&asset_bags]() {
    auto& asset_bag = std::get<AssetBag<Ts>>(asset_bags);
    auto& assets = asset_bag.assets;

    assets.resize(asset_bag.has_assets ? (asset_bag.max_id + 1) : 0);
    assets.shrink_to_fit();
  }(),...);
}

template <typename... Ts,typename AssetBags>
void AssetMan::shrink_ghost_assets(AssetBags& asset_bags) {
  (std::get<AssetBag<Ts>>(asset_bags).assets.shrink_to_fit(),...);
}

template <typename... Ts,typename AssetBags>
void AssetMan::check_assets([[maybe_unused]] const AssetBags& asset_bags) const {
#if !defined(NDEBUG)
  ([&asset_bags]() {
    auto& asset_bag = std::get<AssetBag<Ts>>(asset_bags);
    auto& assets = asset_bag.assets;

    if(!asset_bag.has_assets) { return; }

    for(std::size_t id = 0; id <= asset_bag.max_id; ++id) {
      if(id >= assets.size() || !assets[id]) {
        std::cerr << "[WARN] " << asset_bag.name << " asset ID `" << id << "` was not loaded." << std::endl;
      }
    }
  }(),...);
#endif
}

template <typename T,typename... Args>
T& AssetMan::load_gfx_asset(asset_id_t id,Args&&... args) {
  return *load_asset(std::get<AssetBag<T>>(gfx_asset_bags_),id,std::forward<Args>(args)...);
}

template <typename T,typename... Args>
T* AssetMan::load_audio_asset(asset_id_t id,Args&&... args) {
  if(!is_audio_alive_) { return nullptr; }

  return load_asset(std::get<AssetBag<T>>(audio_asset_bags_),id,std::forward<Args>(args)...);
}

template <typename T,typename... Args>
T* AssetMan::load_asset(AssetBag<T>& asset_bag,asset_id_t id,Args&&... args) {
  auto& assets = asset_bag.assets;

  asset_bag.has_assets = true;
  if(id > asset_bag.max_id) { asset_bag.max_id = id; }

  Util::grow_for_index(assets,id);
  auto& asset_ptr = assets[id];

  if constexpr(UnkeyedAsset<T,Args...>) {
    asset_ptr = std::make_unique<T>(std::forward<Args>(args)...);
  } else {
    asset_ptr = std::make_unique<T>(AssetManKey{},std::forward<Args>(args)...);
  }

  return asset_ptr.get();
}

template <typename T,typename... Args>
T& AssetMan::load_ghost_gfx_asset(Args&&... args) {
  auto& asset_bag = std::get<AssetBag<T>>(ghost_gfx_asset_bags_);
  auto& assets = asset_bag.assets;

  AssetPtr<T> asset_ptr{};

  if constexpr(UnkeyedAsset<T,Args...>) {
    asset_ptr = std::make_unique<T>(std::forward<Args>(args)...);
  } else {
    asset_ptr = std::make_unique<T>(AssetManKey{},std::forward<Args>(args)...);
  }

  T& asset = *asset_ptr;
  assets.push_back(std::move(asset_ptr));

  if(asset_bag.has_assets) {
    ++asset_bag.max_id;
  } else {
    asset_bag.has_assets = true;
  }

  return asset;
}

template <AssetIdLike T>
void AssetMan::swap_image(T id1,T id2) {
  swap_gfx_asset<Image>(static_cast<asset_id_t>(id1),static_cast<asset_id_t>(id2));
}

template <AssetIdLike T>
void AssetMan::swap_texture(T id1,T id2) {
  swap_gfx_asset<Texture>(static_cast<asset_id_t>(id1),static_cast<asset_id_t>(id2));
}

template <AssetIdLike T>
void AssetMan::swap_sprite(T id1,T id2) {
  swap_gfx_asset<Sprite>(static_cast<asset_id_t>(id1),static_cast<asset_id_t>(id2));
}

template <AssetIdLike T>
void AssetMan::swap_sprite_atlas(T id1,T id2) {
  swap_gfx_asset<SpriteAtlas>(static_cast<asset_id_t>(id1),static_cast<asset_id_t>(id2));
}

template <AssetIdLike T>
void AssetMan::swap_font_atlas(T id1,T id2) {
  swap_gfx_asset<FontAtlas>(static_cast<asset_id_t>(id1),static_cast<asset_id_t>(id2));
}

template <AssetIdLike T>
void AssetMan::swap_music(T id1,T id2) {
  swap_audio_asset<Music>(static_cast<asset_id_t>(id1),static_cast<asset_id_t>(id2));
}

template <AssetIdLike T>
void AssetMan::swap_sound(T id1,T id2) {
  swap_audio_asset<Sound>(static_cast<asset_id_t>(id1),static_cast<asset_id_t>(id2));
}

template <typename T>
void AssetMan::swap_gfx_asset(asset_id_t id1,asset_id_t id2) {
  auto& asset_bag = std::get<AssetBag<T>>(gfx_asset_bags_);
  auto& assets = asset_bag.assets;

  if(id1 >= assets.size() || !assets[id1]) {
    std::cerr << "[WARN] Failed to swap " << asset_bag.name << " assets with invalid "
              << asset_bag.name << " asset ID1 `" << id1 << "`." << std::endl;
    return;
  }
  if(id2 >= assets.size() || !assets[id2]) {
    std::cerr << "[WARN] Failed to swap " << asset_bag.name << " assets with invalid "
              << asset_bag.name << " asset ID2 `" << id2 << "`." << std::endl;
    return;
  }

  std::ranges::swap(assets[id1],assets[id2]);
}

template <typename T>
void AssetMan::swap_audio_asset(asset_id_t id1,asset_id_t id2) {
  if(!is_audio_alive_) { return; }

  auto& asset_bag = std::get<AssetBag<T>>(audio_asset_bags_);
  auto& assets = asset_bag.assets;

  if(id1 >= assets.size()) {
    std::cerr << "[WARN] Failed to swap " << asset_bag.name << " assets with invalid "
              << asset_bag.name << " asset ID1 `" << id1 << "`." << std::endl;
    return;
  }
  if(id2 >= assets.size()) {
    std::cerr << "[WARN] Failed to swap " << asset_bag.name << " assets with invalid "
              << asset_bag.name << " asset ID2 `" << id2 << "`." << std::endl;
    return;
  }

  std::ranges::swap(assets[id1],assets[id2]);
}

template <AssetIdLike T>
Image& AssetMan::image(T id) { return gfx_asset<Image>(static_cast<asset_id_t>(id)); }

template <AssetIdLike T>
Texture& AssetMan::texture(T id) { return gfx_asset<Texture>(static_cast<asset_id_t>(id)); }

template <AssetIdLike T>
Sprite& AssetMan::sprite(T id) { return gfx_asset<Sprite>(static_cast<asset_id_t>(id)); }

template <AssetIdLike T>
SpriteAtlas& AssetMan::sprite_atlas(T id) { return gfx_asset<SpriteAtlas>(static_cast<asset_id_t>(id)); }

template <AssetIdLike T>
FontAtlas& AssetMan::font_atlas(T id) { return gfx_asset<FontAtlas>(static_cast<asset_id_t>(id)); }

template <AssetIdLike T>
Music* AssetMan::music(T id) { return audio_asset<Music>(static_cast<asset_id_t>(id)); }

template <AssetIdLike T>
Sound* AssetMan::sound(T id) { return audio_asset<Sound>(static_cast<asset_id_t>(id)); }

template <typename T>
T& AssetMan::gfx_asset(asset_id_t id) {
  auto& asset_bag = std::get<AssetBag<T>>(gfx_asset_bags_);
  auto& assets = asset_bag.assets;

#if defined(NDEBUG)
  if(id >= assets.size() || !assets[id]) [[unlikely]] {
    if constexpr(std::same_as<T,Image>) {
      return missing_image_;
    } else if constexpr(std::same_as<T,Texture>) {
      return missing_texture_;
    } else {
      return *std::get<AssetPtr<T>>(missing_gfx_assets_);
    }
  }
#else // Debug
  if(id >= assets.size() || !assets[id]) [[unlikely]] {
    throw CybelError{"Invalid ",asset_bag.name," asset ID `",id,"` or was not loaded."};
  }
#endif

  return *assets[id];
}

template <typename T>
T* AssetMan::audio_asset(asset_id_t id) {
  if(!is_audio_alive_) [[unlikely]] { return nullptr; }

  auto& asset_bag = std::get<AssetBag<T>>(audio_asset_bags_);
  auto& assets = asset_bag.assets;

  if(id >= assets.size()) [[unlikely]] {
    std::cerr << "[WARN] Invalid " << asset_bag.name << " asset ID `" << id << "`.";
    return nullptr;
  }

  return assets[id].get();
}

template <typename T>
AssetMan::AssetBag<T>::AssetBag(const char* name,std::size_t init_size,bool is_ghost)
  : name{name} {
  if(is_ghost) {
    assets.reserve(init_size);
  } else {
    assets.resize(init_size);
  }
}

} // namespace cybel
#endif
