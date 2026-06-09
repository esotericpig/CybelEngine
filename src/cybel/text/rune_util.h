/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TEXT_RUNE_UTIL_H_
#define CYBEL_TEXT_RUNE_UTIL_H_

#include "cybel/common.h"

namespace cybel {

using octet_t = unsigned char;

/// UTF-8 links:
/// - https://datatracker.ietf.org/doc/html/rfc3629#autoid-3
/// - https://datatracker.ietf.org/doc/html/rfc3629#autoid-4
/// - https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G31703
/// - https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G48534
/// - https://www.unicode.org/Public/latest/ucd/PropList.txt
namespace RuneUtil {
  constexpr char32_t kInvalidRune = U'�'; /// \uFFFD.
  inline const auto* kInvalidPackedRune = "�"; /// \xEF\xBF\xBD.

  constexpr octet_t kMaxAsciiOctet = 0x7F; /// 0b0111'1111.
  constexpr octet_t kMinTailOctet = 0x80; /// 0b1000'0000.
  constexpr octet_t kMaxTailOctet = 0xBF; /// 0b1011'1111.
  constexpr octet_t kTailOctetMask = 0b0011'1111; /// 0b10xxxxxx.

  char32_t next_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count);
  char32_t prev_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count);

  std::string pack(char32_t rune);

  bool is_whitespace(char32_t rune);
}

} // namespace cybel
#endif
