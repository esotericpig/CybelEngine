/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_INPUT_KEY_INPUT_H_
#define CYBEL_INPUT_KEY_INPUT_H_

#include "cybel/common.h"

#include "cybel/util/algo_util.h"

namespace cybel {

using key_mods_t = Uint16;

namespace KeyMods {
  /// Ignores Caps Lock, etc.
  key_mods_t norm_mods(key_mods_t mods);

  /// If the left or right versions (duplicates) of the modifier keys are pressed,
  /// makes sure that both the left & right bits are set.
  key_mods_t norm_dup_mods(key_mods_t mods);
}

template <typename T>
class KeyInput {
public:
  class Hash {
  public:
    std::size_t operator()(const KeyInput& ki) const;
  };

  explicit constexpr KeyInput(T key) noexcept;
  explicit constexpr KeyInput(key_mods_t mods,T key) noexcept;

  constexpr bool operator==(const KeyInput& other) const = default;

  constexpr key_mods_t mods() const;
  constexpr T key() const;

private:
  key_mods_t mods_{KMOD_NONE};
  T key_{};
};

using RawKey = SDL_Scancode;
using RawKeyInput = KeyInput<RawKey>;
using SymKey = SDL_KeyCode;
using SymKeyInput = KeyInput<SymKey>;

template <typename T>
std::size_t KeyInput<T>::Hash::operator()(const KeyInput& ki) const {
  return AlgoUtil::build_hash(ki.mods_,ki.key_);
}

template <typename T>
constexpr KeyInput<T>::KeyInput(T key) noexcept
  : key_{key} {}

template <typename T>
constexpr KeyInput<T>::KeyInput(key_mods_t mods,T key) noexcept
  : mods_{KeyMods::norm_mods(mods)},key_{key} {}

template <typename T>
constexpr key_mods_t KeyInput<T>::mods() const { return mods_; }

template <typename T>
constexpr T KeyInput<T>::key() const { return key_; }

} // namespace cybel
#endif
