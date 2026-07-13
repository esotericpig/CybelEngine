/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_ASSET_ASSET_MAN_H_
#define CYBEL_ASSET_ASSET_MAN_H_

#include "cybel/common.h"

#include "cybel/asset/asset_loader_key.h"
#include "cybel/asset/asset_man_key.h"
#include "cybel/asset/asset_types.h"
#include "cybel/audio/music.h"
#include "cybel/audio/sound.h"
#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/gpu_context_key.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"
#include "cybel/text/text_util.h"
#include "cybel/types/color.h"
#include "cybel/types/cybel_error.h"
#include "cybel/util/algo_util.h"

#include <algorithm>
#include <concepts>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <vector>

namespace cybel {

class AssetLoader;

class AssetMan final {
public:
  explicit AssetMan(bool is_audio_alive);

  void add_asset_dir(const std::filesystem::path& dir);
  void add_asset_dirs(std::initializer_list<std::filesystem::path> dirs,
                      const std::filesystem::path& sub_dir = "");

  /// If your Game doesn't use assets, pass in null to shrink the assets,
  /// or call shrink_assets() directly.
  void load_assets(std::shared_ptr<AssetLoader> asset_loader);
  void reload_assets();
  void shrink_assets();

  void reload_gfx();
  void reload_cpu_gfx();
  void reload_gpu_gfx();

  void reload_audio();

  template <typename T,AssetIdLike Id,typename... Args>
  T& load_gfx_asset(AssetLoaderKey,Id id,const std::filesystem::path& file,Args&&... args);
  template <typename T,AssetIdLike Id,typename... Args>
  T& load_gfx_asset(AssetLoaderKey,Id id,Args&&... args);

  template <typename T,AssetIdLike Id,typename... Args>
  T* load_audio_asset(AssetLoaderKey,Id id,const std::filesystem::path& file,Args&&... args);

  template <typename T,typename... Args>
  T load_temp_gfx_asset(AssetLoaderKey,const std::filesystem::path& file,Args&&... args);

  template <typename T,typename... Args>
  T& load_ghost_gfx_asset(AssetLoaderKey,Args&&... args);

  void on_gpu_context_loss(GpuContextKey);
  void on_gpu_context_restore(GpuContextKey);

  template <AssetIdLike Id>
  void swap_image(Id id1,Id id2);
  template <AssetIdLike Id>
  void swap_texture(Id id1,Id id2);
  template <AssetIdLike Id>
  void swap_sprite(Id id1,Id id2);
  template <AssetIdLike Id>
  void swap_sprite_atlas(Id id1,Id id2);
  template <AssetIdLike Id>
  void swap_font_atlas(Id id1,Id id2);

  template <AssetIdLike Id>
  void swap_music(Id id1,Id id2);
  template <AssetIdLike Id>
  void swap_sound(Id id1,Id id2);

  void set_fail_on_audio_error(bool fail);

  const std::vector<std::filesystem::path>& asset_dirs() const;

  template <AssetIdLike Id>
  Image& image(Id id);
  template <AssetIdLike Id>
  Texture& texture(Id id);
  template <AssetIdLike Id>
  Sprite& sprite(Id id);
  template <AssetIdLike Id>
  SpriteAtlas& sprite_atlas(Id id);
  template <AssetIdLike Id>
  FontAtlas& font_atlas(Id id);

  template <AssetIdLike Id>
  Music* music(Id id);
  template <AssetIdLike Id>
  Sound* sound(Id id);

private:
  template <typename T>
  using AssetPtr = std::unique_ptr<T>;
  template <typename T>
  using Assets = std::vector<AssetPtr<T>>;

  template <typename T>
  class AssetBag final {
  public:
    const char* name = "";
    bool is_ghost{};
    Assets<T> assets{};
    std::size_t count = 0;

    explicit AssetBag(const char* name,std::size_t init_size,bool is_ghost = false);

    void reset();
    void shrink();
    void check() const;
    void zombify();

    /// For Ghost assets, `id` is unused, but this is safer than creating an overload
    /// because it prevents non-Ghost assets from calling the wrong function.
    template <AssetIdLike Id>
    T* add(Id id_like,AssetPtr<T> asset_ptr);
  };

  template <typename... >
  struct AssetTypes final {};

