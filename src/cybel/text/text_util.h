/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TEXT_TEXT_UTIL_H_
#define CYBEL_TEXT_TEXT_UTIL_H_

#include "cybel/common.h"

#include <format>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace cybel {

namespace TextUtil {
  template <typename... Args>
  constexpr std::string fmt(std::format_string<Args...> fmt_str,Args&&... args);
  template <typename... Args>
  std::string build_str(Args&&... args);

  int casecmp_ascii(std::string_view str1,std::string_view str2);
  std::size_t count_runes(std::string_view str);
  std::string ellipsize(std::string_view str,std::size_t max_len);
  std::string ljust(std::string_view str,std::size_t new_len);
  std::string_view strip(std::string_view str);
  std::string wrap_words(std::string_view str,std::size_t max_len);
}

template <typename... Args>
constexpr std::string TextUtil::fmt(std::format_string<Args...> fmt_str,Args&&... args) {
  return std::format(fmt_str,std::forward<Args>(args)...);
}

template <typename... Args>
std::string TextUtil::build_str(Args&&... args) {
  std::ostringstream buffer{};

  // See printer() example at bottom:
  // - https://en.cppreference.com/w/cpp/language/fold
  (buffer << ... << std::forward<Args>(args));

  return buffer.str();
}

} // namespace cybel
#endif
