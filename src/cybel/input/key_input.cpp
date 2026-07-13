/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "key_input.h"

#include <array>

namespace cybel {

key_mods_t KeyMods::norm_mods(key_mods_t mods) {
  // - https://wiki.libsdl.org/SDL2/SDL_Keymod
  static constexpr key_mods_t norm_mods = static_cast<key_mods_t>(
    ~(KMOD_CAPS | KMOD_MODE | KMOD_NUM | KMOD_SCROLL)
  );

  return mods & norm_mods;
}

key_mods_t KeyMods::norm_dup_mods(key_mods_t mods) {
  // - https://wiki.libsdl.org/SDL2/SDL_Keymod
  static constexpr std::array<key_mods_t,4> dup_mods{KMOD_ALT,KMOD_CTRL,KMOD_GUI,KMOD_SHIFT};

  if(mods != KMOD_NONE) {
    for(auto dup_mod : dup_mods) {
      // cppcheck-suppress useStlAlgorithm
      if(mods & dup_mod) { mods |= dup_mod; }
    }
  }

  return mods;
}

} // namespace cybel