  using CpuAssetTypes = AssetTypes<Image>;
  using GpuAssetTypes = AssetTypes<Texture,Sprite,SpriteAtlas,FontAtlas>;
  using AudioAssetTypes = AssetTypes<Music,Sound>;

  using GhostGpuAssetTypes = AssetTypes<Texture>;

  using ZombieGpuAssetTypes = AssetTypes<Texture>;

  std::shared_ptr<AssetLoader> asset_loader_{};
  std::vector<std::filesystem::path> asset_dirs_{}; // Insertion order matters.

  bool is_audio_alive_ = false;
  bool fail_on_audio_error_ = true;

  std::tuple<
    AssetBag<Image>,

    AssetBag<Texture>,
    AssetBag<Sprite>,
    AssetBag<SpriteAtlas>,
    AssetBag<FontAtlas>,

    AssetBag<Music>,
    AssetBag<Sound>
  > asset_bags_{
    AssetBag<Image>{"Image",8},

    AssetBag<Texture>{"Texture",64},
    AssetBag<Sprite>{"Sprite",64},
    AssetBag<SpriteAtlas>{"SpriteAtlas",16},
    AssetBag<FontAtlas>{"FontAtlas",8},

    AssetBag<Music>{"Music",8},
    AssetBag<Sound>{"Sound",16},
  };

  // Anonymous assets.
  std::tuple<
    AssetBag<Texture>
  > ghost_asset_bags_{
    AssetBag<Texture>{"Ghost Texture",64,true},
  };

  Image missing_image_{Size2i{64,64},Color4f::kMagenta};
  Texture missing_texture_{AssetManKey{},missing_image_};
  std::tuple<
    AssetPtr<Sprite>,
    AssetPtr<SpriteAtlas>,
    AssetPtr<FontAtlas>
  > missing_gfx_assets_{
    make_asset_ptr<Sprite>(missing_texture_,Sprite::Config{}),
    make_asset_ptr<SpriteAtlas>(missing_texture_,SpriteAtlas::Config{}),
    make_asset_ptr<FontAtlas>(missing_texture_,FontAtlas::Config{}),
  };

  template <typename... Ts,typename AssetBags>
  void reset_assets(AssetTypes<Ts...>,AssetBags& asset_bags);
  template <typename... Ts,typename AssetBags>
  void shrink_assets(AssetTypes<Ts...>,AssetBags& asset_bags);
  template <typename... Ts,typename AssetBags>
  void check_assets(AssetTypes<Ts...>,const AssetBags& asset_bags) const;
  template <typename... Ts,typename AssetBags>
  void zombify_assets(AssetTypes<Ts...>,AssetBags& asset_bags);

  template <typename T,AssetIdLike Id,typename AssetBags,typename... Args>
  T* load_asset_file(AssetBags& asset_bags,Id id,const std::filesystem::path& file,Args&&... args);
  template <typename T,AssetIdLike Id,typename AssetBags,typename... Args>
  T* load_asset(AssetBags& asset_bags,Id id,Args&&... args);

  template <std::invocable<const std::filesystem::path&> OnAssetFile>
  decltype(auto) load_asset_file_in_dirs(std::string_view name,const std::filesystem::path& file,
                                         OnAssetFile&& on_asset_file);
  template <typename T,typename... Args>
  AssetPtr<T> make_asset_ptr(Args&&... args);
  template <typename T,typename... Args>
  T make_asset(Args&&... args);

  template <typename T,AssetIdLike Id>
  void swap_gfx_asset(Id id_like1,Id id_like2);
  template <typename T,AssetIdLike Id>
  void swap_audio_asset(Id id_like1,Id id_like2);

