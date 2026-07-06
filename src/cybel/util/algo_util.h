/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_UTIL_ALGO_UTIL_H_
#define CYBEL_UTIL_ALGO_UTIL_H_

#include "cybel/common.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <unordered_set>
#include <vector>

namespace cybel {

namespace AlgoUtil {
  template <typename... Args>
  std::size_t build_hash(const Args&... args);

  template <typename T>
  void grow_for_index(std::vector<T>& vec,std::size_t index);
  template <typename T>
  std::string join_with(const std::vector<T>& vec,std::string_view separator);
  template <typename T>
  std::vector<T> unique(const std::vector<T>& vec);
}

template <typename... Args>
std::size_t AlgoUtil::build_hash(const Args&... args) {
  std::size_t seed = 0;

  ((seed ^= std::hash<Args>{}(args) + 0x9e3779b9 + (seed << 6) + (seed >> 2)),... );

  return seed;
}

template <typename T>
void AlgoUtil::grow_for_index(std::vector<T>& vec,std::size_t index) {
  if(index < vec.size()) { return; }

  const auto new_size = static_cast<std::size_t>(static_cast<double>(index + 1) * 1.5);
  vec.resize(new_size);

  assert(index < vec.size());
}

template <typename T>
std::string AlgoUtil::join_with(const std::vector<T>& vec,std::string_view separator) {
  if(vec.empty()) { return ""; }

  std::string result{};
  auto it = vec.cbegin();

  result += *it;

  for(++it; it != vec.cend(); ++it) {
    result += separator;
    result += *it;
  }

  return result;
}

template <typename T>
std::vector<T> AlgoUtil::unique(const std::vector<T>& vec) {
  if(vec.size() <= 1) { return vec; }

  std::unordered_set<T> seen{};
  std::vector<T> unique{};

  seen.reserve(vec.size());
  unique.reserve(vec.size());

  std::ranges::copy_if(vec,std::back_inserter(unique),[&seen](const auto& e) {
    // Inserted for the first time? (Unique?)
    return seen.insert(e).second;
  });

  unique.shrink_to_fit();
  return unique;
}

} // namespace cybel
#endif
