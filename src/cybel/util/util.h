/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_UTIL_UTIL_H_
#define CYBEL_UTIL_UTIL_H_

#include "cybel/common.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace cybel {

namespace Util {
  template <typename... Args>
  std::string build_str(const Args&... args);
  template <typename... Args>
  std::size_t build_hash(const Args&... args);

  template <typename T>
  void grow_for_index(std::vector<T>& vec,std::size_t index);
  template <typename T>
  std::vector<T> unique(const std::vector<T>& vec);

  void clear_gl_errors();

  std::string get_sdl_error();
  std::string get_sdl_img_error();
  std::string get_sdl_mix_error();
  std::string get_gl_error(GLenum error);
  std::string get_glew_error(GLenum error);
  std::string get_emscripten_result(int result);
}

template <typename... Args>
std::string Util::build_str(const Args&... args) {
  // See Example at bottom:
  // - https://en.cppreference.com/w/cpp/language/fold
  std::ostringstream ss{};
  (ss << ... << args);

  return ss.str();
}

template <typename... Args>
std::size_t Util::build_hash(const Args&... args) {
  std::size_t seed = 0;
  ((seed ^= std::hash<Args>{}(args) + 0x9e3779b9 + (seed << 6) + (seed >> 2)),... );

  return seed;
}

template <typename T>
void Util::grow_for_index(std::vector<T>& vec,std::size_t index) {
  if(index < vec.size()) { return; }

  const auto new_size = static_cast<std::size_t>(static_cast<double>(index + 1) * 1.5);
  vec.resize(new_size);

  assert(index < vec.size());
}

template <typename T>
std::vector<T> Util::unique(const std::vector<T>& vec) {
  if(vec.size() <= 1) { return vec; }

  std::unordered_set<T> seen{};
  std::vector<T> unique{};

  seen.reserve(vec.size());
  unique.reserve(vec.size());

  std::ranges::copy_if(vec,std::back_inserter(unique),[&seen](const auto& e) {
    // Is unique? (Inserted for the first time?)
    return seen.insert(e).second;
  });

  unique.shrink_to_fit();
  return unique;
}

} // namespace cybel
#endif