  template <typename T,AssetIdLike Id>
  T& gfx_asset(Id id_like);
  template <typename T,AssetIdLike Id>
  T* audio_asset(Id id_like);
};

// Better than KeyedAsset, which would require passing in the AssetManKey.
template <typename T,typename... Args>
concept UnkeyedAsset = std::constructible_from<T,Args...>;

template <typename... Ts,typename AssetBags>
void AssetMan::reset_assets(AssetTypes<Ts...>,AssetBags& asset_bags) {
  (std::get<AssetBag<Ts>>(asset_bags).reset(),...);
}

template <typename... Ts,typename AssetBags>
void AssetMan::shrink_assets(AssetTypes<Ts...>,AssetBags& asset_bags) {
  (std::get<AssetBag<Ts>>(asset_bags).shrink(),...);
}

template <typename... Ts,typename AssetBags>
void AssetMan::check_assets(AssetTypes<Ts...>,[[maybe_unused]] const AssetBags& asset_bags) const {
#if !defined(NDEBUG)
  (std::get<AssetBag<Ts>>(asset_bags).check(),...);
#endif
}

template <typename... Ts,typename AssetBags>
void AssetMan::zombify_assets(AssetTypes<Ts...>,AssetBags& asset_bags) {
  (std::get<AssetBag<Ts>>(asset_bags).zombify(),...);
}

template <typename T,AssetIdLike Id,typename... Args>
T& AssetMan::load_gfx_asset(AssetLoaderKey,Id id,const std::filesystem::path& file,Args&&... args) {
  return *load_asset_file<T,Id>(asset_bags_,id,file,std::forward<Args>(args)...);
}

template <typename T,AssetIdLike Id,typename... Args>
T& AssetMan::load_gfx_asset(AssetLoaderKey,Id id,Args&&... args) {
  return *load_asset<T,Id>(asset_bags_,id,std::forward<Args>(args)...);
}

template <typename T,AssetIdLike Id,typename... Args>
T* AssetMan::load_audio_asset(AssetLoaderKey,Id id,const std::filesystem::path& file,Args&&... args) {
  if(!is_audio_alive_) { return nullptr; }

  try {
    return load_asset_file<T,Id>(asset_bags_,id,file,std::forward<Args>(args)...);
  } catch(const CybelError& e) {
    if(fail_on_audio_error_) { throw; }

    std::cerr << "[WARN] " << e.what() << '\n';
  }

  return nullptr;
}

template <typename T,typename... Args>
T AssetMan::load_temp_gfx_asset(AssetLoaderKey,const std::filesystem::path& file,Args&&... args) {
  const auto& asset_bag = std::get<AssetBag<T>>(asset_bags_);

  return load_asset_file_in_dirs(asset_bag.name,file,[&](const auto& asset_file) {
    return make_asset<T>(asset_file,std::forward<Args>(args)...);
  });
}

template <typename T,typename... Args>
T& AssetMan::load_ghost_gfx_asset(AssetLoaderKey,Args&&... args) {
  constexpr bool has_file_arg = ((std::same_as<std::remove_cvref_t<Args>,std::filesystem::path> || ...));
  static_assert(!has_file_arg,"No Ghost assets currently load files.");

  auto& asset_bag = std::get<AssetBag<T>>(ghost_asset_bags_);

  return *asset_bag.add(0,make_asset_ptr<T>(std::forward<Args>(args)...));
}

template <typename T,AssetIdLike Id,typename AssetBags,typename... Args>
T* AssetMan::load_asset_file(AssetBags& asset_bags,Id id,const std::filesystem::path& file,Args&&... args) {
  auto& asset_bag = std::get<AssetBag<T>>(asset_bags);

  return load_asset_file_in_dirs(asset_bag.name,file,[&](const auto& asset_file) {
    return asset_bag.template add<Id>(id,make_asset_ptr<T>(asset_file,std::forward<Args>(args)...));
  });
}

template <typename T,AssetIdLike Id,typename AssetBags,typename... Args>
T* AssetMan::load_asset(AssetBags& asset_bags,Id id,Args&&... args) {
  constexpr bool has_file_arg = ((std::same_as<std::remove_cvref_t<Args>,std::filesystem::path> || ...));
  static_assert(!has_file_arg,"Use load_asset_file() instead.");

  auto& asset_bag = std::get<AssetBag<T>>(asset_bags);

  return asset_bag.template add<Id>(id,make_asset_ptr<T>(std::forward<Args>(args)...));
}

template <std::invocable<const std::filesystem::path&> OnAssetFile>
decltype(auto) AssetMan::load_asset_file_in_dirs(std::string_view name,const std::filesystem::path& file,
                                                 OnAssetFile&& on_asset_file) {
  if(asset_dirs_.empty() || !file.is_relative()) {
    return std::forward<OnAssetFile>(on_asset_file)(file);
  }
  if(asset_dirs_.size() == 1) {
    return std::forward<OnAssetFile>(on_asset_file)(asset_dirs_[0] / file);
  }

  std::string sdl_errors{};

  for(const auto& asset_dir : asset_dirs_) {
    try {
      const auto asset_file = asset_dir / file;
      std::error_code ec{};

      if(!is_regular_file(asset_file,ec)) { continue; }

      SDL_ClearError();
      return std::forward<OnAssetFile>(on_asset_file)(asset_file);
    } catch(const CybelError& e) {
      std::cerr << "[WARN] " << e.what() << '\n';

      const std::string_view sdl_error = SDL_GetError(); // Covers all gfx & audio.

      if(!sdl_error.empty()) {
        sdl_errors += "\n- ";
        sdl_errors += sdl_error;
      }
    }
  }

  std::string err_msg = TextUtil::fmt(
    "Failed to load {} asset file `{}` in any asset folder [{}].",
    name,file.string(),AlgoUtil::join_with(asset_dirs_,", ")
  );

  if(!sdl_errors.empty()) {
    err_msg += '\n';
    err_msg += sdl_errors;
  }

  throw CybelError{err_msg};
}

template <typename T,typename... Args>
AssetMan::AssetPtr<T> AssetMan::make_asset_ptr(Args&&... args) {
  if constexpr(UnkeyedAsset<T,Args...>) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  } else {
    return std::make_unique<T>(AssetManKey{},std::forward<Args>(args)...);
  }
}

