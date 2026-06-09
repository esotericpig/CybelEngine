/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TEXT_RUNE_TOKEN_H_
#define CYBEL_TEXT_RUNE_TOKEN_H_

#include "cybel/common.h"

#include "cybel/text/rune_util.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace cybel {

class RuneToken final {
public:
  char32_t rune = RuneUtil::kInvalidRune;
  std::string_view view{};

  std::size_t index = 0;
  std::uint8_t byte_count = 0;

  std::string to_str() const;

  constexpr char byte() const;
};

constexpr char RuneToken::byte() const {
  if(view.empty()) [[unlikely]] { return 0; }

  return view[0];
}

} // namespace cybel
#endif