template <typename T,typename... Args>
T AssetMan::make_asset(Args&&... args) {
  if constexpr(UnkeyedAsset<T,Args...>) {
    return T(std::forward<Args>(args)...);
  } else {
    return T(AssetManKey{},std::forward<Args>(args)...);
  }
}

template <AssetIdLike Id>
void AssetMan::swap_image(Id id1,Id id2) {
  swap_gfx_asset<Image,Id>(id1,id2);
}

template <AssetIdLike Id>
void AssetMan::swap_texture(Id id1,Id id2) {
  swap_gfx_asset<Texture,Id>(id1,id2);
}

template <AssetIdLike Id>
void AssetMan::swap_sprite(Id id1,Id id2) {
  swap_gfx_asset<Sprite,Id>(id1,id2);
}

template <AssetIdLike Id>
void AssetMan::swap_sprite_atlas(Id id1,Id id2) {
  swap_gfx_asset<SpriteAtlas,Id>(id1,id2);
}

template <AssetIdLike Id>
void AssetMan::swap_font_atlas(Id id1,Id id2) {
  swap_gfx_asset<FontAtlas,Id>(id1,id2);
}

template <AssetIdLike Id>
void AssetMan::swap_music(Id id1,Id id2) {
  swap_audio_asset<Music,Id>(id1,id2);
}

template <AssetIdLike Id>
void AssetMan::swap_sound(Id id1,Id id2) {
  swap_audio_asset<Sound,Id>(id1,id2);
}

template <typename T,AssetIdLike Id>
void AssetMan::swap_gfx_asset(Id id_like1,Id id_like2) {
  const auto id1 = static_cast<asset_id_t>(id_like1);
  const auto id2 = static_cast<asset_id_t>(id_like2);

  auto& asset_bag = std::get<AssetBag<T>>(asset_bags_);
  auto& assets = asset_bag.assets;

  if(id1 >= assets.size() || !assets[id1]) {
    std::cerr << "[WARN] Failed to swap " << asset_bag.name << " assets with invalid "
              << asset_bag.name << " asset ID1 `" << id1 << "`.\n";
    return;
  }
  if(id2 >= assets.size() || !assets[id2]) {
    std::cerr << "[WARN] Failed to swap " << asset_bag.name << " assets with invalid "
              << asset_bag.name << " asset ID2 `" << id2 << "`.\n";
    return;
  }

  std::ranges::swap(assets[id1],assets[id2]);
}

template <typename T,AssetIdLike Id>
void AssetMan::swap_audio_asset(Id id_like1,Id id_like2) {
  if(!is_audio_alive_) { return; }

  const auto id1 = static_cast<asset_id_t>(id_like1);
  const auto id2 = static_cast<asset_id_t>(id_like2);

  auto& asset_bag = std::get<AssetBag<T>>(asset_bags_);
  auto& assets = asset_bag.assets;

  if(id1 >= assets.size()) {
    std::cerr << "[WARN] Failed to swap " << asset_bag.name << " assets with invalid "
              << asset_bag.name << " asset ID1 `" << id1 << "`.\n";
    return;
  }
  if(id2 >= assets.size()) {
    std::cerr << "[WARN] Failed to swap " << asset_bag.name << " assets with invalid "
              << asset_bag.name << " asset ID2 `" << id2 << "`.\n";
    return;
  }

  std::ranges::swap(assets[id1],assets[id2]);
}

template <AssetIdLike Id>
Image& AssetMan::image(Id id) { return gfx_asset<Image,Id>(id); }

template <AssetIdLike Id>
Texture& AssetMan::texture(Id id) { return gfx_asset<Texture,Id>(id); }

template <AssetIdLike Id>
Sprite& AssetMan::sprite(Id id) { return gfx_asset<Sprite,Id>(id); }

template <AssetIdLike Id>
SpriteAtlas& AssetMan::sprite_atlas(Id id) { return gfx_asset<SpriteAtlas,Id>(id); }

template <AssetIdLike Id>
FontAtlas& AssetMan::font_atlas(Id id) { return gfx_asset<FontAtlas,Id>(id); }

template <AssetIdLike Id>
Music* AssetMan::music(Id id) { return audio_asset<Music,Id>(id); }

template <AssetIdLike Id>
Sound* AssetMan::sound(Id id) { return audio_asset<Sound,Id>(id); }

template <typename T,AssetIdLike Id>
T& AssetMan::gfx_asset(Id id_like) {
  const auto id = static_cast<asset_id_t>(id_like);

  auto& asset_bag = std::get<AssetBag<T>>(asset_bags_);
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

template <typename T,AssetIdLike Id>
T* AssetMan::audio_asset(Id id_like) {
  if(!is_audio_alive_) [[unlikely]] { return nullptr; }

  const auto id = static_cast<asset_id_t>(id_like);

  auto& asset_bag = std::get<AssetBag<T>>(asset_bags_);
  auto& assets = asset_bag.assets;

  if(id >= assets.size()) [[unlikely]] {
    std::cerr << "[WARN] Invalid " << asset_bag.name << " asset ID `" << id << "`.\n";
    return nullptr;
  }

  return assets[id].get();
}

template <typename T>
AssetMan::AssetBag<T>::AssetBag(const char* name,std::size_t init_size,bool is_ghost)
  : name{name},is_ghost{is_ghost} {
  if(this->is_ghost) {
    assets.reserve(init_size);
  } else {
    assets.resize(init_size);
  }
}

template <typename T>
void AssetMan::AssetBag<T>::reset() {
  if(is_ghost) {
    assets.clear();
  } else {
    // Nullify assets.
    std::ranges::fill(assets,nullptr);
  }

  count = 0;
}

template <typename T>
void AssetMan::AssetBag<T>::shrink() {
  if(is_ghost) {
    assets.shrink_to_fit();
  } else {
    assets.resize(count);
    assets.shrink_to_fit();
  }
}

template <typename T>
void AssetMan::AssetBag<T>::check() const {
#if !defined(NDEBUG)
  for(std::size_t id = 0; id < count; ++id) {
    if(id >= assets.size() || !assets[id]) {
      std::cerr << "[WARN] " << name << " asset ID `" << id << "` was not loaded.\n";
    }
  }
#endif
}

template <typename T>
void AssetMan::AssetBag<T>::zombify() {
  for(auto& asset_ptr : assets) {
    if(asset_ptr) { asset_ptr->zombify(AssetManKey{}); }
  }
}

template <typename T>
template <AssetIdLike Id>
T* AssetMan::AssetBag<T>::add(Id id_like,AssetPtr<T> asset_ptr) {
  const auto id = static_cast<asset_id_t>(id_like);

  T* asset = asset_ptr.get(); // Store before move.

  if(is_ghost) {
    ++count;
    assets.push_back(std::move(asset_ptr));
  } else {
    if(id >= count) {
      count = id + 1;
    }
    if(id >= assets.size()) {
      AlgoUtil::grow_for_index(assets,id);
    }

    assets[id] = std::move(asset_ptr);
  }

  return asset;
}

} // namespace cybel
#endif
